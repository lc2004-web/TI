/**
 * @file  empty.c
 * @brief OLED driver demo (rewrite based on OLED-V2.0 by jiangxiekeji.com)
 *
 * Demonstrates:
 *   - Mixed ASCII + GB2312 Chinese string display
 *   - Number display (decimal, signed, hex, binary, float)
 *   - Graphics (line, rectangle, circle, ellipse, arc)
 *   - printf-style formatted output
 *   - Partial screen updates
 *
 * Pins: SCL=PA28  SDA=PA31  LED=PA14
 */

#include "ti_msp_dl_config.h"
#include "BSP/bsp_oled.h"
#include <string.h>

int main(void)
{
    uint32_t cnt = 0;

    SYSCFG_DL_init();
    OLED_Init();

    /* ---- Screen 1: Welcome text (Chinese + ASCII mixed) ---- */
    /* "\xBD\xAD\xD0\xAD\xBF\xC6\xBC\xBC" = "江协科技" in GB2312 */
    OLED_ShowString(0, 0, "\xBD\xAD\xD0\xAD\xBF\xC6\xBC\xBC OLED V2.0", OLED_8X16);
    OLED_ShowString(0, 16, "MSPM0G3507 SSD1306", OLED_6X8);
    OLED_ShowString(0, 26, "I2C Addr: 0x3C", OLED_6X8);

    /* Divider line */
    OLED_DrawLine(0, 35, 127, 35);

    /* Number display */
    OLED_ShowString(0, 38, "Dec:", OLED_6X8);
    OLED_ShowNum(30, 38, 2024, 4, OLED_6X8);

    OLED_ShowString(0, 48, "Hex:", OLED_6X8);
    OLED_ShowHexNum(30, 48, 0xABCD, 4, OLED_6X8);

    OLED_ShowString(70, 38, "Bin:", OLED_6X8);
    OLED_ShowBinNum(100, 38, 0x55, 8, OLED_6X8);

    OLED_ShowString(70, 48, "Float:", OLED_6X8);
    OLED_ShowFloatNum(110, 48, 3.14, 1, 2, OLED_6X8);

    OLED_Update();

    /* Wait ~2 seconds */
    delay_cycles(64000000);

    /* ---- Screen 2: Graphics demo ---- */
    OLED_Clear();

    OLED_ShowString(0, 0, "Graphics Demo", OLED_8X16);

    /* Line */
    OLED_DrawLine(0, 20, 60, 40);

    /* Rectangle outline */
    OLED_DrawRectangle(10, 25, 30, 20, OLED_UNFILLED);

    /* Rectangle filled */
    OLED_DrawRectangle(50, 25, 25, 15, OLED_FILLED);

    /* Circle outline */
    OLED_DrawCircle(85, 50, 12, OLED_UNFILLED);

    /* Circle filled */
    OLED_DrawCircle(115, 40, 8, OLED_FILLED);

    /* Triangle outline */
    OLED_DrawTriangle(70, 20, 90, 20, 80, 30, OLED_UNFILLED);

    OLED_Update();

    /* Wait ~2 seconds */
    delay_cycles(64000000);

    /* ---- Screen 3: Counting loop ---- */
    while (1)
    {
        OLED_Clear();

        OLED_Printf(0, 0, OLED_8X16, "Count: %u", cnt);

        OLED_ShowString(0, 18, "Signed:", OLED_6X8);
        OLED_ShowSignedNum(48, 18, (int32_t)cnt - 100, 4, OLED_6X8);

        OLED_ShowString(0, 28, "Hex:", OLED_6X8);
        OLED_ShowHexNum(36, 28, cnt, 4, OLED_6X8);

        OLED_ShowString(0, 38, "Bin:", OLED_6X8);
        OLED_ShowBinNum(36, 38, cnt, 8, OLED_6X8);

        /* Animated bar */
        {
            uint8_t bar_width = cnt % 110;
            OLED_DrawRectangle(5, 50, bar_width, 8, OLED_FILLED);
        }

        OLED_Update();

        cnt++;

        DL_GPIO_togglePins(LED_PORT, LED_PIN_A14_PIN);
        delay_cycles(16000000);  /* ~0.5s */
    }
}
