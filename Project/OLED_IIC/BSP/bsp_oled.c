/**
 * @file  bsp_oled.c
 * @brief 0.96-inch OLED driver implementation (SSD1306, software I2C, 128x64)
 *
 * Based on OLED-V2.0 by jiangxiekeji.com
 * Ported to MSPM0G3507 (TI Arm Clang)
 *
 * Pins: SCL=PA28  SDA=PA31
 * SSD1306 I2C address: 0x3C (7-bit) / 0x78 (8-bit write)
 *
 * Data storage format:
 *   LSB = top pixel in a page (8 vertical pixels)
 *   Left column to right column, top page to bottom page
 *   Coordinate system: Top-left = (0,0), X→right (0~127), Y↓down (0~63)
 */

#include "bsp_oled.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>

/* ========================================================================
 * Pin definitions
 * ======================================================================== */

/* SCL — PA28, IOMUX_PINCM3 */
#define OLED_SCL_PORT            GPIOA
#define OLED_SCL_PIN             DL_GPIO_PIN_28
#define OLED_SCL_IOMUX           IOMUX_PINCM3

/* SDA — PA31, IOMUX_PINCM6 */
#define OLED_SDA_PORT            GPIOA
#define OLED_SDA_PIN             DL_GPIO_PIN_31
#define OLED_SDA_IOMUX           IOMUX_PINCM6

/* ========================================================================
 * I2C address
 * ======================================================================== */

#define OLED_I2C_ADDR            0x78

/* ========================================================================
 * Local delay helpers
 * ======================================================================== */

static void OLED_DelayMs(uint32_t ms)
{
    delay_cycles(ms * 32000UL);
}

/* I2C bit delay (~2.5us → I2C ~200kHz at 32MHz) */
static void i2c_delay(void)
{
    delay_cycles(80);
}

/* ========================================================================
 * Framebuffer (page-major: 8 pages × 128 columns)
 * ======================================================================== */

static uint8_t OLED_DisplayBuf[8][128];

/* ========================================================================
 * Low-level GPIO operations
 * ======================================================================== */

static void OLED_W_SCL(uint8_t BitValue)
{
    if (BitValue)
        DL_GPIO_setPins(OLED_SCL_PORT, OLED_SCL_PIN);
    else
        DL_GPIO_clearPins(OLED_SCL_PORT, OLED_SCL_PIN);
}

static void OLED_W_SDA(uint8_t BitValue)
{
    if (BitValue)
        DL_GPIO_setPins(OLED_SDA_PORT, OLED_SDA_PIN);
    else
        DL_GPIO_clearPins(OLED_SDA_PORT, OLED_SDA_PIN);
}

static void OLED_GPIO_Init(void)
{
    /* Configure SCL and SDA as digital outputs */
    DL_GPIO_initDigitalOutput(OLED_SCL_IOMUX);
    DL_GPIO_initDigitalOutput(OLED_SDA_IOMUX);

    DL_GPIO_clearPins(GPIOA, OLED_SCL_PIN | OLED_SDA_PIN);
    DL_GPIO_enableOutput(GPIOA, OLED_SCL_PIN | OLED_SDA_PIN);

    /* Initial state: SCL low, SDA high (matched to old working code) */
    OLED_W_SCL(0);
    OLED_W_SDA(1);

    /* Wait for OLED power stability (~120ms) */
    OLED_DelayMs(120);
}

/* ========================================================================
 * Software I2C protocol
 * ======================================================================== */

static void OLED_I2C_Start(void)
{
    OLED_W_SDA(1);
    OLED_W_SCL(1);
    i2c_delay();
    OLED_W_SDA(0);
    i2c_delay();
    OLED_W_SCL(0);
}

static void OLED_I2C_Stop(void)
{
    OLED_W_SDA(0);
    OLED_W_SCL(1);
    i2c_delay();
    OLED_W_SDA(1);
    i2c_delay();
}

static void OLED_I2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        OLED_W_SDA(!!(Byte & (0x80 >> i)));
        i2c_delay();
        OLED_W_SCL(1);
        i2c_delay();
        OLED_W_SCL(0);
    }
    /* Extra clock for ACK (we don't check ACK from SSD1306) */
    OLED_W_SCL(1);
    i2c_delay();
    OLED_W_SCL(0);
}

/* ========================================================================
 * OLED command/data write
 * ======================================================================== */

static void OLED_WriteCommand(uint8_t Command)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(OLED_I2C_ADDR);
    OLED_I2C_SendByte(0x00);          /* Control byte: command mode */
    OLED_I2C_SendByte(Command);
    OLED_I2C_Stop();
}

static void OLED_WriteData(uint8_t *Data, uint8_t Count)
{
    uint8_t i;
    OLED_I2C_Start();
    OLED_I2C_SendByte(OLED_I2C_ADDR);
    OLED_I2C_SendByte(0x40);          /* Control byte: data mode */
    for (i = 0; i < Count; i++)
    {
        OLED_I2C_SendByte(Data[i]);
    }
    OLED_I2C_Stop();
}

/* ========================================================================
 * Hardware control
 * ======================================================================== */

/**
 * @brief Set SSD1306 cursor to a specific page and column
 * @param Page  page index (0~7, each page is 8 vertical pixels)
 * @param X     column index (0~127)
 */
static void OLED_SetCursor(uint8_t Page, uint8_t X)
{
    OLED_WriteCommand(0xB0 | Page);                     /* Set page address */
    OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));       /* Set column high nibble */
    OLED_WriteCommand(0x00 | (X & 0x0F));              /* Set column low nibble */
}

/**
 * @brief Initialize SSD1306 with the standard command sequence
 */
void OLED_Init(void)
{
    OLED_GPIO_Init();

    /* ---- SSD1306 initialization sequence ---- */
    OLED_WriteCommand(0xAE);    /* Display OFF */

    OLED_WriteCommand(0xD5);    /* Set display clock divide ratio / oscillator freq */
    OLED_WriteCommand(0x80);

    OLED_WriteCommand(0xA8);    /* Set multiplex ratio */
    OLED_WriteCommand(0x3F);    /* 64 rows */

    OLED_WriteCommand(0xD3);    /* Set display offset */
    OLED_WriteCommand(0x00);

    OLED_WriteCommand(0x40);    /* Set display start line */

    OLED_WriteCommand(0xA1);    /* Segment re-map (left-right: col 127 = SEG0) */

    OLED_WriteCommand(0xC8);    /* COM scan direction (bottom-up → remapped to top-down) */

    OLED_WriteCommand(0xDA);    /* COM pins hardware configuration */
    OLED_WriteCommand(0x12);

    OLED_WriteCommand(0x81);    /* Set contrast */
    OLED_WriteCommand(0xCF);

    OLED_WriteCommand(0xD9);    /* Set pre-charge period */
    OLED_WriteCommand(0xF1);

    OLED_WriteCommand(0xDB);    /* Set VCOMH deselect level */
    OLED_WriteCommand(0x30);

    OLED_WriteCommand(0xA4);    /* Entire display ON (follows RAM content) */

    OLED_WriteCommand(0xA6);    /* Normal (non-inverted) display */

    OLED_WriteCommand(0x8D);    /* Charge pump setting */
    OLED_WriteCommand(0x14);    /* Enable charge pump */

    OLED_WriteCommand(0xAF);    /* Display ON */

    OLED_Clear();
    OLED_Update();
}

/* ========================================================================
 * Buffer management
 * ======================================================================== */

/**
 * @brief Transfer entire framebuffer to the OLED display
 */
void OLED_Update(void)
{
    uint8_t j;
    for (j = 0; j < 8; j++)
    {
        OLED_SetCursor(j, 0);
        OLED_WriteData(OLED_DisplayBuf[j], 128);
    }
}

/**
 * @brief Transfer a rectangular region of the framebuffer to the OLED display
 */
void OLED_UpdateArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
    int16_t j;
    int16_t Page, Page1;

    Page = Y / 8;
    Page1 = (Y + Height - 1) / 8 + 1;
    if (Y < 0)
    {
        Page -= 1;
        Page1 -= 1;
    }

    for (j = Page; j < Page1; j++)
    {
        if (X >= 0 && X <= 127 && j >= 0 && j <= 7)
        {
            OLED_SetCursor(j, X);
            OLED_WriteData(&OLED_DisplayBuf[j][X], Width);
        }
    }
}

/**
 * @brief Clear the entire framebuffer
 */
void OLED_Clear(void)
{
    uint8_t i, j;
    for (j = 0; j < 8; j++)
    {
        for (i = 0; i < 128; i++)
        {
            OLED_DisplayBuf[j][i] = 0x00;
        }
    }
}

/**
 * @brief Clear a rectangular region in the framebuffer
 */
void OLED_ClearArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
    int16_t i, j;
    for (j = Y; j < Y + Height; j++)
    {
        for (i = X; i < X + Width; i++)
        {
            if (i >= 0 && i <= 127 && j >= 0 && j <= 63)
            {
                OLED_DisplayBuf[j / 8][i] &= ~(0x01 << (j % 8));
            }
        }
    }
}

/**
 * @brief Invert the entire framebuffer (XOR with 0xFF)
 */
void OLED_Reverse(void)
{
    uint8_t i, j;
    for (j = 0; j < 8; j++)
    {
        for (i = 0; i < 128; i++)
        {
            OLED_DisplayBuf[j][i] ^= 0xFF;
        }
    }
}

/**
 * @brief Invert a rectangular region in the framebuffer
 */
void OLED_ReverseArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
    int16_t i, j;
    for (j = Y; j < Y + Height; j++)
    {
        for (i = X; i < X + Width; i++)
        {
            if (i >= 0 && i <= 127 && j >= 0 && j <= 63)
            {
                OLED_DisplayBuf[j / 8][i] ^= 0x01 << (j % 8);
            }
        }
    }
}

/* ========================================================================
 * Utility functions (internal)
 * ======================================================================== */

/**
 * @brief Integer power: X^Y
 */
static uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)
    {
        Result *= X;
    }
    return Result;
}

/**
 * @brief Point-in-polygon test (PNPOLY algorithm by W. Randolph Franklin)
 * @return 1 if point is inside polygon, 0 otherwise
 */
static uint8_t OLED_pnpoly(uint8_t nvert, int16_t *vertx, int16_t *verty, int16_t testx, int16_t testy)
{
    int16_t i, j, c = 0;
    for (i = 0, j = nvert - 1; i < nvert; j = i++)
    {
        if (((verty[i] > testy) != (verty[j] > testy)) &&
            (testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]))
        {
            c = !c;
        }
    }
    return c;
}

/**
 * @brief Check if a point is within a specified angle range
 * @param X, Y           point coordinates relative to center
 * @param StartAngle     start angle (-180~180), 0° = right, clockwise positive
 * @param EndAngle       end angle (-180~180)
 * @return 1 if inside angle range, 0 otherwise
 */
static uint8_t OLED_IsInAngle(int16_t X, int16_t Y, int16_t StartAngle, int16_t EndAngle)
{
    int16_t PointAngle;
    PointAngle = atan2(Y, X) / 3.14 * 180;

    if (StartAngle < EndAngle)
    {
        if (PointAngle >= StartAngle && PointAngle <= EndAngle)
        {
            return 1;
        }
    }
    else
    {
        if (PointAngle >= StartAngle || PointAngle <= EndAngle)
        {
            return 1;
        }
    }
    return 0;
}

/* ========================================================================
 * Text display functions
 * ======================================================================== */

/**
 * @brief Display a single ASCII character
 */
void OLED_ShowChar(int16_t X, int16_t Y, char Char, uint8_t FontSize)
{
    if (FontSize == OLED_8X16)
    {
        OLED_ShowImage(X, Y, 8, 16, OLED_F8x16[Char - ' ']);
    }
    else if (FontSize == OLED_6X8)
    {
        OLED_ShowImage(X, Y, 6, 8, OLED_F6x8[Char - ' ']);
    }
}

/**
 * @brief Display a string (mixed ASCII + GB2312 Chinese, auto-detected)
 *
 * Single-byte (ASCII) chars have bit 7 == 0.
 * Double-byte (GB2312 Chinese) chars have bit 7 == 1 on both bytes.
 */
void OLED_ShowString(int16_t X, int16_t Y, char *String, uint8_t FontSize)
{
    uint16_t i = 0;
    char SingleChar[5];
    uint8_t CharLength = 0;
    uint16_t XOffset = 0;
    uint16_t pIndex;

    while (String[i] != '\0')
    {
#ifdef OLED_CHARSET_UTF8
        /* Extract one UTF-8 character into SingleChar[] */
        if ((String[i] & 0x80) == 0x00)
        {
            CharLength = 1;
            SingleChar[0] = String[i++];
            SingleChar[1] = '\0';
        }
        else if ((String[i] & 0xE0) == 0xC0)
        {
            CharLength = 2;
            SingleChar[0] = String[i++];
            if (String[i] == '\0') { break; }
            SingleChar[1] = String[i++];
            SingleChar[2] = '\0';
        }
        else if ((String[i] & 0xF0) == 0xE0)
        {
            CharLength = 3;
            SingleChar[0] = String[i++];
            if (String[i] == '\0') { break; }
            SingleChar[1] = String[i++];
            if (String[i] == '\0') { break; }
            SingleChar[2] = String[i++];
            SingleChar[3] = '\0';
        }
        else if ((String[i] & 0xF8) == 0xF0)
        {
            CharLength = 4;
            SingleChar[0] = String[i++];
            if (String[i] == '\0') { break; }
            SingleChar[1] = String[i++];
            if (String[i] == '\0') { break; }
            SingleChar[2] = String[i++];
            if (String[i] == '\0') { break; }
            SingleChar[3] = String[i++];
            SingleChar[4] = '\0';
        }
        else
        {
            i++;
            continue;
        }
#endif

#ifdef OLED_CHARSET_GB2312
        /* Extract one GB2312 character into SingleChar[] */
        if ((String[i] & 0x80) == 0x00)
        {
            CharLength = 1;
            SingleChar[0] = String[i++];
            SingleChar[1] = '\0';
        }
        else
        {
            CharLength = 2;
            SingleChar[0] = String[i++];
            if (String[i] == '\0') { break; }
            SingleChar[1] = String[i++];
            SingleChar[2] = '\0';
        }
#endif

        /* Display the extracted character */
        if (CharLength == 1)
        {
            OLED_ShowChar(X + XOffset, Y, SingleChar[0], FontSize);
            XOffset += FontSize;
        }
        else
        {
            /* Search for matching Chinese character in font table */
            for (pIndex = 0; strcmp(OLED_CF16x16[pIndex].Index, "") != 0; pIndex++)
            {
                if (strcmp(OLED_CF16x16[pIndex].Index, SingleChar) == 0)
                {
                    break;
                }
            }

            if (FontSize == OLED_8X16)
            {
                OLED_ShowImage(X + XOffset, Y, 16, 16, OLED_CF16x16[pIndex].Data);
                XOffset += 16;
            }
            else if (FontSize == OLED_6X8)
            {
                /* Not enough space for 16x16 Chinese, show '?' instead */
                OLED_ShowChar(X + XOffset, Y, '?', OLED_6X8);
                XOffset += OLED_6X8;
            }
        }
    }
}

/**
 * @brief Display an unsigned decimal integer
 */
void OLED_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(X + i * FontSize, Y,
                      Number / OLED_Pow(10, Length - i - 1) % 10 + '0', FontSize);
    }
}

/**
 * @brief Display a signed decimal integer
 */
void OLED_ShowSignedNum(int16_t X, int16_t Y, int32_t Number, uint8_t Length, uint8_t FontSize)
{
    uint8_t i;
    uint32_t Number1;

    if (Number >= 0)
    {
        OLED_ShowChar(X, Y, '+', FontSize);
        Number1 = Number;
    }
    else
    {
        OLED_ShowChar(X, Y, '-', FontSize);
        Number1 = -Number;
    }

    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(X + (i + 1) * FontSize, Y,
                      Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0', FontSize);
    }
}

/**
 * @brief Display a hexadecimal number
 */
void OLED_ShowHexNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
    uint8_t i, SingleNumber;
    for (i = 0; i < Length; i++)
    {
        SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
        if (SingleNumber < 10)
        {
            OLED_ShowChar(X + i * FontSize, Y, SingleNumber + '0', FontSize);
        }
        else
        {
            OLED_ShowChar(X + i * FontSize, Y, SingleNumber - 10 + 'A', FontSize);
        }
    }
}

/**
 * @brief Display a binary number
 */
void OLED_ShowBinNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(X + i * FontSize, Y,
                      Number / OLED_Pow(2, Length - i - 1) % 2 + '0', FontSize);
    }
}

/**
 * @brief Display a floating-point number
 */
void OLED_ShowFloatNum(int16_t X, int16_t Y, double Number, uint8_t IntLength,
                       uint8_t FraLength, uint8_t FontSize)
{
    uint32_t PowNum, IntNum, FraNum;

    if (Number >= 0)
    {
        OLED_ShowChar(X, Y, '+', FontSize);
    }
    else
    {
        OLED_ShowChar(X, Y, '-', FontSize);
        Number = -Number;
    }

    /* Separate integer and fraction parts */
    IntNum = Number;
    Number -= IntNum;
    PowNum = OLED_Pow(10, FraLength);
    FraNum = round(Number * PowNum);
    IntNum += FraNum / PowNum;      /* Handle rounding overflow carry */

    /* Display integer part */
    OLED_ShowNum(X + FontSize, Y, IntNum, IntLength, FontSize);

    /* Display decimal point */
    OLED_ShowChar(X + (IntLength + 1) * FontSize, Y, '.', FontSize);

    /* Display fraction part */
    OLED_ShowNum(X + (IntLength + 2) * FontSize, Y, FraNum, FraLength, FontSize);
}

/**
 * @brief Display an image (used as the core rendering primitive for all text)
 *
 * Handles vertical pixel offset (Y not aligned to page boundary) by
 * splitting data across two pages.
 */
void OLED_ShowImage(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image)
{
    uint8_t i = 0, j = 0;
    int16_t Page, Shift;

    /* Clear the target area first */
    OLED_ClearArea(X, Y, Width, Height);

    /* Iterate over each page slice of the image */
    for (j = 0; j < (Height - 1) / 8 + 1; j++)
    {
        for (i = 0; i < Width; i++)
        {
            if (X + i >= 0 && X + i <= 127)
            {
                Page = Y / 8;
                Shift = Y % 8;
                if (Y < 0)
                {
                    Page -= 1;
                    Shift += 8;
                }

                if (Page + j >= 0 && Page + j <= 7)
                {
                    OLED_DisplayBuf[Page + j][X + i] |= Image[j * Width + i] << Shift;
                }

                if (Page + j + 1 >= 0 && Page + j + 1 <= 7)
                {
                    OLED_DisplayBuf[Page + j + 1][X + i] |= Image[j * Width + i] >> (8 - Shift);
                }
            }
        }
    }
}

/**
 * @brief printf-style formatted string display
 */
void OLED_Printf(int16_t X, int16_t Y, uint8_t FontSize, char *format, ...)
{
    char String[256];
    va_list arg;
    va_start(arg, format);
    vsnprintf(String, sizeof(String), format, arg);
    va_end(arg);
    OLED_ShowString(X, Y, String, FontSize);
}

/* ========================================================================
 * Graphics primitives
 * ======================================================================== */

/**
 * @brief Draw a single pixel
 */
void OLED_DrawPoint(int16_t X, int16_t Y)
{
    if (X >= 0 && X <= 127 && Y >= 0 && Y <= 63)
    {
        OLED_DisplayBuf[Y / 8][X] |= 0x01 << (Y % 8);
    }
}

/**
 * @brief Read a single pixel's state
 * @return 1 if pixel is lit, 0 if off
 */
uint8_t OLED_GetPoint(int16_t X, int16_t Y)
{
    if (X >= 0 && X <= 127 && Y >= 0 && Y <= 63)
    {
        if (OLED_DisplayBuf[Y / 8][X] & (0x01 << (Y % 8)))
        {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Draw a line using Bresenham's algorithm
 */
void OLED_DrawLine(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1)
{
    int16_t x, y, dx, dy, d, incrE, incrNE, temp;
    int16_t x0 = X0, y0 = Y0, x1 = X1, y1 = Y1;
    uint8_t yflag = 0, xyflag = 0;

    if (y0 == y1)           /* Horizontal line */
    {
        if (x0 > x1) { temp = x0; x0 = x1; x1 = temp; }
        for (x = x0; x <= x1; x++)
        {
            OLED_DrawPoint(x, y0);
        }
    }
    else if (x0 == x1)      /* Vertical line */
    {
        if (y0 > y1) { temp = y0; y0 = y1; y1 = temp; }
        for (y = y0; y <= y1; y++)
        {
            OLED_DrawPoint(x0, y);
        }
    }
    else                    /* Diagonal line — Bresenham */
    {
        /* Normalize: ensure x0 <= x1 */
        if (x0 > x1)
        {
            temp = x0; x0 = x1; x1 = temp;
            temp = y0; y0 = y1; y1 = temp;
        }

        /* Mirror Y if needed to work in quadrant 1 */
        if (y0 > y1)
        {
            y0 = -y0;
            y1 = -y1;
            yflag = 1;
        }

        /* Swap X/Y if slope > 1 to keep slope in 0~1 range */
        if (y1 - y0 > x1 - x0)
        {
            temp = x0; x0 = y0; y0 = temp;
            temp = x1; x1 = y1; y1 = temp;
            xyflag = 1;
        }

        /* Bresenham core */
        dx = x1 - x0;
        dy = y1 - y0;
        incrE = 2 * dy;
        incrNE = 2 * (dy - dx);
        d = 2 * dy - dx;
        x = x0;
        y = y0;

        /* Draw start point with coordinate transforms applied */
        if (yflag && xyflag)      { OLED_DrawPoint(y, -x); }
        else if (yflag)           { OLED_DrawPoint(x, -y); }
        else if (xyflag)          { OLED_DrawPoint(y, x);  }
        else                      { OLED_DrawPoint(x, y);  }

        while (x < x1)
        {
            x++;
            if (d < 0)
            {
                d += incrE;
            }
            else
            {
                y++;
                d += incrNE;
            }

            if (yflag && xyflag)      { OLED_DrawPoint(y, -x); }
            else if (yflag)           { OLED_DrawPoint(x, -y); }
            else if (xyflag)          { OLED_DrawPoint(y, x);  }
            else                      { OLED_DrawPoint(x, y);  }
        }
    }
}

/**
 * @brief Draw a rectangle (outline or filled)
 */
void OLED_DrawRectangle(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, uint8_t IsFilled)
{
    int16_t i, j;
    if (!IsFilled)
    {
        /* Horizontal edges */
        for (i = X; i < X + Width; i++)
        {
            OLED_DrawPoint(i, Y);
            OLED_DrawPoint(i, Y + Height - 1);
        }
        /* Vertical edges */
        for (i = Y; i < Y + Height; i++)
        {
            OLED_DrawPoint(X, i);
            OLED_DrawPoint(X + Width - 1, i);
        }
    }
    else
    {
        for (i = X; i < X + Width; i++)
        {
            for (j = Y; j < Y + Height; j++)
            {
                OLED_DrawPoint(i, j);
            }
        }
    }
}

/**
 * @brief Draw a triangle (outline or filled)
 */
void OLED_DrawTriangle(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1,
                       int16_t X2, int16_t Y2, uint8_t IsFilled)
{
    int16_t minx = X0, miny = Y0, maxx = X0, maxy = Y0;
    int16_t i, j;
    int16_t vx[] = {X0, X1, X2};
    int16_t vy[] = {Y0, Y1, Y2};

    if (!IsFilled)
    {
        OLED_DrawLine(X0, Y0, X1, Y1);
        OLED_DrawLine(X0, Y0, X2, Y2);
        OLED_DrawLine(X1, Y1, X2, Y2);
    }
    else
    {
        /* Find bounding box */
        if (X1 < minx) { minx = X1; }
        if (X2 < minx) { minx = X2; }
        if (Y1 < miny) { miny = Y1; }
        if (Y2 < miny) { miny = Y2; }
        if (X1 > maxx) { maxx = X1; }
        if (X2 > maxx) { maxx = X2; }
        if (Y1 > maxy) { maxy = Y1; }
        if (Y2 > maxy) { maxy = Y2; }

        for (i = minx; i <= maxx; i++)
        {
            for (j = miny; j <= maxy; j++)
            {
                if (OLED_pnpoly(3, vx, vy, i, j))
                {
                    OLED_DrawPoint(i, j);
                }
            }
        }
    }
}

/**
 * @brief Draw a circle using Bresenham's midpoint algorithm
 */
void OLED_DrawCircle(int16_t X, int16_t Y, uint8_t Radius, uint8_t IsFilled)
{
    int16_t x, y, d, j;

    d = 1 - Radius;
    x = 0;
    y = Radius;

    /* Draw initial points (4 octants) */
    OLED_DrawPoint(X + x, Y + y);
    OLED_DrawPoint(X - x, Y - y);
    OLED_DrawPoint(X + y, Y + x);
    OLED_DrawPoint(X - y, Y - x);

    if (IsFilled)
    {
        for (j = -y; j < y; j++)
        {
            OLED_DrawPoint(X, Y + j);
        }
    }

    while (x < y)
    {
        x++;
        if (d < 0)
        {
            d += 2 * x + 1;
        }
        else
        {
            y--;
            d += 2 * (x - y) + 1;
        }

        /* Draw all 8 octant points */
        OLED_DrawPoint(X + x, Y + y);
        OLED_DrawPoint(X + y, Y + x);
        OLED_DrawPoint(X - x, Y - y);
        OLED_DrawPoint(X - y, Y - x);
        OLED_DrawPoint(X + x, Y - y);
        OLED_DrawPoint(X + y, Y - x);
        OLED_DrawPoint(X - x, Y + y);
        OLED_DrawPoint(X - y, Y + x);

        if (IsFilled)
        {
            for (j = -y; j < y; j++)
            {
                OLED_DrawPoint(X + x, Y + j);
                OLED_DrawPoint(X - x, Y + j);
            }
            for (j = -x; j < x; j++)
            {
                OLED_DrawPoint(X - y, Y + j);
                OLED_DrawPoint(X + y, Y + j);
            }
        }
    }
}

/**
 * @brief Draw an ellipse using Bresenham's algorithm
 */
void OLED_DrawEllipse(int16_t X, int16_t Y, uint8_t A, uint8_t B, uint8_t IsFilled)
{
    int16_t x, y, j;
    int16_t a = A, b = B;
    float d1, d2;

    x = 0;
    y = b;
    d1 = b * b + a * a * (-b + 0.5);

    if (IsFilled)
    {
        for (j = -y; j < y; j++)
        {
            OLED_DrawPoint(X, Y + j);
        }
    }

    /* Region 1: initial points */
    OLED_DrawPoint(X + x, Y + y);
    OLED_DrawPoint(X - x, Y - y);
    OLED_DrawPoint(X - x, Y + y);
    OLED_DrawPoint(X + x, Y - y);

    /* Region 1: upper half */
    while (b * b * (x + 1) < a * a * (y - 0.5))
    {
        if (d1 <= 0)
        {
            d1 += b * b * (2 * x + 3);
        }
        else
        {
            d1 += b * b * (2 * x + 3) + a * a * (-2 * y + 2);
            y--;
        }
        x++;

        if (IsFilled)
        {
            for (j = -y; j < y; j++)
            {
                OLED_DrawPoint(X + x, Y + j);
                OLED_DrawPoint(X - x, Y + j);
            }
        }

        OLED_DrawPoint(X + x, Y + y);
        OLED_DrawPoint(X - x, Y - y);
        OLED_DrawPoint(X - x, Y + y);
        OLED_DrawPoint(X + x, Y - y);
    }

    /* Region 2 */
    d2 = b * b * (x + 0.5) * (x + 0.5) + a * a * (y - 1) * (y - 1) - a * a * b * b;

    while (y > 0)
    {
        if (d2 <= 0)
        {
            d2 += b * b * (2 * x + 2) + a * a * (-2 * y + 3);
            x++;
        }
        else
        {
            d2 += a * a * (-2 * y + 3);
        }
        y--;

        if (IsFilled)
        {
            for (j = -y; j < y; j++)
            {
                OLED_DrawPoint(X + x, Y + j);
                OLED_DrawPoint(X - x, Y + j);
            }
        }

        OLED_DrawPoint(X + x, Y + y);
        OLED_DrawPoint(X - x, Y - y);
        OLED_DrawPoint(X - x, Y + y);
        OLED_DrawPoint(X + x, Y - y);
    }
}

/**
 * @brief Draw a circular arc with angular clipping
 */
void OLED_DrawArc(int16_t X, int16_t Y, uint8_t Radius, int16_t StartAngle,
                  int16_t EndAngle, uint8_t IsFilled)
{
    int16_t x, y, d, j;

    d = 1 - Radius;
    x = 0;
    y = Radius;

    /* Initial points with angle check */
    if (OLED_IsInAngle(x, y, StartAngle, EndAngle))       { OLED_DrawPoint(X + x, Y + y); }
    if (OLED_IsInAngle(-x, -y, StartAngle, EndAngle))     { OLED_DrawPoint(X - x, Y - y); }
    if (OLED_IsInAngle(y, x, StartAngle, EndAngle))       { OLED_DrawPoint(X + y, Y + x); }
    if (OLED_IsInAngle(-y, -x, StartAngle, EndAngle))     { OLED_DrawPoint(X - y, Y - x); }

    if (IsFilled)
    {
        for (j = -y; j < y; j++)
        {
            if (OLED_IsInAngle(0, j, StartAngle, EndAngle))
            {
                OLED_DrawPoint(X, Y + j);
            }
        }
    }

    while (x < y)
    {
        x++;
        if (d < 0)
        {
            d += 2 * x + 1;
        }
        else
        {
            y--;
            d += 2 * (x - y) + 1;
        }

        if (OLED_IsInAngle(x, y, StartAngle, EndAngle))   { OLED_DrawPoint(X + x, Y + y); }
        if (OLED_IsInAngle(y, x, StartAngle, EndAngle))   { OLED_DrawPoint(X + y, Y + x); }
        if (OLED_IsInAngle(-x, -y, StartAngle, EndAngle)) { OLED_DrawPoint(X - x, Y - y); }
        if (OLED_IsInAngle(-y, -x, StartAngle, EndAngle)) { OLED_DrawPoint(X - y, Y - x); }
        if (OLED_IsInAngle(x, -y, StartAngle, EndAngle))  { OLED_DrawPoint(X + x, Y - y); }
        if (OLED_IsInAngle(y, -x, StartAngle, EndAngle))  { OLED_DrawPoint(X + y, Y - x); }
        if (OLED_IsInAngle(-x, y, StartAngle, EndAngle))  { OLED_DrawPoint(X - x, Y + y); }
        if (OLED_IsInAngle(-y, x, StartAngle, EndAngle))  { OLED_DrawPoint(X - y, Y + x); }

        if (IsFilled)
        {
            for (j = -y; j < y; j++)
            {
                if (OLED_IsInAngle(x, j, StartAngle, EndAngle))
                {
                    OLED_DrawPoint(X + x, Y + j);
                }
                if (OLED_IsInAngle(-x, j, StartAngle, EndAngle))
                {
                    OLED_DrawPoint(X - x, Y + j);
                }
            }
            for (j = -x; j < x; j++)
            {
                if (OLED_IsInAngle(-y, j, StartAngle, EndAngle))
                {
                    OLED_DrawPoint(X - y, Y + j);
                }
                if (OLED_IsInAngle(y, j, StartAngle, EndAngle))
                {
                    OLED_DrawPoint(X + y, Y + j);
                }
            }
        }
    }
}
