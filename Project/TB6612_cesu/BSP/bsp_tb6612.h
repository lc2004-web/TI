/*
 * bsp_tb6612.h
 *
 * TB6612 Dual DC Motor Driver BSP
 *
 * Pin Mapping:
 *   Motor A: PWMA→PB2, AIN1→PA13, AIN2→PA14
 *   Motor B: PWMB→PB3, BIN1→PA16, BIN2→PA17
 *   STBY → 5V (hardwired, always enabled)
 *
 * PWM: 8kHz (32MHz / 4000), duty range 0~4000
 *
 * TB6612 Control Truth Table:
 *   AIN1 | AIN2 | Function
 *   -----|------|---------
 *    H   |  H   | Brake
 *    L   |  L   | Stop (coast)
 *    H   |  L   | CW / Forward
 *    L   |  H   | CCW / Reverse
 */

#ifndef BSP_TB6612_H_
#define BSP_TB6612_H_

#include <stdint.h>

#define MOTOR_A     1
#define MOTOR_B     2

#define MOTOR_STOP  0
#define MOTOR_CW    1
#define MOTOR_CCW   2

#define PWM_MAX     4000

/*
 * Motor direction invert — set to 1 if CW/CCW is reversed on that motor.
 * This depends on motor +/- wiring order.
 */
#define MOTOR_A_DIR_INVERT  0   /* 0=normal, 1=swap CW/CCW */
#define MOTOR_B_DIR_INVERT  1

/**
 * @brief  Initialize motor (start PWM timer, set duty to 0)
 * @param  motor_id  MOTOR_A or MOTOR_B
 */
void tb6612_init(uint8_t motor_id);

/**
 * @brief  Set PWM duty cycle (speed)
 * @param  motor_id  MOTOR_A or MOTOR_B
 * @param  duty      0 ~ PWM_MAX (4000)
 */
void tb6612_set_duty(uint8_t motor_id, uint32_t duty);

/**
 * @brief  Set motor direction
 * @param  motor_id    MOTOR_A or MOTOR_B
 * @param  direction   MOTOR_STOP (0), MOTOR_CW (1), MOTOR_CCW (2)
 */
void tb6612_set_direction(uint8_t motor_id, uint8_t direction);

#endif /* BSP_TB6612_H_ */
