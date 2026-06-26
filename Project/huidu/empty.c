/*
 * 七路灰度传感器测试 — MSPM0G3507 (48-pin LQFP)
 *
 * 硬件连接:
 *   Sensor 1: PB8   Sensor 2: PA12   Sensor 3: PA18
 *   Sensor 4: PB3   Sensor 5: PB9    Sensor 6: PA9   Sensor 7: PB19
 *   检测到黑线 = 低电平 → bit置1
 *
 * OLED: SSD1306 128x64 (4线软件SPI)
 *   SCL=PA28  SDA=PA31  RST=PB14  DC=PB15  CS=PA22
 */

#include "ti_msp_dl_config.h"
#include "BSP/bsp_oled.h"
#include <string.h>
#include <stdio.h>

/* ========================================================================
 * 传感器引脚定义
 * ========================================================================*/

/* PORTA IOMUX */
#define IOMUX_S2       IOMUX_PINCM34   /* PA12 */
#define IOMUX_S3       IOMUX_PINCM40   /* PA18 */
#define IOMUX_S6       IOMUX_PINCM20   /* PA9  */

/* PORTB IOMUX */
#define IOMUX_S1       IOMUX_PINCM25   /* PB8  — 替代PA27 */
#define IOMUX_S4       IOMUX_PINCM16   /* PB3  */
#define IOMUX_S5       IOMUX_PINCM26   /* PB9  */
#define IOMUX_S7       IOMUX_PINCM45   /* PB19 */

/* PORTA GPIO 读取掩码 */
#define MASK_A  (DL_GPIO_PIN_12 | DL_GPIO_PIN_18 | DL_GPIO_PIN_9)

/* PORTB GPIO 读取掩码 */
#define MASK_B  (DL_GPIO_PIN_8  | DL_GPIO_PIN_3  | DL_GPIO_PIN_9  | DL_GPIO_PIN_19)

/* ========================================================================
 * 传感器初始化
 * ========================================================================*/

void Sensor_Init(void)
{
    /* PORTA — S2, S3, S6 */
    DL_GPIO_initDigitalInputFeatures(IOMUX_S2,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initDigitalInputFeatures(IOMUX_S3,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initDigitalInputFeatures(IOMUX_S6,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    /* PORTB — S1, S4, S5, S7 */
    DL_GPIO_initDigitalInputFeatures(IOMUX_S1,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initDigitalInputFeatures(IOMUX_S4,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initDigitalInputFeatures(IOMUX_S5,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initDigitalInputFeatures(IOMUX_S7,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
}

/* ========================================================================
 * 读取全部 7 路传感器
 * ========================================================================*/

uint8_t Read_Sensors(void)
{
    uint8_t result = 0;

    uint32_t valA = DL_GPIO_readPins(GPIOA, MASK_A);
    uint32_t valB = DL_GPIO_readPins(GPIOB, MASK_B);

    if (!(valB & DL_GPIO_PIN_8))  result |= 0x01;  /* S1: PB8  */
    if (!(valA & DL_GPIO_PIN_12)) result |= 0x02;  /* S2: PA12 */
    if (!(valA & DL_GPIO_PIN_18)) result |= 0x04;  /* S3: PA18 */
    if (!(valB & DL_GPIO_PIN_3))  result |= 0x08;  /* S4: PB3  */
    if (!(valB & DL_GPIO_PIN_9))  result |= 0x10;  /* S5: PB9  */
    if (!(valA & DL_GPIO_PIN_9))  result |= 0x20;  /* S6: PA9  */
    if (!(valB & DL_GPIO_PIN_19)) result |= 0x40;  /* S7: PB19 */

    return result;
}

/* ========================================================================
 * 统计检测到黑线的传感器数量 (popcount)
 * ========================================================================*/

static uint8_t Count_Active(uint8_t data)
{
    uint8_t cnt = 0;
    while (data) {
        cnt++;
        data &= data - 1;
    }
    return cnt;
}

/* ========================================================================
 * OLED 显示传感器状态
 * ========================================================================*/

void OLED_Show_Sensors(uint8_t data)
{
    char buf[32];
    uint8_t i;

    /* 行0: 调试 — GPIO 原始值 */
    {
        uint32_t rawA = DL_GPIO_readPins(GPIOA, MASK_A);
        uint32_t rawB = DL_GPIO_readPins(GPIOB, MASK_B);
        uint8_t pos = 0;
        buf[pos++] = 'A'; buf[pos++] = ':';
        buf[pos++] = (rawA & DL_GPIO_PIN_12) ? '1' : '0';  /* S2 */
        buf[pos++] = (rawA & DL_GPIO_PIN_18) ? '1' : '0';  /* S3 */
        buf[pos++] = (rawA & DL_GPIO_PIN_9)  ? '1' : '0';  /* S6 */
        buf[pos++] = ' ';
        buf[pos++] = 'B'; buf[pos++] = ':';
        buf[pos++] = (rawB & DL_GPIO_PIN_8)  ? '1' : '0';  /* S1 */
        buf[pos++] = (rawB & DL_GPIO_PIN_19) ? '1' : '0';  /* S7 */
        buf[pos++] = (rawB & DL_GPIO_PIN_9)  ? '1' : '0';  /* S5 */
        buf[pos++] = (rawB & DL_GPIO_PIN_3)  ? '1' : '0';  /* S4 */
        buf[pos] = '\0';
        OLED_ShowString(0, 0, (const uint8_t *)buf);
    }

    /* 行1: 7位二进制 S1..S7 */
    for (i = 0; i < 7; i++) {
        uint8_t bit = (data >> i) & 0x01;
        OLED_ShowChar(16 + i * 14, 13, bit ? '1' : '0', 16, 1);
    }

    /* 行2: 状态条 */
    for (i = 0; i < 7; i++) {
        uint8_t bit = (data >> i) & 0x01;
        uint8_t x  = i * 18;
        OLED_ShowChar(x, 31, bit ? '#' : '_', 16, 1);
        OLED_ShowChar(x + 4, 34, '1' + i, 12, 1);
    }

    /* 行3: HEX值 + 检测到的传感器数 */
    {
        uint8_t hex_hi = (data >> 4) & 0x0F;
        uint8_t hex_lo = data & 0x0F;
        uint8_t cnt = Count_Active(data);
        uint8_t p = 0;
        buf[p++] = 'H'; buf[p++] = 'E'; buf[p++] = 'X';
        buf[p++] = ':'; buf[p++] = '0'; buf[p++] = 'x';
        buf[p++] = hex_hi > 9 ? 'A' + hex_hi - 10 : '0' + hex_hi;
        buf[p++] = hex_lo > 9 ? 'A' + hex_lo - 10 : '0' + hex_lo;
        buf[p++] = ' '; buf[p++] = 'C'; buf[p++] = ':';
        buf[p++] = '0' + cnt;
        buf[p] = '\0';
        OLED_ShowString(0, 50, (const uint8_t *)buf);
    }
}

/* ========================================================================
 * 主函数
 * ========================================================================*/

int main(void)
{
    SYSCFG_DL_init();

    Sensor_Init();
    OLED_Init();

    OLED_ShowString(0, 0, (const uint8_t *)"Init Done...");
    OLED_Refresh_Gram();
    delay_cycles(32000000);

    while (1)
    {
        uint8_t sensor_data = Read_Sensors();

        memset(OLED_GRAM, 0, sizeof(OLED_GRAM));
        OLED_Show_Sensors(sensor_data);
        OLED_Refresh_Gram();

        delay_cycles(3200000);
    }
}
