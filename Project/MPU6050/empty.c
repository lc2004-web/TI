/*
 * MPU6500 DMP 姿态解算 — OLED 显示 Pitch/Roll/Yaw
 * 芯片: MPU6500 (WHO_AM_I=0x70), I2C0: PA0=SDA, PA1=SCL
 */
#include "ti_msp_dl_config.h"
#include "BSP/bsp_oled.h"
#include "bsp_siic.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include <string.h>
#include <stdio.h>

#define q30 1073741824.0f

static signed char gyro_orientation[9] = {1, 0, 0,
                                          0, 1, 0,
                                          0, 0, 1};

unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;
    if (row[0] > 0) b = 0;
    else if (row[0] < 0) b = 4;
    else if (row[1] > 0) b = 1;
    else if (row[1] < 0) b = 5;
    else if (row[2] > 0) b = 2;
    else if (row[2] < 0) b = 6;
    else b = 7;
    return b;
}

unsigned short inv_orientation_matrix_to_scalar(const signed char *mtx)
{
    unsigned short scalar;
    scalar  = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;
    return scalar;
}

void SysTick_Handler(void) {}

int main(void)
{
    float pitch = 0, roll = 0, yaw = 0;
    int dmp_ok = 0;
    char str[32];

    SYSCFG_DL_init();
    DL_I2C_setTimerPeriod(MPU6050_INST, 3);  /* 400kHz */
    OLED_Init();
    OLED_Clear();

    OLED_ShowString(0, 0, (const uint8_t *)"MPU6500 DMP Init");
    OLED_Refresh_Gram();

    /* ---- MPU + DMP 初始化 ---- */
    if (mpu_init() == 0)
    {
        mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
        mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
        mpu_set_sample_rate(100);

        if (dmp_load_motion_driver_firmware() == 0)
        {
            dmp_set_orientation(
                inv_orientation_matrix_to_scalar(gyro_orientation));
            dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
                               DMP_FEATURE_ANDROID_ORIENT |
                               DMP_FEATURE_SEND_RAW_ACCEL |
                               DMP_FEATURE_SEND_CAL_GYRO |
                               DMP_FEATURE_GYRO_CAL);
            dmp_set_fifo_rate(100);

            if (mpu_set_dmp_state(1) == 0)
                dmp_ok = 1;
        }
    }

    if (!dmp_ok)
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
        short gyro[3], accel[3], sensors;
        unsigned char more;
        long quat[4];

        if (dmp_read_fifo(gyro, accel, quat, NULL, &sensors, &more) == 0)
        {
            if (sensors & INV_WXYZ_QUAT)
            {
                float q0 = quat[0] / q30;
                float q1 = quat[1] / q30;
                float q2 = quat[2] / q30;
                float q3 = quat[3] / q30;

                pitch =  asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3f;
                roll  =  atan2(2 * q2 * q3 + 2 * q0 * q1,
                              -2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.3f;
                yaw   =  atan2(2 * (q1 * q2 + q0 * q3),
                               q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * 57.3f;
            }
        }

        memset(OLED_GRAM, 0, 128 * 8);
        sprintf(str, "P: %6.2f", pitch);
        OLED_ShowString(0, 0, (const uint8_t *)str);
        sprintf(str, "R: %6.2f", roll);
        OLED_ShowString(0, 16, (const uint8_t *)str);
        sprintf(str, "Y: %6.2f", yaw);
        OLED_ShowString(0, 32, (const uint8_t *)str);
        OLED_Refresh_Gram();

        DL_GPIO_togglePins(LED_PORT, LED_LED0_PIN);
        delay_cycles(CPUCLK_FREQ / 100); /* ~10ms */
    }
}
