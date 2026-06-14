/**
 * @file  bsp_servo.c
 * @brief 舵机驱动 — TIMA0 CCP0 PWM 输出, 50Hz
 *
 * 硬件:  PA8 → IOMUX_PINCM19 → TIMA0_CCP0
 *
 * 定时器参数 (CPUCLK=32MHz):
 *   - 分频: /8 → 4MHz
 *   - 预分频: /64 → 62.5kHz (16us/tick)
 *   - PWM 周期: 1250 ticks → 20ms (50Hz)
 *   - 0° 脉宽: 31 ticks (0.5ms) → compare=1218
 *   - 180°脉宽: 156 ticks (2.5ms) → compare=1093
 */

#include "bsp_servo.h"
#include <ti/driverlib/dl_timera.h>

/* ---- 定时器参数 ---- */
#define SERVO_TIMER             (TIMA0)
#define SERVO_PWM_PERIOD        1250U   /* 20ms in ticks     */
#define SERVO_CMP_0DEG          1218U   /* 0.5ms pulse       */
#define SERVO_CMP_180DEG        1093U   /* 2.5ms pulse       */
#define SERVO_CMP_RANGE         (SERVO_CMP_0DEG - SERVO_CMP_180DEG) /* 125 */

/* PA8 引脚复用 */
#define SERVO_PINCM             (IOMUX_PINCM19)
#define SERVO_PINCM_FUNC        (IOMUX_PINCM19_PF_TIMA0_CCP0)

/* ---- API ---- */

/**
 * @brief  初始化舵机 PWM 输出
 */
void SERVO_Init(void)
{
    /* 1. 使能 TIMA0 电源 (必须先于所有寄存器操作!) */
    DL_TimerA_enablePower(SERVO_TIMER);

    /* 2. 配置 PA8 为 TIMA0_CCP0 外设功能 */
    DL_GPIO_initPeripheralOutputFunctionFeatures(
        SERVO_PINCM, SERVO_PINCM_FUNC,
        DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE,
        DL_GPIO_DRIVE_STRENGTH_LOW,
        DL_GPIO_HIZ_DISABLE);

    /* 3. 配置定时器时钟: BUSCLK / 8 / 64 → 62.5kHz */
    DL_Timer_ClockConfig clkCfg = {
        .clockSel    = DL_TIMER_CLOCK_BUSCLK,
        .divideRatio = DL_TIMER_CLOCK_DIVIDE_8,
        .prescale    = 63U     /* ÷64 */
    };
    DL_TimerA_setClockConfig(SERVO_TIMER, &clkCfg);

    /* 4. 设置 CCP0 为输出方向 */
    DL_TimerA_setCCPDirection(SERVO_TIMER, DL_TIMER_CC0_OUTPUT);

    /* 5. 初始化 PWM 模式: 边沿对齐, 50Hz, isTimerWithFourCC=true */
    DL_Timer_PWMConfig pwmCfg = {
        .period            = SERVO_PWM_PERIOD,
        .pwmMode           = DL_TIMER_PWM_MODE_EDGE_ALIGN,
        .isTimerWithFourCC = true,   /* TIMA0 有 4 个 CC 通道 */
        .startTimer        = DL_TIMER_START
    };
    DL_TimerA_initPWMMode(SERVO_TIMER, &pwmCfg);

    /* 6. 默认占空比 → 90° (1.5ms) 居中 */
    DL_TimerA_setCaptureCompareValue(SERVO_TIMER,
        SERVO_CMP_0DEG - (SERVO_CMP_RANGE / 2),  /* 1218 - 62 = 1156 */
        DL_TIMER_CC_0_INDEX);
}

/**
 * @brief  设置舵机角度
 * @param  angle  目标角度 [0, 180], 自动钳位
 */
void SERVO_SetAngle(uint8_t angle)
{
    uint16_t cmp;

    if (angle > 180)
        angle = 180;

    /* compare = 1218 - angle * 125 / 180 */
    cmp = SERVO_CMP_0DEG - (uint16_t)(((uint32_t)angle * SERVO_CMP_RANGE) / 180U);

    DL_TimerA_setCaptureCompareValue(SERVO_TIMER, cmp, DL_TIMER_CC_0_INDEX);
}
