/*
 * MPU6500 DMP 姿态解算 — OLED 显示 Pitch/Roll/Yaw
 * 芯片: MPU6500 (WHO_AM_I=0x70, I2C Addr=0x68)
 * I2C0: PA0=SDA, PA1=SCL
 *
 * 姿态解算逻辑已封装到 BSP/bsp_imu.c
 */
#include "ti_msp_dl_config.h"
#include "BSP/bsp_imu.h"
#include "BSP/bsp_oled.h"
#include "BSP/inv_mpu.h"
#include "BSP/bsp_siic.h"
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
    int imu_ret = IMU_Init();
    if (imu_ret != 0)
    {
        OLED_Clear();
        sprintf(str, "DMP Fail(%d)", imu_ret);
        OLED_ShowString(0, 0, (const uint8_t *)str);
        /* 解释错误码 */
        if (imu_ret == -11)
            OLED_ShowString(0, 16, (const uint8_t *)"WHO_AM_I rd fail");
        else if (imu_ret == -12)
            OLED_ShowString(0, 16, (const uint8_t *)"WHO_AM_I mismatch");
        else if (imu_ret == -21)
            OLED_ShowString(0, 16, (const uint8_t *)"Reset wr fail");
        else if (imu_ret == -22)
            OLED_ShowString(0, 16, (const uint8_t *)"Wake wr fail");
        else if (imu_ret == -23)
            OLED_ShowString(0, 16, (const uint8_t *)"Post-rst rd fail");
        else if (imu_ret == -24)
            OLED_ShowString(0, 16, (const uint8_t *)"Post-rst mismatch");
        else if (imu_ret == -1) {
            sprintf(str, "mpu@%d i2c@%d", mpu_init_step, g_i2c_err_line);
            OLED_ShowString(0, 16, (const uint8_t *)str);
        }
        else if (imu_ret == -2)
            OLED_ShowString(0, 16, (const uint8_t *)"DMP fw load err");
        else if (imu_ret == -3)
            OLED_ShowString(0, 16, (const uint8_t *)"DMP start err");
        else
            OLED_ShowString(0, 16, (const uint8_t *)"Unknown err");
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

            if (!IMU_IsCalibrated())
            {
                /* 等待陀螺校准完成 — 请保持静止 */
                OLED_ShowString(0, 0, (const uint8_t *)"Gyro Calibrating");
                OLED_ShowString(0, 16, (const uint8_t *)"Keep Still...");
                int cnt = IMU_GetSettleCnt();
                sprintf(str, "Wait ~%ds", 8 - cnt / 100);
                OLED_ShowString(0, 32, (const uint8_t *)str);
                /* 进度条 */
                int dots = cnt * 8 / YAW_SETTLE_TARGET;
                for (int d = 0; d < dots; d++) str[d] = '#';
                str[dots] = '\0';
                OLED_ShowString(0, 48, (const uint8_t *)str);
            }
            else
            {
                /* 校准完成 — 正常显示姿态 */
                sprintf(str, "P: %6.2f", att.pitch);
                OLED_ShowString(0, 0, (const uint8_t *)str);
                sprintf(str, "R: %6.2f", att.roll);
                OLED_ShowString(0, 16, (const uint8_t *)str);
                sprintf(str, "Y: %6.2f", att.yaw);
                OLED_ShowString(0, 32, (const uint8_t *)str);
            }

            OLED_Refresh_Gram();
        }

        DL_GPIO_togglePins(LED_PORT, LED_LED0_PIN);
        delay_cycles(CPUCLK_FREQ / 100); /* ~10ms */
    }
}
