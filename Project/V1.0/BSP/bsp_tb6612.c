/*
 * bsp_tb6612.c
 *
 * TB6612 Motor Driver Implementation
 *
 * Uses SysConfig-generated macros:
 *   PWMA_INST / PWMB_INST       — TimerG instance handles
 *   GPIO_PWMA_C0_IDX / GPIO_PWMB_C0_IDX — CCP index
 *   DC_MOTOR_AIN1/AIN2/BIN1/BIN2  — Direction GPIO pins
 */

#include "bsp_tb6612.h"
#include "ti_msp_dl_config.h"

/* ------------------------------------------------------------------ */
/* Motor Initialization                                               */
/* ------------------------------------------------------------------ */
void tb6612_init(uint8_t motor_id)
{
    if (motor_id == MOTOR_A) {
        /* Start PWM timer A */
        DL_Timer_startCounter(PWMA_INST);
        /* AIN1, AIN2 both HIGH = brake (safe startup) */
        DL_GPIO_setPins(DC_MOTOR_PORT, DC_MOTOR_AIN1_PIN);
        DL_GPIO_setPins(DC_MOTOR_PORT, DC_MOTOR_AIN2_PIN);
        /* Duty = 0 */
        DL_Timer_setCaptureCompareValue(PWMA_INST, 0, GPIO_PWMA_C0_IDX);
    }
    else if (motor_id == MOTOR_B) {
        DL_Timer_startCounter(PWMB_INST);
        DL_GPIO_setPins(DC_MOTOR_PORT, DC_MOTOR_BIN1_PIN);
        DL_GPIO_setPins(DC_MOTOR_PORT, DC_MOTOR_BIN2_PIN);
        DL_Timer_setCaptureCompareValue(PWMB_INST, 0, GPIO_PWMB_C0_IDX);
    }
}

/* ------------------------------------------------------------------ */
/* Set PWM Duty (Speed)                                               */
/* ------------------------------------------------------------------ */
void tb6612_set_duty(uint8_t motor_id, uint32_t duty)
{
    if (duty > PWM_MAX) {
        duty = PWM_MAX;
    }

    if (motor_id == MOTOR_A) {
        DL_Timer_setCaptureCompareValue(PWMA_INST, duty, GPIO_PWMA_C0_IDX);
    }
    else if (motor_id == MOTOR_B) {
        DL_Timer_setCaptureCompareValue(PWMB_INST, duty, GPIO_PWMB_C0_IDX);
    }
}

/* ------------------------------------------------------------------ */
/* Helper: apply per-motor direction invert                           */
/* ------------------------------------------------------------------ */
static uint8_t apply_dir_invert(uint8_t motor_id, uint8_t direction)
{
    if (direction == MOTOR_STOP) return direction;

    uint8_t invert = (motor_id == MOTOR_A) ? MOTOR_A_DIR_INVERT
                                           : MOTOR_B_DIR_INVERT;
    if (invert) {
        return (direction == MOTOR_CW) ? MOTOR_CCW : MOTOR_CW;
    }
    return direction;
}

/* ------------------------------------------------------------------ */
/* Set Motor Direction                                                */
/* ------------------------------------------------------------------ */
void tb6612_set_direction(uint8_t motor_id, uint8_t direction)
{
    direction = apply_dir_invert(motor_id, direction);

    if (motor_id == MOTOR_A) {
        switch (direction) {
        case MOTOR_STOP:  /* Brake: AIN1=H, AIN2=H */
            DL_GPIO_setPins(DC_MOTOR_PORT, DC_MOTOR_AIN1_PIN);
            DL_GPIO_setPins(DC_MOTOR_PORT, DC_MOTOR_AIN2_PIN);
            break;
        case MOTOR_CW:    /* CW: AIN1=H, AIN2=L */
            DL_GPIO_setPins(DC_MOTOR_PORT, DC_MOTOR_AIN1_PIN);
            DL_GPIO_clearPins(DC_MOTOR_PORT, DC_MOTOR_AIN2_PIN);
            break;
        case MOTOR_CCW:   /* CCW: AIN1=L, AIN2=H */
            DL_GPIO_clearPins(DC_MOTOR_PORT, DC_MOTOR_AIN1_PIN);
            DL_GPIO_setPins(DC_MOTOR_PORT, DC_MOTOR_AIN2_PIN);
            break;
        }
    }
    else if (motor_id == MOTOR_B) {
        switch (direction) {
        case MOTOR_STOP:  /* Brake: BIN1=H, BIN2=H */
            DL_GPIO_setPins(DC_MOTOR_PORT, DC_MOTOR_BIN1_PIN);
            DL_GPIO_setPins(DC_MOTOR_PORT, DC_MOTOR_BIN2_PIN);
            break;
        case MOTOR_CW:    /* CW: BIN1=H, BIN2=L */
            DL_GPIO_setPins(DC_MOTOR_PORT, DC_MOTOR_BIN1_PIN);
            DL_GPIO_clearPins(DC_MOTOR_PORT, DC_MOTOR_BIN2_PIN);
            break;
        case MOTOR_CCW:   /* CCW: BIN1=L, BIN2=H */
            DL_GPIO_clearPins(DC_MOTOR_PORT, DC_MOTOR_BIN1_PIN);
            DL_GPIO_setPins(DC_MOTOR_PORT, DC_MOTOR_BIN2_PIN);
            break;
        }
    }
}
