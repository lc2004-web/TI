/*
 * 云台追踪 — 位置模式 PID (从头调参)
 *
 * 调参顺序: P → D → I
 *   第1步: I=0 D=0, 只调 KP, 从5开始往上加直到刚出现震荡, 取60%
 *   第2步: 加 KD, 从小往上加直到震荡消除
 *   第3步: (可选) 加 KI, 消除静差
 */

#include "ti_msp_dl_config.h"
#include "bsp/Emm_V5.h"
#include "bsp/bsp_servo.h"
#include "bsp/bsp_uart1.h"

#define MOTOR_ADDR          1
#define PULSES_PER_REV      3200
#define SERVO_MIN           0
#define SERVO_MAX           20
#define SERVO_NEUTRAL       10

/* ================================================================
 *  X轴 — 步进电机 (位置模式)
 * ================================================================ */
#define KP_X                18      /* P: 0.18 °/px              */
#define KI_X                0       /* I: 0                      */
#define KD_X                0       /* D: 0                      */
#define DB_X                14      /* 死区 ±px                  */
#define IMAX_X              100
#define OUTMAX_X            12      /* 单次最大 °               */

/* ================================================================
 *  Y轴 — 舵机 (位置模式)
 * ================================================================ */
#define KP_Y                15
#define KI_Y                0
#define KD_Y                0
#define DB_Y                14
#define IMAX_Y              60
#define OUTMAX_Y             6

/* ---- PID ---- */
typedef struct { int32_t kp, ki, kd, db, imax, omax; } pid_cfg_t;
typedef struct { const pid_cfg_t *c; int32_t integral, last; } pid_t;

static void pid_init(pid_t *p, const pid_cfg_t *c) {
    p->c = c; p->integral = 0; p->last = 0;
}

static int16_t pid_run(pid_t *p, int16_t err) {
    const pid_cfg_t *c = p->c;
    int32_t out;
    int16_t e = err;  /* abs */

    if (err < 0) e = -err;
    if (e < c->db) { p->last = 0; p->integral = 0; return 0; }

    /* P */
    out  = c->kp * err;

    /* D — 出死区首帧跳过 */
    if (p->last != 0) out += c->kd * (err - p->last);
    p->last = err;

    /* I — 积分限幅 */
    if (c->ki) {
        p->integral += err;
        if (p->integral >  c->imax) p->integral =  c->imax;
        if (p->integral < -c->imax) p->integral = -c->imax;
        out += c->ki * p->integral;
    }

    out /= 100;
    if (out >  c->omax) out =  c->omax;
    if (out < -c->omax) out = -c->omax;
    return (int16_t)out;
}

/* ---- 步进 ---- */
static int32_t deg2pulse(int16_t deg) { return (int32_t)deg * PULSES_PER_REV / 360; }

static void stepper_step(uint8_t dir, uint16_t angle) {
    int32_t p = deg2pulse(angle);
    if (p <= 0) return;
    Emm_V5_Pos_Control(MOTOR_ADDR, dir, 150, 20, (uint32_t)p, 1, 0);
}

/* ---- main ---- */
int main(void) {
    track_coord_t coord;
    int16_t     servo_angle = SERVO_NEUTRAL;
    int16_t     ox, oy;
    uint8_t     fn = 0;

    const pid_cfg_t cx = { KP_X, KI_X, KD_X, DB_X, IMAX_X, OUTMAX_X };
    const pid_cfg_t cy = { KP_Y, KI_Y, KD_Y, DB_Y, IMAX_Y, OUTMAX_Y };
    pid_t px, py;

    SYSCFG_DL_init();
    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN); NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN); NVIC_EnableIRQ(UART_1_INST_INT_IRQN);
    delay_ms(500);
    SERVO_Init(SERVO_NEUTRAL);
    UART1_Init();
    pid_init(&px, &cx);
    pid_init(&py, &cy);

    while (1) {
        if (UART1_Parse(&coord) && coord.valid) {

            /* 位置计数器清零 */
            fn++; if (fn >= 50) { fn = 0;
                Emm_V5_Reset_CurPos_To_Zero(MOTOR_ADDR);
                delay_ms(5); usart_getCmd();
            }

            /* X */
            ox = pid_run(&px, -coord.x);
            if      (ox > 0) stepper_step(0, (uint16_t)ox);
            else if (ox < 0) stepper_step(1, (uint16_t)(-ox));

            /* Y */
            oy = pid_run(&py, -coord.y);
            servo_angle += oy;
            if (servo_angle < SERVO_MIN) servo_angle = SERVO_MIN;
            if (servo_angle > SERVO_MAX) servo_angle = SERVO_MAX;
            SERVO_SetAngle((uint8_t)servo_angle);
        }
    }
}
