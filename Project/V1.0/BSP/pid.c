/*
 * pid.c
 *
 * Simple PID Controller Implementation
 */

#include "pid.h"

void PID_Init(PID_Controller_t *pid, float Kp, float Ki, float Kd,
              float setpoint, float integral_max, float output_max)
{
    pid->Kp           = Kp;
    pid->Ki           = Ki;
    pid->Kd           = Kd;
    pid->setpoint     = setpoint;
    pid->integral     = 0.0f;
    pid->prev_error   = 0.0f;
    pid->integral_max = integral_max;
    pid->output_max   = output_max;
}

float PID_Compute(PID_Controller_t *pid, float measurement, float dt)
{
    float error, p_term, i_term, d_term, output;

    /* 1. Compute error */
    error = pid->setpoint - measurement;

    /* 2. Proportional term */
    p_term = pid->Kp * error;

    /* 3. Integral term (with anti-windup clamping) */
    pid->integral += pid->Ki * error * dt;
    if (pid->integral > pid->integral_max) {
        pid->integral = pid->integral_max;
    } else if (pid->integral < -pid->integral_max) {
        pid->integral = -pid->integral_max;
    }
    i_term = pid->integral;

    /* 4. Derivative term (derivative on measurement,
     *    avoids "derivative kick" on setpoint changes) */
    if (dt > 0.0001f) {
        d_term = pid->Kd * (error - pid->prev_error) / dt;
    } else {
        d_term = 0.0f;
    }
    pid->prev_error = error;

    /* 5. Sum and clamp output */
    output = p_term + i_term + d_term;

    if (output > pid->output_max) {
        output = pid->output_max;
    } else if (output < -pid->output_max) {
        output = -pid->output_max;
    }

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
