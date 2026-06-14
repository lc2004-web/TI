/*
 * MPU6500 DMP 姿态解算 — OLED 显示 Pitch/Roll/Yaw
 * 芯片: MPU6500 (WHO_AM_I=0x70), I2C0: PA0=SDA, PA1=SCL
 *
 * 姿态解算逻辑已封装到 BSP/bsp_imu.c
 */
#include "ti_msp_dl_config.h"
#include "BSP/bsp_imu.h"
#include "BSP/bsp_oled.h"
#include <string.h>
#include <stdio.h>

void SysTick_Handler(void) {}

int main(void)
{
    IMU_Attitude_t att;
    char str[32];

    SYSCFG_DL_init();
    OLED_Init();
    OLED_Clear();

    OLED_ShowString(0, 0, (const uint8_t *)"MPU6500 DMP Init");
    OLED_Refresh_Gram();

    /* ---- IMU 初始化 ---- */
    if (IMU_Init() != 0)
    {
        OLED_Clear();
        OLED_ShowString(0, 0, (const uint8_t *)"DMP Init FAIL");
        OLED_Refresh_Gram();
        while (1)
        {
            DL_GPIO_togglePins(LED_PORT, LED_LED0_PIN);
            delay_cycles(CPUCLK_FREQ / 20); /* 快闪 */
        }
    }

    OLED_Clear();
    OLED_ShowString(0, 0, (const uint8_t *)"DMP OK");
    OLED_Refresh_Gram();
    delay_cycles(CPUCLK_FREQ / 2); /* 500ms */

    /* ---- 主循环 ---- */
    while (1)
    {
        /* 读取姿态 */
        if (IMU_Read(&att) == 0)
        {
            memset(OLED_GRAM, 0, 128 * 8);
            sprintf(str, "P: %6.2f", att.pitch);
            OLED_ShowString(0, 0, (const uint8_t *)str);
            sprintf(str, "R: %6.2f", att.roll);
            OLED_ShowString(0, 16, (const uint8_t *)str);
            sprintf(str, "Y: %6.2f", att.yaw);
            OLED_ShowString(0, 32, (const uint8_t *)str);
            OLED_Refresh_Gram();
        }

        DL_GPIO_togglePins(LED_PORT, LED_LED0_PIN);
        delay_cycles(CPUCLK_FREQ / 100); /* ~10ms */
    }
}
