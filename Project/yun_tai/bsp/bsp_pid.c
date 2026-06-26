/**
 * @file  bsp_pid.c
 * @brief 整数 PID 实现
 *
 * 公式:  output = (Kp*err + Ki*∫err + Kd*Δerr) / 100
 *
 * Kp/Ki/Kd 存 ×100, error 为原始像素, integral 累计原始像素。
 * 除法用 /100, 单个软除 M0+ 可接受。
 */

#include "bsp_pid.h"

#define ABS(x)  ((x) < 0 ? -(x) : (x))

static int32_t clamp(int32_t v, int32_t lo, int32_t hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/* ================================================================
 *  API
 * ================================================================ */

void PID_Init(pid_state_t *pid, const pid_cfg_t *cfg)
{
    pid->cfg        = cfg;
    pid->integral   = 0;
    pid->last_error = 0;
}

int16_t PID_Compute(pid_state_t *pid, int16_t error)
{
    const pid_cfg_t *c = pid->cfg;
    int32_t p_term, i_term, d_term, output;

    /* ---- 死区 ---- */
    if (ABS(error) < c->deadband) {
        return 0;
    }

    /* ---- 比例 ---- */
    p_term = (int32_t)c->kp * error;

    /* ---- 积分 (anti-windup) ---- */
    pid->integral += error;
    pid->integral = clamp(pid->integral, -(c->imax), c->imax);
    i_term = (int32_t)c->ki * pid->integral;

    /* ---- 微分 ---- */
    d_term = (int32_t)c->kd * (error - pid->last_error);
    pid->last_error = error;

    /* ---- 合成 ---- */
    output = (p_term + i_term + d_term) / 100;

    /* ---- 输出限幅 ---- */
    output = clamp(output, -(c->out_max), c->out_max);

    return (int16_t)output;
}
