/**
 * @file  bsp_oled.c
 * @brief 0.96寸 OLED 驱动实现 (SSD1306, 4线软件SPI)
 *
 * 严格移植自 WHEELTEC C07A OLED 例程
 * 引脚: SCL=PA28  SDA=PA31  RST=PB14  DC=PB15
 */

#include "bsp_oled.h"
#include "oledfont.h"
#include <stdlib.h>
#include <stdio.h>

/* ========================================================================
 * 引脚宏 (与参考代码 ti_msp_dl_config.h 一致的命名)
 * ======================================================================== */

/* RST — PB14, IOMUX_PINCM31 */
#define OLED_RST_PORT            GPIOB
#define OLED_RST_PIN_RST_PIN     DL_GPIO_PIN_14
#define OLED_RST_PIN_RST_IOMUX   IOMUX_PINCM31

/* DC — PB15, IOMUX_PINCM32 */
#define OLED_DC_PORT             GPIOB
#define OLED_DC_PIN_DC_PIN       DL_GPIO_PIN_15
#define OLED_DC_PIN_DC_IOMUX     IOMUX_PINCM32

/* SCL — PA28, IOMUX_PINCM3 */
#define OLED_SCL_PORT            GPIOA
#define OLED_SCL_PIN_SCL_PIN     DL_GPIO_PIN_28
#define OLED_SCL_PIN_SCL_IOMUX   IOMUX_PINCM3

/* SDA — PA31, IOMUX_PINCM6 */
#define OLED_SDA_PORT            GPIOA
#define OLED_SDA_PIN_SDA_PIN     DL_GPIO_PIN_31
#define OLED_SDA_PIN_SDA_IOMUX   IOMUX_PINCM6

/* CS — PA22 */
#define OLED_CS_PORT             GPIOA
#define OLED_CS_PIN_CS_PIN       DL_GPIO_PIN_22
#define OLED_CS_PIN_CS_IOMUX     IOMUX_PINCM47

/* ========================================================================
 * 本地延时 (纯循环, 不依赖 SysTick/delay_cycles)
 * ======================================================================== */

static void delay_ms(uint32_t ms)
{
    delay_cycles(ms * 32000UL);  /* 32000 cycles/ms @ 32MHz SYSOSC */
}

/* ========================================================================
 * 显存
 * ======================================================================== */

uint8_t OLED_GRAM[128][8];

/* ========================================================================
 * 底层 GPIO 操作 (与参考代码完全一致)
 * ======================================================================== */

void OLED_RST_Clr(void)
{
    DL_GPIO_clearPins(OLED_RST_PORT, OLED_RST_PIN_RST_PIN);
}

void OLED_RST_Set(void)
{
    DL_GPIO_setPins(OLED_RST_PORT, OLED_RST_PIN_RST_PIN);
}

void OLED_RS_Clr(void)
{
    DL_GPIO_clearPins(OLED_DC_PORT, OLED_DC_PIN_DC_PIN);  /* DC */
}

void OLED_RS_Set(void)
{
    DL_GPIO_setPins(OLED_DC_PORT, OLED_DC_PIN_DC_PIN);    /* DC */
}

void OLED_SCLK_Clr(void)
{
    DL_GPIO_clearPins(OLED_SCL_PORT, OLED_SCL_PIN_SCL_PIN);  /* SCL */
}

void OLED_SCLK_Set(void)
{
    DL_GPIO_setPins(OLED_SCL_PORT, OLED_SCL_PIN_SCL_PIN);    /* SCL */
}

void OLED_SDIN_Clr(void)
{
    DL_GPIO_clearPins(OLED_SDA_PORT, OLED_SDA_PIN_SDA_PIN);  /* SDA */
}

void OLED_SDIN_Set(void)
{
    DL_GPIO_setPins(OLED_SDA_PORT, OLED_SDA_PIN_SDA_PIN);    /* SDA */
}

static void OLED_CS_Clr(void)
{
    DL_GPIO_clearPins(OLED_CS_PORT, OLED_CS_PIN_CS_PIN);     /* CS */
}

static void OLED_CS_Set(void)
{
    DL_GPIO_setPins(OLED_CS_PORT, OLED_CS_PIN_CS_PIN);       /* CS */
}

/* ========================================================================
 * 软件SPI 写一字节 (与参考代码完全一致)
 * ======================================================================== */

void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
    uint8_t i;
    if (cmd)
        OLED_RS_Set();
    else
        OLED_RS_Clr();

    OLED_CS_Clr();
    for (i = 0; i < 8; i++) {
        OLED_SCLK_Clr();
        if (dat & 0x80)
            OLED_SDIN_Set();
        else
            OLED_SDIN_Clr();
        __asm volatile ("nop");
        __asm volatile ("nop");
        __asm volatile ("nop");
        OLED_SCLK_Set();
        __asm volatile ("nop");
        __asm volatile ("nop");
        __asm volatile ("nop");
        dat <<= 1;
    }
    OLED_CS_Set();
    OLED_RS_Set();
    OLED_SCLK_Clr();
}

/* ========================================================================
 * 设置显示坐标
 * ======================================================================== */

void OLED_Set_Pos(unsigned char x, unsigned char y)
{
    OLED_WR_Byte(0xb0 + y, OLED_CMD);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte((x & 0x0f), OLED_CMD);
}

/* ========================================================================
 * 开/关显示
 * ======================================================================== */

void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD);  /* SET DCDC */
    OLED_WR_Byte(0X14, OLED_CMD);  /* DCDC ON  */
    OLED_WR_Byte(0XAF, OLED_CMD);  /* DISPLAY ON */
}

void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD);  /* SET DCDC  */
    OLED_WR_Byte(0X10, OLED_CMD);  /* DCDC OFF  */
    OLED_WR_Byte(0XAE, OLED_CMD);  /* DISPLAY OFF */
}

/* ========================================================================
 * 清屏
 * ======================================================================== */

void OLED_Clear(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
        for (n = 0; n < 128; n++)
            OLED_GRAM[n][i] = 0X00;
    OLED_Refresh_Gram();
}

/* ========================================================================
 * 刷新显存到屏幕
 * ======================================================================== */

/* SSD1306 初始化命令序列 (不含 RST, 每次刷新前重发以复位状态机) */
static void OLED_SendInitSeq(void)
{
    OLED_WR_Byte(0xAE, OLED_CMD); /* 关闭显示 */
    OLED_WR_Byte(0xD5, OLED_CMD);
    OLED_WR_Byte(80, OLED_CMD);
    OLED_WR_Byte(0xA8, OLED_CMD);
    OLED_WR_Byte(0X3F, OLED_CMD);
    OLED_WR_Byte(0xD3, OLED_CMD);
    OLED_WR_Byte(0X00, OLED_CMD);
    OLED_WR_Byte(0x40, OLED_CMD);
    OLED_WR_Byte(0x8D, OLED_CMD);
    OLED_WR_Byte(0x14, OLED_CMD);
    OLED_WR_Byte(0x20, OLED_CMD);
    OLED_WR_Byte(0x02, OLED_CMD);
    OLED_WR_Byte(0xA1, OLED_CMD);
    OLED_WR_Byte(0xC0, OLED_CMD);
    OLED_WR_Byte(0xDA, OLED_CMD);
    OLED_WR_Byte(0x12, OLED_CMD);
    OLED_WR_Byte(0x81, OLED_CMD);
    OLED_WR_Byte(0xEF, OLED_CMD);
    OLED_WR_Byte(0xD9, OLED_CMD);
    OLED_WR_Byte(0xf1, OLED_CMD);
    OLED_WR_Byte(0xDB, OLED_CMD);
    OLED_WR_Byte(0x30, OLED_CMD);
    OLED_WR_Byte(0xA4, OLED_CMD);
    OLED_WR_Byte(0xA6, OLED_CMD);
    OLED_WR_Byte(0xAF, OLED_CMD); /* 开显示 */
}

void OLED_Refresh_Gram(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++) {
        OLED_WR_Byte(0xb0 + i, OLED_CMD);
        OLED_WR_Byte(0x00, OLED_CMD);
        OLED_WR_Byte(0x10, OLED_CMD);
        for (n = 0; n < 128; n++)
            OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
    }
}

/* ========================================================================
 * 画点
 * ======================================================================== */

void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
    uint8_t pos, bx, temp = 0;
    if (x > 127 || y > 63) return;
    pos  = 7 - y / 8;
    bx   = y % 8;
    temp = 1 << (7 - bx);
    if (t)
        OLED_GRAM[x][pos] |= temp;
    else
        OLED_GRAM[x][pos] &= ~temp;
}

/* ========================================================================
 * 显示字符 (size=12 用 1206, size=16 用 1608)
 * ======================================================================== */

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
    uint8_t temp, t, t1;
    uint8_t y0 = y;
    chr = chr - ' ';

    for (t = 0; t < size; t++) {
        if (size == 12)
            temp = oled_asc2_1206[chr][t];
        else
            temp = oled_asc2_1608[chr][t];

        for (t1 = 0; t1 < 8; t1++) {
            if (temp & 0x80)
                OLED_DrawPoint(x, y, mode);
            else
                OLED_DrawPoint(x, y, !mode);
            temp <<= 1;
            y++;
            if ((y - y0) == size) {
                y = y0;
                x++;
                break;
            }
        }
    }
}

/* ========================================================================
 * 辅助: m^n
 * ======================================================================== */

static uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--) result *= m;
    return result;
}

/* ========================================================================
 * 显示数字
 * ======================================================================== */

void OLED_ShowNumber(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for (t = 0; t < len; t++) {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1)) {
            if (temp == 0) {
                OLED_ShowChar(x + (size / 2) * t, y, ' ', size, 1);
                continue;
            } else {
                enshow = 1;
            }
        }
        OLED_ShowChar(x + (size / 2) * t, y, temp + '0', size, 1);
    }
}

/* ========================================================================
 * 显示字符串 (自动换行)
 * ======================================================================== */

void OLED_ShowString(uint8_t x, uint8_t y, const uint8_t *p)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 58
    while (*p != '\0') {
        if (x > MAX_CHAR_POSX) { x = 0; y += 16; }
        if (y > MAX_CHAR_POSY) { y = x = 0; OLED_Clear(); }
        OLED_ShowChar(x, y, *p, 12, 1);
        x += 8;
        p++;
    }
}

/* ========================================================================
 * 显示中文 (Hzk16 字库, 16x16)
 * ======================================================================== */

void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no,
                      uint8_t font_width, uint8_t font_height)
{
    uint8_t t, i;
    for (i = 0; i < (font_height / 8); i++) {
        OLED_Set_Pos(x, y + i);
        for (t = 0; t < font_width; t++) {
            OLED_WR_Byte(Hzk16[(font_height / 8) * no + i][t], OLED_DATA);
        }
    }
}

/* ========================================================================
 * 初始化
 *
 * GPIO 初始化严格遵循 SysConfig 生成代码的模式:
 *   DL_GPIO_initDigitalOutput → DL_GPIO_clearPins → DL_GPIO_enableOutput
 *
 * 注意: SYSCFG_DL_initPower() 已调用 DL_GPIO_reset(GPIOA/B),
 * 所以 OLED 引脚在上电时被复位为输入状态, 此处重新配置为输出.
 * ======================================================================== */

void OLED_Init(void)
{
    /* ---- 1. GPIO 初始化 ---- */
    DL_GPIO_initDigitalOutput(OLED_RST_PIN_RST_IOMUX);
    DL_GPIO_initDigitalOutput(OLED_DC_PIN_DC_IOMUX);
    DL_GPIO_initDigitalOutput(OLED_SCL_PIN_SCL_IOMUX);
    DL_GPIO_initDigitalOutput(OLED_SDA_PIN_SDA_IOMUX);
    DL_GPIO_initDigitalOutput(OLED_CS_PIN_CS_IOMUX);

    DL_GPIO_clearPins(GPIOA, OLED_SCL_PIN_SCL_PIN | OLED_SDA_PIN_SDA_PIN
                            | OLED_CS_PIN_CS_PIN);
    DL_GPIO_enableOutput(GPIOA, OLED_SCL_PIN_SCL_PIN | OLED_SDA_PIN_SDA_PIN
                               | OLED_CS_PIN_CS_PIN);
    DL_GPIO_clearPins(GPIOB, OLED_RST_PIN_RST_PIN | OLED_DC_PIN_DC_PIN);
    DL_GPIO_enableOutput(GPIOB, OLED_RST_PIN_RST_PIN | OLED_DC_PIN_DC_PIN);

    OLED_CS_Set();   /* CS 初始拉高 */

    /* ---- 2. OLED 硬件复位 ---- */
    OLED_RST_Clr();
    delay_ms(120);
    OLED_RST_Set();

    /* ---- 3. SSD1306 初始化序列 ---- */
    OLED_SendInitSeq();
    OLED_Clear();
}
