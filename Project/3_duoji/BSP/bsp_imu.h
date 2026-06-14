/**
 * @file  bsp_imu.h
 * @brief MPU6500 DMP 姿态解算模块 — 封装 MPU 初始化、DMP FIFO 读取、四元数→欧拉角
 *
 * 使用 InvenSense DMP 驱动库 (inv_mpu + inv_mpu_dmp_motion_driver)
 * 硬件: MPU6500 (WHO_AM_I=0x70), I2C0: PA0=SDA, PA1=SCL
 */

#ifndef BSP_IMU_H
#define BSP_IMU_H

#include "ti_msp_dl_config.h"

/* ---- 姿态角数据结构 ---- */
typedef struct {
    float pitch;  /* 俯仰角 (°) */
    float roll;   /* 横滚角 (°) */
    float yaw;    /* 偏航角 (°) */
} IMU_Attitude_t;

/* ---- API ---- */

/**
 * @brief  初始化 MPU6500 + DMP
 * @retval  0  成功
 * @retval -1  失败 (mpu_init 或 DMP 固件加载或 DMP 启动失败)
 */
int IMU_Init(void);

/**
 * @brief  读取 DMP FIFO 并解算姿态角
 * @param  att  输出: 姿态角数据指针
 * @retval  0  读取到新的姿态数据
 * @retval -1  FIFO 中无有效四元数数据
 */
int IMU_Read(IMU_Attitude_t *att);

#endif /* BSP_IMU_H */
