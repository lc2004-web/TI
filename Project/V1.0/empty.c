/*
 * ========================================================================
 * 巡线小车 — PID 控制
 * ========================================================================
 *
 * 传感器逻辑已修正: # = 黑线, _ = 白色. 追踪黑线.
 *
 * PID:  error = setpoint(0) - position
 *       左轮 = BASE - steering    右轮 = BASE + steering
 *       steering>0 → 左转 (线在左边)
 * ========================================================================
 */

#include "ti_msp_dl_config.h"
#include "BSP/bsp_tb6612.h"
#include "BSP/bsp_encoder.h"
#include "BSP/bsp_delay.h"
#include "BSP/bsp_imu.h"
#include "BSP/bsp_oled.h"
#include "BSP/pid.h"
#include "BSP/line_sensor.h"
#include <string.h>
#include <stdio.h>

/* ========================================================================
 * 控制参数
 * ========================================================================*/

#define KP_LINE           200.0f   /* 比例增益                          */
#define KI_LINE             3.0f   /* 积分增益 (消除稳态偏移)            */
#define KD_LINE            50.0f   /* 微分增益 (抑制过冲, 提前感知)      */
#define POS_DEADZONE       0.6f    /* 测量死区: |pos|<0.6 当 0 处理      */
#define YAW_DAMP           2.5f    /* 偏航角速率阻尼: 车身已在转→自动收力  */

#define PID_INTEGRAL_MAX  300.0f
#define PID_OUTPUT_MAX    700.0f

#define BASE_DUTY           700    /* 基础占空比 (700/4000=17.5%)        */
#define MIN_DUTY            200    /* 最低占空比                         */
#define MOTOR_BALANCE         0    /* 电机平衡: 机械对称置0, 有偏再调     */

#define LOOP_PERIOD_MS       10    /* 控制周期 10ms → 100Hz             */
#define OLED_UPDATE_EVERY    10    /* OLED 每 10 次刷新 (100ms)          */
#define LINE_LOST_TIMEOUT    50    /* 丢线超时 50×10ms = 500ms (停车)   */
#define LINE_LOST_FILTER      5    /* 连续白点数 ≥5 才算丢线 (消抖)      */

#define SENSOR_DEBUG          0    /* 1=传感器调试, 0=巡线               */

/* ========================================================================
 * 全局变量
 * ========================================================================*/
static PID_Controller_t pid_pos;
static int              line_lost_cnt = 0;
static int              white_cnt    = 0;   /* 连续全白计数, 用于丢线消抖   */
static int              imu_ok       = 0;   /* IMU 可用标志                  */
static float            last_position = 0.0f;
static float            yaw_offset   = 0.0f; /* 初始偏航零偏                  */
static float            prev_yaw     = 0.0f; /* 上一帧偏航角(已校零), 用于角速率 */
static volatile uint32_t sys_tick = 0;

/* ========================================================================
 * SysTick
 * ========================================================================*/
void SysTick_Handler(void)
{
    sys_tick++;
}

/* ========================================================================
 * 工具函数
 * ========================================================================*/
static inline int16_t clamp_duty(int16_t val)
{
    if (val > PWM_MAX) return PWM_MAX;
    if (val < -PWM_MAX) return -PWM_MAX;
    return val;
}

static void motor_set(int motor_id, int16_t duty)
{
    if (duty >= 0) {
        tb6612_set_direction(motor_id, MOTOR_CW);
        tb6612_set_duty(motor_id, (uint32_t)duty);
    } else {
        tb6612_set_direction(motor_id, MOTOR_CCW);
        tb6612_set_duty(motor_id, (uint32_t)(-duty));
    }
}

static void motors_stop(void)
{
    tb6612_set_duty(MOTOR_A, 0);
    tb6612_set_direction(MOTOR_A, MOTOR_STOP);
    tb6612_set_duty(MOTOR_B, 0);
    tb6612_set_direction(MOTOR_B, MOTOR_STOP);
}

/* ========================================================================
 * OLED 显示
 * ========================================================================*/
static void oled_update(float position, float steering,
                        int16_t left_d, int16_t right_d)
{
    char buf[22];

    memset(OLED_GRAM, 0, sizeof(OLED_GRAM));

    /* 行0: 传感器 (7路, #=踩线, _=无线) */
    {
        uint8_t raw = LineSensor_Read();
        uint8_t i;
        for (i = 0; i < 7; i++) {
            OLED_ShowChar(i * 18, 0, (raw & (1U << i)) ? '#' : '_', 16, 1);
        }
    }

    /* 行1: 位置 + 转向量 */
    snprintf(buf, sizeof(buf), "Pos:%+3.1f Steer:%+4.0f", position, steering);
    OLED_ShowString(0, 16, (const uint8_t*)buf);

    /* 行2: P + I + D 分量 */
    snprintf(buf, sizeof(buf), "P:%+4.0f I:%+4.0f D:%+4.0f",
             pid_pos.Kp * (0.0f - position),
             pid_pos.integral,
             pid_pos.Kd * pid_pos.prev_error);  /* prev_error 近似 D 分量 */
    OLED_ShowString(0, 32, (const uint8_t*)buf);

    /* 行3: 左右电机 */
    snprintf(buf, sizeof(buf), "L:%-4d R:%-4d", left_d, right_d);
    OLED_ShowString(0, 48, (const uint8_t*)buf);

    OLED_Refresh_Gram();
}

/* ========================================================================
 * 主函数
 * ========================================================================*/
int main(void)
{
    uint32_t loop_cnt = 0;
    float    position, steering;
    int16_t  left_duty, right_duty;

    /* ---- 1. 硬件初始化 ---- */
    SYSCFG_DL_init();
    LineSensor_Init();
    encoder_init();

    OLED_Init();
    OLED_Clear();
    OLED_ShowString(0, 0, (const uint8_t*)"PID CONTROL INIT");
    OLED_Refresh_Gram();

    tb6612_init(MOTOR_A);
    tb6612_init(MOTOR_B);
    bsp_delay_ms(50);

#if SENSOR_DEBUG
    /* ====================================================================
     * 传感器调试模式
     * ====================================================================*/
    OLED_ShowString(0, 0, (const uint8_t*)"SENSOR DEBUG");
    OLED_Refresh_Gram();

    while (1)
    {
        uint8_t raw  = LineSensor_Read();
        uint8_t raw_d = LineSensor_ReadDebounced();
        float   pos  = LineSensor_GetPosition();
        char    buf[22];
        int     i;

        memset(OLED_GRAM, 0, sizeof(OLED_GRAM));

        for (i = 0; i < 7; i++) {
            OLED_ShowChar(i * 18, 0, (raw_d & (1U << i)) ? '#' : '_', 16, 1);
        }

        OLED_ShowString(0, 16, (const uint8_t*)"S1 S2 S3 S4 S5 S6 S7");

        snprintf(buf, sizeof(buf), "RAW:0x%02X Deb:0x%02X", raw, raw_d);
        OLED_ShowString(0, 32, (const uint8_t*)buf);

        snprintf(buf, sizeof(buf), "Pos:%+3.1f", pos);
        OLED_ShowString(0, 48, (const uint8_t*)buf);

        snprintf(buf, sizeof(buf), "%d %d %d %d %d %d %d",
            (raw_d & 0x01) ? 1 : 0, (raw_d & 0x02) ? 1 : 0,
            (raw_d & 0x04) ? 1 : 0, (raw_d & 0x08) ? 1 : 0,
            (raw_d & 0x10) ? 1 : 0, (raw_d & 0x20) ? 1 : 0,
            (raw_d & 0x40) ? 1 : 0);
        OLED_ShowString(0, 56, (const uint8_t*)buf);

        OLED_Refresh_Gram();
        bsp_delay_ms(50);
    }
#else
    /* ---- 2. IMU 初始化 (8 秒稳定) ---- */
    {
        OLED_Clear();
        OLED_ShowString(0, 0, (const uint8_t*)"IMU Init...");
        OLED_Refresh_Gram();

        int imu_ret = IMU_Init();
        if (imu_ret == 0) {
            int settle = 0;
            IMU_Attitude_t att;
            while (settle < 800) {
                if (IMU_Read(&att) == 0) settle++;
                bsp_delay_ms(10);
            }
            /* 校准零偏: 取稳定后第一次读数为零点 */
            if (IMU_Read(&att) == 0) {
                yaw_offset = att.yaw;
                prev_yaw   = 0.0f;
                imu_ok     = 1;
            }
        }
    }

    /* ---- 3. PID 初始化 ---- */
    PID_Init(&pid_pos, KP_LINE, KI_LINE, KD_LINE,
             0.0f,                    /* setpoint = 0 (线在中心)          */
             PID_INTEGRAL_MAX,
             PID_OUTPUT_MAX);

    OLED_Clear();
    OLED_ShowString(0, 0, (const uint8_t*)"PID READY");
    OLED_Refresh_Gram();
    bsp_delay_ms(500);

    /* ====================================================================
     * 4. 主控制循环
     * ====================================================================*/
    while (1)
    {
        /* ---- 4a. 读取传感器位置 ---- */
        position = LineSensor_GetPosition();    /* -3.0 … +3.0 */

        /* ---- 4b. 丢线处理 (消抖 + 方向记忆) ---- */
        if (!LineSensor_IsOnLine()) {
            white_cnt++;
            /* 瞬时全白 (< LINE_LOST_FILTER): 忽略, 保持直行 */
            if (white_cnt < LINE_LOST_FILTER) {
                position = 0.0f;     /* 直行通过短暂间隙 */
            } else {
                /* 确认丢线, 开始寻线 */
                line_lost_cnt++;
                if (line_lost_cnt > LINE_LOST_TIMEOUT) {
                    motors_stop();
                    OLED_Clear();
                    OLED_ShowString(0, 20, (const uint8_t*)"LINE LOST");
                    OLED_Refresh_Gram();
                    while (1) {}
                }
                /* 按记忆方向原地转向寻线 */
                {
                    int16_t lost_speed = 300;
                    if (last_position < -0.3f) {
                        motor_set(MOTOR_A, -lost_speed);
                        motor_set(MOTOR_B,  lost_speed);
                    } else if (last_position > 0.3f) {
                        motor_set(MOTOR_A,  lost_speed);
                        motor_set(MOTOR_B, -lost_speed);
                    } else {
                        motor_set(MOTOR_A,  lost_speed);
                        motor_set(MOTOR_B,  lost_speed);
                    }
                }
                bsp_delay_ms(LOOP_PERIOD_MS);
                loop_cnt++;
                continue;
            }
        } else {
            white_cnt = 0;
            if (line_lost_cnt > 0) {
                PID_Reset(&pid_pos);
            }
            line_lost_cnt = 0;
            last_position = position;
        }

        /* 全黑=十字路口 → 直行 */
        if (LineSensor_IsAllBlack()) {
            position = 0.0f;
            PID_Reset(&pid_pos);
        }

        /* ---- 4c. 测量死区: |pos|<0.6 视为居中, 不修正 ---- */
        {
            float abs_p = (position >= 0.0f) ? position : (-position);
            if (abs_p < POS_DEADZONE) position = 0.0f;
        }

        /* ---- 4d. PID 计算 ---- */
        steering = PID_Compute(&pid_pos, position, (float)LOOP_PERIOD_MS / 1000.0f);

        /* ---- 4e. 偏航角速率阻尼 (陀螺仪反馈) ---- */
        if (imu_ok) {
            IMU_Attitude_t att;
            if (IMU_Read(&att) == 0) {
                float dt = (float)LOOP_PERIOD_MS / 1000.0f;
                /* 校零并归一化到 [-180, 180] */
                float yaw = att.yaw - yaw_offset;
                if (yaw > 180.0f)      yaw -= 360.0f;
                else if (yaw < -180.0f) yaw += 360.0f;
                /* 角速率 = (当前 - 上一帧) / dt */
                float yaw_rate = (yaw - prev_yaw) / dt;
                prev_yaw = yaw;
                /* 车身已在旋转 → 减少同方向转向力, 防过冲 */
                steering -= YAW_DAMP * yaw_rate;
            }
        }

        /* ---- 4f. 合成电机输出 ---- */
        {
            int16_t l = (int16_t)BASE_DUTY - (int16_t)steering + (int16_t)MOTOR_BALANCE;
            int16_t r = (int16_t)BASE_DUTY + (int16_t)steering - (int16_t)MOTOR_BALANCE;

            left_duty  = clamp_duty(l);
            right_duty = clamp_duty(r);
        }

        /* 死区 */
        if (left_duty  > 0 && left_duty  < MIN_DUTY) left_duty  = MIN_DUTY;
        if (left_duty  < 0 && left_duty  > -MIN_DUTY) left_duty = -MIN_DUTY;
        if (right_duty > 0 && right_duty < MIN_DUTY) right_duty = MIN_DUTY;
        if (right_duty < 0 && right_duty > -MIN_DUTY) right_duty = -MIN_DUTY;

        motor_set(MOTOR_A, left_duty);
        motor_set(MOTOR_B, right_duty);

        /* ---- 4g. OLED ---- */
        if (loop_cnt % OLED_UPDATE_EVERY == 0) {
            oled_update(position, steering, left_duty, right_duty);
        }

        /* ---- 4f. 延时 ---- */
        bsp_delay_ms(LOOP_PERIOD_MS);
        loop_cnt++;
    }
#endif

    return 0;
}
