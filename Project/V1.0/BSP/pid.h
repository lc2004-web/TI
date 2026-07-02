/*
 * pid.h
 *
 * Simple PID Controller with anti-windup
 * For line-following steering control
 */

#ifndef BSP_PID_H_
#define BSP_PID_H_

typedef struct {
    float Kp;             /* Proportional gain                         */
    float Ki;             /* Integral gain                             */
    float Kd;             /* Derivative gain                           */
    float setpoint;       /* Target value (typically 0 for centering)  */
    float integral;       /* Accumulated integral error                */
    float prev_error;     /* Previous error (for derivative)           */
    float integral_max;   /* Anti-windup: max integral magnitude       */
    float output_max;     /* Output clamping: max output magnitude     */
    float d_max;          /* Per-cycle D-term clamp (防量化尖峰)        */
} PID_Controller_t;

/**
 * @brief  Initialize PID controller
 * @param  pid          Pointer to PID struct
 * @param  Kp, Ki, Kd   PID gains
 * @param  setpoint     Target value
 * @param  integral_max Anti-windup limit (max absolute integral)
 * @param  output_max   Output clamp limit (max absolute output)
 * @param  d_max        Per-cycle derivative clamp (0=no limit)
 */
void PID_Init(PID_Controller_t *pid, float Kp, float Ki, float Kd,
              float setpoint, float integral_max, float output_max,
              float d_max);

/**
 * @brief  Compute PID output
 * @param  pid         Pointer to PID struct
 * @param  measurement Current sensor value
 * @param  dt          Time delta in seconds (e.g. 0.01 for 10ms)
 * @return float       PID output (clamped to ±output_max)
 */
float PID_Compute(PID_Controller_t *pid, float measurement, float dt);

/**
 * @brief  Reset PID state (clear integral and prev_error)
 * @param  pid  Pointer to PID struct
 */
void PID_Reset(PID_Controller_t *pid);

/**
 * @brief  Update PID gains at runtime (for tuning)
 */
void PID_SetGains(PID_Controller_t *pid, float Kp, float Ki, float Kd);

#endif /* BSP_PID_H_ */
