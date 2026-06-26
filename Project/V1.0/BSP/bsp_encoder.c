/*
 * bsp_encoder.c
 *
 * Quadrature Encoder Speed Measurement BSP Implementation
 *
 * Uses SysConfig-generated GPIO macros for encoder pins.
 * GPIO interrupts on A-phase edges (both rise & fall) for 2x resolution.
 * B-phase level determines rotation direction.
 * A 50ms periodic timer (TIMG0) triggers speed calculation.
 *
 * Pin assignment (via SysConfig ENCODER_A / ENCODER_B groups):
 *   Motor A: PA26 (A phase, interrupt), PA25 (B phase, input)
 *   Motor B: PB24 (A phase, interrupt), PB20 (B phase, input)
 *
 * Speed Formula:
 *   speed_rpm = count * (1/0.05s) * 60 / ENCODER_2X_RES
 *             = count * 1200 / ENCODER_2X_RES
 *
 * Reference: 10_DC_MOTOR_PID_2
 */

#include "bsp_encoder.h"
#include "ti_msp_dl_config.h"

/* ---------- Encoder pulse counters (ISR-accessed, signed for direction) ---------- */
static volatile int32_t encoder_count[2] = {0, 0};

/* ---------- Calculated speeds (updated every 50ms by timer ISR) ---------- */
static volatile float encoder_speed_rpm[2] = {0.0f, 0.0f};

/* ====================================================================== */
/* Public API                                                             */
/* ====================================================================== */

/**
 * @brief  Initialize encoders — enable GPIO & timer NVIC interrupts,
 *         start the periodic speed-measurement timer.
 */
void encoder_init(void)
{
    /* Enable GPIO GROUP1 interrupts (both GPIOA for Motor A and GPIOB for Motor B) */
    NVIC_EnableIRQ(ENCODER_A_INT_IRQN);
    NVIC_EnableIRQ(ENCODER_B_INT_IRQN);

    /* Start speed-calc timer and enable its interrupt */
    DL_Timer_startCounter(MOTOR_SPEED_INST);
    NVIC_EnableIRQ(MOTOR_SPEED_INST_INT_IRQN);
}

int32_t encoder_get_count(uint8_t encoder_id)
{
    if (encoder_id == ENCODER_A) return encoder_count[0];
    if (encoder_id == ENCODER_B) return encoder_count[1];
    return 0;
}

float encoder_get_speed_rpm(uint8_t encoder_id)
{
    if (encoder_id == ENCODER_A) return encoder_speed_rpm[0];
    if (encoder_id == ENCODER_B) return encoder_speed_rpm[1];
    return 0.0f;
}

void encoder_clear_count(uint8_t encoder_id)
{
    if (encoder_id == ENCODER_A) {
        encoder_count[0] = 0;
        encoder_speed_rpm[0] = 0.0f;
    }
    else if (encoder_id == ENCODER_B) {
        encoder_count[1] = 0;
        encoder_speed_rpm[1] = 0.0f;
    }
}

/* ====================================================================== */
/* GPIO GROUP1 Interrupt Handler                                          */
/* Covers both GPIOA (Motor A encoder) and GPIOB (Motor B encoder)        */
/* ====================================================================== */
void GROUP1_IRQHandler(void)
{
    uint32_t a_level, b_level;

    /* ---- Motor A: PA26 (A phase edge) on GPIOA ---- */
    switch (DL_GPIO_getPendingInterrupt(GPIOA))
    {
    case ENCODER_A_AA_IIDX:
        /*
         * Quadrature XOR method:
         *   Read both A and B levels at the interrupt moment.
         *   CW  (A leads B):  A ^ B = 1  →  increment
         *   CCW (B leads A):  A ^ B = 0  →  decrement
         * This works correctly for both rising AND falling edges.
         */
        a_level = (DL_GPIO_readPins(ENCODER_A_PORT, ENCODER_A_AA_PIN) != 0U);
        b_level = (DL_GPIO_readPins(ENCODER_A_PORT, ENCODER_A_AB_PIN) != 0U);

        if ((a_level ^ b_level) != 0U) {
            /* CW */ encoder_count[0] += (ENCODER_A_INVERT ? -1 : 1);
        } else {
            /* CCW */ encoder_count[0] += (ENCODER_A_INVERT ? 1 : -1);
        }
        DL_GPIO_clearInterruptStatus(GPIOA, ENCODER_A_AA_PIN);
        break;
    default:
        break;
    }

    /* ---- Motor B: PB24 (A phase edge) on GPIOB ---- */
    switch (DL_GPIO_getPendingInterrupt(GPIOB))
    {
    case ENCODER_B_BA_IIDX:
        a_level = (DL_GPIO_readPins(ENCODER_B_PORT, ENCODER_B_BA_PIN) != 0U);
        b_level = (DL_GPIO_readPins(ENCODER_B_PORT, ENCODER_B_BB_PIN) != 0U);

        if ((a_level ^ b_level) != 0U) {
            /* CW */ encoder_count[1] += (ENCODER_B_INVERT ? -1 : 1);
        } else {
            /* CCW */ encoder_count[1] += (ENCODER_B_INVERT ? 1 : -1);
        }
        DL_GPIO_clearInterruptStatus(GPIOB, ENCODER_B_BA_PIN);
        break;
    default:
        break;
    }
}

/* ====================================================================== */
/* Speed-Calculation Timer ISR  (50 ms period, TIMG0)                     */
/* ====================================================================== */
void MOTOR_SPEED_INST_IRQHandler(void)
{
    switch (DL_Timer_getPendingInterrupt(MOTOR_SPEED_INST))
    {
    case DL_TIMER_IIDX_LOAD:
        /*
         * Convert pulse count accumulated over 50 ms to output-shaft RPM:
         *   motor_rpm  = count × (1000 / 50) × 60 / ENCODER_2X_RES
         *              = count × 1200 / ENCODER_2X_RES
         *   output_rpm = motor_rpm / MOTOR_GEAR_RATIO
         */
        encoder_speed_rpm[0] = (float)encoder_count[0] * 1200.0f
                               / (float)ENCODER_2X_RES
                               / (float)MOTOR_GEAR_RATIO;
        encoder_speed_rpm[1] = (float)encoder_count[1] * 1200.0f
                               / (float)ENCODER_2X_RES
                               / (float)MOTOR_GEAR_RATIO;

        /* Reset counters for next period */
        encoder_count[0] = 0;
        encoder_count[1] = 0;
        break;
    default:
        break;
    }
}
