/**
 * @file  bsp_servo.h
 * @brief 舵机驱动模块 — TIMA0 CCP0 PWM, 50Hz, PA8
 *
 * 参数范围:
 *   角度:     0° ~ 180°
 *   脉宽:     0.5ms ~ 2.5ms
 *   周期:     20ms (50Hz)
 *   分辨率:   ~1.44°/tick
 */

#ifndef BSP_SERVO_H
#define BSP_SERVO_H

#include "ti_msp_dl_config.h"

/* ---- API ---- */

void SERVO_Init(void);
void SERVO_SetAngle(uint8_t angle);

#endif /* BSP_SERVO_H */
