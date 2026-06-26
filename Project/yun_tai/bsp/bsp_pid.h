/**
 * @file  bsp_pid.h
 * @brief 整数 PID 控制器 (位置式, 双轴独立)
 *
 * Kp/Ki/Kd 存 ×100 (如 0.50 → 50), error 为原始像素。
 * output = (Kp*err + Ki*∫err + Kd*Δerr) / 100
 *
 * 特性: 积分限幅 (anti-windup), 死区, 输出限幅。
 */

#ifndef BSP_PID_H
#define BSP_PID_H

#include <stdint.h>
#include <stdbool.h>

/* ---- PID 参数结构 ---- */
typedef struct {
    int32_t kp;         /* 比例系数 ×100     (如 50 = 0.50 °/px) */
    int32_t ki;         /* 积分系数 ×100     (如  5 = 0.05)       */
    int32_t kd;         /* 微分系数 ×100     (如 20 = 0.20)       */
    int32_t deadband;   /* 死区 (像素绝对值) */
    int32_t imax;       /* 积分限幅 (像素)   */
    int32_t out_max;    /* 输出限幅 (正值)   */
} pid_cfg_t;

/* ---- PID 状态 (每轴一份) ---- */
typedef struct {
    const pid_cfg_t *cfg;
    int32_t integral;   /* 积分累加 (像素)   */
    int32_t last_error; /* 上次误差 (像素)   */
} pid_state_t;

/* ---- API ---- */

void PID_Init(pid_state_t *pid, const pid_cfg_t *cfg);
int16_t PID_Compute(pid_state_t *pid, int16_t error);

#endif /* BSP_PID_H */
