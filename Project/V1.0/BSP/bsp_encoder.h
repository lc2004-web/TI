/*
 * bsp_encoder.h
 *
 * Quadrature Encoder Speed Measurement BSP
 *
 * Pin Mapping (Motor Encoder):
 *   Motor A: PA26 = A phase (interrupt), PA25 = B phase (input)
 *   Motor B: PB24 = A phase (interrupt), PB20 = B phase (input)
 *
 * Method:
 *   - Both edges of A phase trigger interrupt (2x resolution per pulse)
 *   - B phase level is read in ISR to determine direction
 *   - A 50ms periodic timer calculates speed from accumulated counts
 *
 * Speed Calculation:
 *   speed_rpm = (count * 20 * 60) / ENCODER_PPR
 *   where 20 = 1/50ms (converts to per-second)
 */

#ifndef BSP_ENCODER_H_
#define BSP_ENCODER_H_

#include <stdint.h>

/* ---------- Encoder ID ---------- */
#define ENCODER_A     0
#define ENCODER_B     1

/* ---------- Encoder Parameters (user-tunable) ---------- */
#define ENCODER_PPR          500     /* Encoder pulses per revolution (GMR, 500 PPR)    */
#define MOTOR_GEAR_RATIO     20      /* Gearbox reduction ratio (output = motor / ratio) */
#define ENCODER_2X_RES       (ENCODER_PPR * 2)  /* 2x counting (both edges)             */

/*
 * Direction invert flags — set to 1 if the encoder counts backwards
 * (e.g. positive speed shows negative RPM, or vice versa).
 * This depends on A/B phase wiring order and motor CW/CCW convention.
 */
#define ENCODER_A_INVERT     0       /* Motor A: 0=normal, 1=invert direction */
#define ENCODER_B_INVERT     1       /* Motor B: 0=normal, 1=invert direction */

/**
 * @brief  Initialize encoder module (GPIO interrupts + speed timer)
 *         Must be called after SYSCFG_DL_init().
 */
void encoder_init(void);

/**
 * @brief  Get accumulated pulse count (signed — direction-aware)
 * @param  encoder_id  ENCODER_A or ENCODER_B
 * @return int32_t raw pulse count since last clear
 */
int32_t encoder_get_count(uint8_t encoder_id);

/**
 * @brief  Get calculated speed
 * @param  encoder_id  ENCODER_A or ENCODER_B
 * @return float speed in RPM (revolutions per minute)
 */
float encoder_get_speed_rpm(uint8_t encoder_id);

/**
 * @brief  Clear accumulated pulse count
 * @param  encoder_id  ENCODER_A or ENCODER_B
 */
void encoder_clear_count(uint8_t encoder_id);

#endif /* BSP_ENCODER_H_ */
