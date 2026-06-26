/*
 * line_sensor.c
 *
 * 7-Channel Grayscale Line Sensor Implementation
 * Uses GPIO digital input with pull-up — each sensor pulls LOW over black line.
 *
 * Pin mapping:
 *   S1→PB8, S2→PA12, S3→PA18, S4→PB3, S5→PB9, S6→PA9, S7→PB19
 */

#include "line_sensor.h"
#include "ti_msp_dl_config.h"

/* ------------------------------------------------------------------ */
/* IOMUX Pin Config indices (from MSPM0 datasheet)                    */
/* ------------------------------------------------------------------ */
#define IOMUX_S1    IOMUX_PINCM25   /* PB8  */
#define IOMUX_S2    IOMUX_PINCM34   /* PA12 */
#define IOMUX_S3    IOMUX_PINCM40   /* PA18 */
#define IOMUX_S4    IOMUX_PINCM16   /* PB3  */
#define IOMUX_S5    IOMUX_PINCM26   /* PB9  */
#define IOMUX_S6    IOMUX_PINCM20   /* PA9  */
#define IOMUX_S7    IOMUX_PINCM45   /* PB19 */

/* GPIO read masks — group by port for efficiency */
#define MASK_A  (DL_GPIO_PIN_12 | DL_GPIO_PIN_18 | DL_GPIO_PIN_9)   /* S2, S3, S6 */
#define MASK_B  (DL_GPIO_PIN_8  | DL_GPIO_PIN_3  | DL_GPIO_PIN_9 | DL_GPIO_PIN_19) /* S1, S4, S5, S7 */

/* Position weights: S1=-3, S2=-2, S3=-1, S4=0, S5=+1, S6=+2, S7=+3 */
static const float sensor_weight[7] = {
    -3.0f, -2.0f, -1.0f, 0.0f, 1.0f, 2.0f, 3.0f
};

/* Last known position for line-lost recovery */
static float last_position = 0.0f;

/* ================================================================== */
/* Public API                                                          */
/* ================================================================== */

/**
 * @brief  Initialize all 7 sensor pins as digital inputs with pull-up.
 */
void LineSensor_Init(void)
{
    /* PORTA — S2, S3, S6 */
    DL_GPIO_initDigitalInputFeatures(IOMUX_S2,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_ENABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initDigitalInputFeatures(IOMUX_S3,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_ENABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initDigitalInputFeatures(IOMUX_S6,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_ENABLE, DL_GPIO_WAKEUP_DISABLE);

    /* PORTB — S1, S4, S5, S7 */
    DL_GPIO_initDigitalInputFeatures(IOMUX_S1,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_ENABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initDigitalInputFeatures(IOMUX_S4,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_ENABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initDigitalInputFeatures(IOMUX_S5,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_ENABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initDigitalInputFeatures(IOMUX_S7,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_ENABLE, DL_GPIO_WAKEUP_DISABLE);
}

/**
 * @brief  Read all 7 channels — returns byte where bit=1 means black detected.
 */
uint8_t LineSensor_Read(void)
{
    uint8_t  result = 0;
    uint32_t valA, valB;

    valA = DL_GPIO_readPins(GPIOA, MASK_A);
    valB = DL_GPIO_readPins(GPIOB, MASK_B);

    /* Active-high: HIGH = black → set bit to 1 */
    if ((valB & DL_GPIO_PIN_8))  result |= 0x01;   /* S1: PB8  */
    if ((valA & DL_GPIO_PIN_12)) result |= 0x02;   /* S2: PA12 */
    if ((valA & DL_GPIO_PIN_18)) result |= 0x04;   /* S3: PA18 */
    if ((valB & DL_GPIO_PIN_3))  result |= 0x08;   /* S4: PB3  */
    if ((valB & DL_GPIO_PIN_9))  result |= 0x10;   /* S5: PB9  */
    if ((valA & DL_GPIO_PIN_9))  result |= 0x20;   /* S6: PA9  */
    if ((valB & DL_GPIO_PIN_19)) result |= 0x40;   /* S7: PB19 */

    return result;
}

/**
 * @brief  3次采样多数投票消抖 — 过滤传感器模块输出抖动
 */
uint8_t LineSensor_ReadDebounced(void)
{
    uint8_t r1 = LineSensor_Read();
    uint8_t r2 = LineSensor_Read();
    uint8_t r3 = LineSensor_Read();
    /* 3次中 ≥2次为黑 → 确认黑色; <2次 → 忽略为噪声 */
    return (r1 & r2) | (r2 & r3) | (r1 & r3);
}

/**
 * @brief  Compute weighted line position (-3.0 left … +3.0 right).
 *         Falls back to last known position when no line is visible.
 */
float LineSensor_GetPosition(void)
{
    uint8_t raw   = LineSensor_ReadDebounced();
    float   w_sum = 0.0f;
    int     count = 0;
    int     i;

    for (i = 0; i < 7; i++) {
        if (raw & (1U << i)) {
            w_sum += sensor_weight[i];
            count++;
        }
    }

    if (count == 0) {
        return last_position;          /* hold last position */
    }

    last_position = w_sum / (float)count;
    return last_position;
}

int LineSensor_IsOnLine(void)
{
    return (LineSensor_ReadDebounced() != 0x00);
}

int LineSensor_IsAllBlack(void)
{
    return (LineSensor_ReadDebounced() == 0x7F);
}
