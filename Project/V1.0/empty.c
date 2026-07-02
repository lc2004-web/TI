/*
 * ========================================================================
 * 巡线小车 — 参考PID公式 + 陀螺仪阻尼 + 丢线恢复
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
 * PID (参考 循迹小车练习: 固定周期, dt嵌入增益, 无死区)
 * ========================================================================*/
#define KP              200.0f
#define KI                0.25f
#define KD               12.0f
#define D_MAX           150.0f
#define DEADZONE          0.4f   /* 小死区: 仅过滤传感器微颤   */
#define PID_INTEGRAL_MAX 3000.0f
#define PID_OUTPUT_MAX   700.0f

#define YAW_DAMP          2.5f
#define BASE_DUTY          650
#define MIN_DUTY           200
#define MOTOR_BALANCE        0

#define LOOP_PERIOD_MS      10
#define OLED_REFRESH_MS    100
#define LINE_LOST_TIMEOUT   50
#define LINE_LOST_FILTER     5

/* ---- 按键 ---- */
#define KEY1_PORT          GPIOB
#define KEY1_PIN           DL_GPIO_PIN_18
#define KEY2_PORT          GPIOA
#define KEY2_PIN           DL_GPIO_PIN_15
#define KEY_DEBOUNCE_MS     40
#define LAP_CROSS_DELAY    150

/* ========================================================================
 * 全局变量
 * ========================================================================*/
static PID_Controller_t pid;
static int   imu_ok, deviation_dir;
static int   line_lost_cnt, white_cnt;
static float pos_filt, yaw_offset, prev_yaw, last_yaw_rate;
static volatile uint32_t tick = 0;

static volatile uint8_t k1 = 0, k2 = 0;
static volatile uint32_t k1t = 0, k2t = 0;
static volatile uint8_t pk1 = 1, pk2 = 1, krdy = 0;

static int target = 1, started = 0, lap = 0, cooldown = 0;

void SysTick_Handler(void)
{
    uint8_t a, b; tick++;
    if (!krdy) return;
    a = (DL_GPIO_readPins(KEY1_PORT, KEY1_PIN) == 0) ? 0 : 1;
    if (a == 0 && pk1 == 1) { if ((tick - k1t) > KEY_DEBOUNCE_MS) { k1 = 1; k1t = tick; } }
    pk1 = a;
    b = (DL_GPIO_readPins(KEY2_PORT, KEY2_PIN) == 0) ? 0 : 1;
    if (b == 0 && pk2 == 1) { if ((tick - k2t) > KEY_DEBOUNCE_MS) { k2 = 1; k2t = tick; } }
    pk2 = b;
}

static inline float absf(float x) { return x >= 0.0f ? x : -x; }
static inline int16_t clamp(int16_t v) {
    if (v > PWM_MAX) return PWM_MAX; if (v < -PWM_MAX) return -PWM_MAX; return v;
}
static void mset(int id, int16_t d) {
    if (d >= 0) { tb6612_set_direction(id, MOTOR_CW); tb6612_set_duty(id, (uint32_t)d); }
    else        { tb6612_set_direction(id, MOTOR_CCW); tb6612_set_duty(id, (uint32_t)(-d)); }
}
static void mstop(void) {
    tb6612_set_duty(MOTOR_A,0); tb6612_set_direction(MOTOR_A,MOTOR_STOP);
    tb6612_set_duty(MOTOR_B,0); tb6612_set_direction(MOTOR_B,MOTOR_STOP);
}
static int k1c(void) { if (k1) { k1=0; return 1; } return 0; }
static int k2c(void) { if (k2) { k2=0; return 1; } return 0; }

static void oled_run(uint8_t raw, float pos, float steer, int16_t l, int16_t r)
{
    char b[22]; uint8_t i;
    memset(OLED_GRAM, 0, sizeof(OLED_GRAM));
    for (i=0;i<7;i++) OLED_ShowChar(i*18,0,(raw&(1U<<i))?'#':'_',16,1);
    snprintf(b,sizeof(b),"%s L:%d/%d",started?"":"Wait",started?lap+1:0,target);
    OLED_ShowString(0,16,(const uint8_t*)b);
    snprintf(b,sizeof(b),"P:%+3d St:%+4d",(int)(pos*10),(int)steer);
    OLED_ShowString(0,32,(const uint8_t*)b);
    snprintf(b,sizeof(b),"L:%-4d R:%-4d",l,r);
    OLED_ShowString(0,48,(const uint8_t*)b);
    __disable_irq(); OLED_Refresh_Gram(); __enable_irq();
}

int main(void)
{
    uint32_t lp=0, lo=0;
    float    pos, steer, abs_p, yr=0;
    int16_t  ld, rd;

    SYSCFG_DL_init(); LineSensor_Init(); encoder_init(); OLED_Init();
    DL_GPIO_initDigitalInputFeatures(IOMUX_PINCM44,DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_PULL_UP,DL_GPIO_HYSTERESIS_ENABLE,DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initDigitalInputFeatures(IOMUX_PINCM37,DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_PULL_UP,DL_GPIO_HYSTERESIS_ENABLE,DL_GPIO_WAKEUP_DISABLE);
    krdy=1; tb6612_init(MOTOR_A); tb6612_init(MOTOR_B); bsp_delay_ms(50);

    /* IMU 校准 */
    { int imu_ret=IMU_Init(); IMU_Attitude_t att;
        if(imu_ret==0){ int s=0; while(s<800){ if(IMU_Read(&att)==0)s++; bsp_delay_ms(10); }
            if(IMU_Read(&att)==0){ yaw_offset=att.yaw; prev_yaw=0; imu_ok=1; } } }

    PID_Init(&pid,KP,KI,KD,0.0f,PID_INTEGRAL_MAX,PID_OUTPUT_MAX,D_MAX);

    /* 设置圈数 */
    { uint8_t go=0;
        while(!go){ char b[22]; memset(OLED_GRAM,0,sizeof(OLED_GRAM));
            snprintf(b,sizeof(b),"%d",target); OLED_ShowString(55,5,(const uint8_t*)b);
            OLED_ShowString(10,22,(const uint8_t*)"SET LAPS");
            OLED_ShowString(10,36,(const uint8_t*)"KEY1:+ (1-10)");
            OLED_ShowString(10,50,(const uint8_t*)"KEY2:START");
            __disable_irq(); OLED_Refresh_Gram(); __enable_irq();
            if(k1c()){ target++; if(target>10)target=1; }
            if(k2c()) go=1; bsp_delay_ms(50); } }

    /* 倒计时 */
    { int i; char b[22];
        for(i=3;i>0;i--){ memset(OLED_GRAM,0,sizeof(OLED_GRAM));
            snprintf(b,sizeof(b),"START IN %d",i); OLED_ShowString(30,25,(const uint8_t*)b);
            __disable_irq(); OLED_Refresh_Gram(); __enable_irq(); bsp_delay_ms(1000); } }

    started=0; lap=0; cooldown=0; pos_filt=0; line_lost_cnt=0; white_cnt=0;
    PID_Reset(&pid);

    while(1)
    {
        uint8_t raw;
        pos = LineSensor_GetPosition();
        raw = LineSensor_ReadDebounced();
        pos_filt = 0.45f*pos + 0.55f*pos_filt;
        pos = pos_filt;
        abs_p = absf(pos);

        if(pos<-1.0f) deviation_dir=-1; else if(pos>1.0f) deviation_dir=1;

        /* 丢线 → 高速旋转找线 */
        if(!LineSensor_IsOnLine()){
            white_cnt++;
            if(white_cnt>=LINE_LOST_FILTER){
                line_lost_cnt++;
                if(line_lost_cnt>LINE_LOST_TIMEOUT){ mstop(); while(1){} }
                PID_Reset(&pid);
                { int16_t ss=350;  /* 温和转向找线, 不用 500 暴力旋转 */
                    if(deviation_dir<0){ mset(MOTOR_A,-ss); mset(MOTOR_B,ss); }
                    else              { mset(MOTOR_A, ss); mset(MOTOR_B,-ss); } }
                while(!LineSensor_IsOnLine()) bsp_delay_ms(5);
                { int i; for(i=0;i<15;i++){ mset(MOTOR_A,BASE_DUTY); mset(MOTOR_B,BASE_DUTY); bsp_delay_ms(10); } }
                pos_filt=0.0f;
                PID_Reset(&pid); white_cnt=0; line_lost_cnt=0;
                /* 关键: 重置陀螺仪基准, 避免旋转攒下的角位移变成尖峰 */
                if(imu_ok){ IMU_Attitude_t att;
                    if(IMU_Read(&att)==0){
                        float y=att.yaw-yaw_offset;
                        if(y>180)y-=360; else if(y<-180)y+=360;
                        prev_yaw=y; last_yaw_rate=0;
                    } }
                bsp_delay_ms(LOOP_PERIOD_MS); lp++; continue;
            }
        } else { white_cnt=0; line_lost_cnt=0; }

        /* 十字路口: ≥5路检测到黑线即算 (用原始读数, 不用消抖) */
        { uint8_t cr=LineSensor_Read(); int i,cb=0;
          for(i=0;i<7;i++) if(cr&(1<<i)) cb++;
          if(cb>=5&&cooldown==0){
            if(!started) started=1; else { lap++; if(lap>=target){ mstop(); while(1){} } }
            cooldown=LAP_CROSS_DELAY;
        }}
        if(cooldown>0) cooldown--;

        /* 小死区: 过滤传感器微颤, 不响应 ±0.3 以内的偏移 */
        if(abs_p < DEADZONE) pos = 0.0f;

        /* PID */
        steer = PID_Compute(&pid, pos, (float)LOOP_PERIOD_MS/1000.0f);
        /* 居中时积分极慢衰减, 防跨圈累积但允许弯道积分保持 */
        if(abs_p<0.4f) pid.integral *= 0.995f;

        /* 陀螺仪 */
        if(imu_ok){ IMU_Attitude_t att;
            if(IMU_Read(&att)==0){
                float y=att.yaw-yaw_offset;
                if(y>180)y-=360; else if(y<-180)y+=360;
                yr=(y-prev_yaw)/((float)LOOP_PERIOD_MS/1000.0f);
                prev_yaw=y; last_yaw_rate=yr;
            } else yr=last_yaw_rate;
            if(yr>200)yr=200; else if(yr<-200)yr=-200; /* 角速率限幅 */
            steer-=YAW_DAMP*yr; }

        /* 电机 */
        { int16_t l=(int16_t)BASE_DUTY-(int16_t)steer+MOTOR_BALANCE;
          int16_t r=(int16_t)BASE_DUTY+(int16_t)steer-MOTOR_BALANCE;
          ld=clamp(l); rd=clamp(r); }
        if(ld>0&&ld<MIN_DUTY)ld=MIN_DUTY; if(ld<0&&ld>-MIN_DUTY)ld=-MIN_DUTY;
        if(rd>0&&rd<MIN_DUTY)rd=MIN_DUTY; if(rd<0&&rd>-MIN_DUTY)rd=-MIN_DUTY;
        mset(MOTOR_A,ld); mset(MOTOR_B,rd);

        if(tick-lo>=OLED_REFRESH_MS){ lo=tick; oled_run(raw,pos,steer,ld,rd); }
        bsp_delay_ms(LOOP_PERIOD_MS); lp++;
    }
    return 0;
}
