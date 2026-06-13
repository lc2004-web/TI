/*
 * OLED 递增数字测试
 */

#include "ti_msp_dl_config.h"
#include "BSP/bsp_oled.h"
#include <string.h>

int main(void)
{
    uint32_t cnt = 0;

    SYSCFG_DL_init();
    OLED_Init();

    while (1)
    {
        memset(OLED_GRAM, 0, 128 * 8);
        OLED_ShowString(0, 0, (const uint8_t *)"CNT:");
        OLED_ShowNumber(40, 0, cnt, 8, 12);
        OLED_Refresh_Gram();
        cnt++;

        DL_GPIO_togglePins(LED_PORT, LED_PIN_A14_PIN);
        delay_cycles(32000000);  /* ~1s */
    }
}
