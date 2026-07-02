/*
 * pid.c
 *
 * Simple PID Controller Implementation
 */

#include "pid.h"

void PID_Init(PID_Controller_t *pid, float Kp, float Ki, float Kd,
              float setpoint, float integral_max, float output_max,
              float d_max)
{
    pid->Kp           = Kp;
    pid->Ki           = Ki;
    pid->Kd           = Kd;
    pid->setpoint     = setpoint;
    pid->integral     = 0.0f;
    pid->prev_error   = 0.0f;
    pid->integral_max = integral_max;
    pid->output_max   = output_max;
    pid->d_max        = d_max;
}

float PID_Compute(PID_Controller_t *pid, float measurement, float dt)
{
    /* 参考 循迹小车练习: 固定周期PID, dt已嵌入增益
     *   P = Kp * error
     *   I = Ki * sum(error)
     *   D = Kd * (error - prev_error)
     *   无 dt 缩放, 杜绝传感器量化尖峰被 1/dt 放大
     */
    float error = pid->setpoint - measurement;
    float p_term, i_term, d_term, output;

    /* P */
    p_term = pid->Kp * error;

    /* I: 累加误差 (无 dt), 抗饱和 */
    pid->integral += error;
    if (pid->integral > pid->integral_max)
        pid->integral = pid->integral_max;
    else if (pid->integral < -pid->integral_max)
        pid->integral = -pid->integral_max;
    i_term = pid->Ki * pid->integral;

    /* D: 误差差分 (无 dt 除法), 加限幅 */
    d_term = pid->Kd * (error - pid->prev_error);
    if (pid->d_max > 0.0f) {
        if (d_term > pid->d_max)       d_term = pid->d_max;
        else if (d_term < -pid->d_max) d_term = -pid->d_max;
    }
    pid->prev_error = error;

    /* 求和 + 输出限幅 */
    output = p_term + i_term + d_term;
    if (output > pid->output_max)       output = pid->output_max;
    else if (output < -pid->output_max) output = -pid->output_max;

    return output;
}

void PID_Reset(PID_Controller_t *pid)
{
    pid->integral   = 0.0f;
    pid->prev_error = 0.0f;
}

void PID_SetGains(PID_Controller_t *pid, float Kp, float Ki, float Kd)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
}
