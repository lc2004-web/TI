/**
 * @file  bsp_oled.h
 * @brief 0.96-inch OLED driver (SSD1306, I2C, 128x64)
 *
 * Based on OLED-V2.0 by jiangxiekeji.com
 * Ported to MSPM0G3507 with software I2C
 *
 * Pins: SCL=PA28  SDA=PA31
 * SSD1306 I2C address: 0x3C (7-bit) / 0x78 (8-bit write)
 */

#ifndef BSP_OLED_H
#define BSP_OLED_H

#include <stdint.h>
#include "ti_msp_dl_config.h"
#include "oledfont.h"

/* ========================================================================
 * Font size constants
 * ======================================================================== */
#define OLED_8X16               8
#define OLED_6X8                6

/* ========================================================================
 * Fill mode constants
 * ======================================================================== */
#define OLED_UNFILLED           0
#define OLED_FILLED             1

/* ========================================================================
 * Framebuffer (for backward compatibility — maps to internal buffer)
 * ======================================================================== */
extern uint8_t OLED_GRAM[128][8];

/* ========================================================================
 * Initialization
 * ======================================================================== */
void OLED_Init(void);

/* ========================================================================
 * Buffer management (call after drawing to update the physical display)
 * ======================================================================== */
void OLED_Update(void);
void OLED_UpdateArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);
void OLED_Clear(void);
void OLED_ClearArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);
void OLED_Reverse(void);
void OLED_ReverseArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);

/* ========================================================================
 * Text display
 * ======================================================================== */
void OLED_ShowChar(int16_t X, int16_t Y, char Char, uint8_t FontSize);
void OLED_ShowString(int16_t X, int16_t Y, char *String, uint8_t FontSize);
void OLED_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowSignedNum(int16_t X, int16_t Y, int32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowHexNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowBinNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowFloatNum(int16_t X, int16_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize);
void OLED_ShowImage(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image);
void OLED_Printf(int16_t X, int16_t Y, uint8_t FontSize, char *format, ...);

/* ========================================================================
 * Graphics primitives
 * ======================================================================== */
void OLED_DrawPoint(int16_t X, int16_t Y);
uint8_t OLED_GetPoint(int16_t X, int16_t Y);
void OLED_DrawLine(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1);
void OLED_DrawRectangle(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, uint8_t IsFilled);
void OLED_DrawTriangle(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint8_t IsFilled);
void OLED_DrawCircle(int16_t X, int16_t Y, uint8_t Radius, uint8_t IsFilled);
void OLED_DrawEllipse(int16_t X, int16_t Y, uint8_t A, uint8_t B, uint8_t IsFilled);
void OLED_DrawArc(int16_t X, int16_t Y, uint8_t Radius, int16_t StartAngle, int16_t EndAngle, uint8_t IsFilled);

#endif /* BSP_OLED_H */
