/**
 * @file  bsp_oled.h
 * @brief 0.96寸 OLED 驱动 (SSD1306, 4线软件SPI, 128x64)
 *
 * 移植自 WHEELTEC C07A OLED 例程 (MSPM0G3507)
 * 引脚: SCL=PA28  SDA=PA31  RST=PB14  DC=PB15
 */

#ifndef BSP_OLED_H
#define BSP_OLED_H

#include "ti_msp_dl_config.h"

#define OLED_CMD  0
#define OLED_DATA 1

extern uint8_t OLED_GRAM[128][8];

void OLED_WR_Byte(uint8_t dat, uint8_t cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode);
void OLED_ShowNumber(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);
void OLED_ShowString(uint8_t x, uint8_t y, const uint8_t *p);
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no, uint8_t font_width, uint8_t font_height);
void OLED_Set_Pos(unsigned char x, unsigned char y);

void OLED_RST_Clr(void);
void OLED_RST_Set(void);
void OLED_RS_Clr(void);
void OLED_RS_Set(void);
void OLED_SCLK_Clr(void);
void OLED_SCLK_Set(void);
void OLED_SDIN_Clr(void);
void OLED_SDIN_Set(void);

#endif /* BSP_OLED_H */
