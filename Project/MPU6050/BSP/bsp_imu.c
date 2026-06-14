/**
 * @file  bsp_imu.c
 * @brief MPU6500 DMP 姿态解算 — 封装 DMP 初始化与四元数→欧拉角
 *
 * 依赖: inv_mpu.c / inv_mpu_dmp_motion_driver.c (InvenSense DMP 库)
 *        bsp_siic.c (硬件 I2C 适配层)
 */

#include "bsp_imu.h"
#include "bsp_siic.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include <math.h>

/* ---- 内部常量 ---- */

/* Q30 定点数比例因子 (2^30) */
#define Q30  1073741824.0f

/* 弧度 → 度: 180/π ≈ 57.29578 */
#define RAD2DEG  57.3f

/* ---- 内部变量 ---- */

/* 陀螺仪坐标系 — 单位矩阵 (默认方向) */
static const signed char gyro_orientation[9] = {
     1,  0,  0,
     0,  1,  0,
     0,  0,  1
};

/* DMP 是否已成功初始化 */
static int dmp_ready = 0;

/* ---- 内部函数 ---- */

/**
 * @brief  将方向矩阵的一行编码为 3-bit 标量
 *         来自 InvenSense eMPL 库
 */
static unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;
    if      (row[0] > 0) b = 0;
    else if (row[0] < 0) b = 4;
    else if (row[1] > 0) b = 1;
    else if (row[1] < 0) b = 5;
    else if (row[2] > 0) b = 2;
    else if (row[2] < 0) b = 6;
    else                 b = 7;
    return b;
}

/**
 * @brief  将 3×3 方向矩阵编码为 DMP 所需标量
 */
static unsigned short inv_orientation_matrix_to_scalar(const signed char *mtx)
{
    unsigned short scalar;
    scalar  = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;
    return scalar;
}

/**
 * @brief  四元数 → 欧拉角 (pitch, roll, yaw)
 * @param  q  四元数数组 (Q30 定点: q0=w, q1=x, q2=y, q3=z)
 * @param  att  输出: 欧拉角 (°)
 */
static void quat2euler(const long q[4], IMU_Attitude_t *att)
{
    float q0 = q[0] / Q30;  /* w — 实部 */
    float q1 = q[1] / Q30;  /* x */
    float q2 = q[2] / Q30;  /* y */
    float q3 = q[3] / Q30;  /* z */

    /* Pitch: 绕 X 轴, 范围 [-90°, 90°] */
    att->pitch = asinf(-2.0f * q1 * q3 + 2.0f * q0 * q2) * RAD2DEG;

    /* Roll: 绕 Y 轴, 范围 [-180°, 180°] */
    att->roll  = atan2f(2.0f * q2 * q3 + 2.0f * q0 * q1,
                        -2.0f * q1 * q1 - 2.0f * q2 * q2 + 1.0f) * RAD2DEG;

    /* Yaw: 绕 Z 轴, 范围 [-180°, 180°] */
    att->yaw   = atan2f(2.0f * (q1 * q2 + q0 * q3),
                         q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * RAD2DEG;
}

/* ---- 公开 API ---- */

/**
 * @brief  初始化 MPU6500 硬件 + 加载 DMP 固件
 */
int IMU_Init(void)
{
    int ret;

    /* 1. 配置 I2C 速率 ≈ 400kHz */
    DL_I2C_setTimerPeriod(MPU6050_INST, 3);

    /* 2. 初始化 MPU 驱动层 */
    ret = mpu_init();
    if (ret != 0)
        return -1;

    /* 3. 配置传感器与 FIFO */
    mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    mpu_set_sample_rate(100);

    /* 4. 加载 DMP 固件 */
    ret = dmp_load_motion_driver_firmware();
    if (ret != 0)
        return -1;

    /* 5. 配置 DMP */
    dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation));
    dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
                       DMP_FEATURE_ANDROID_ORIENT |
                       DMP_FEATURE_SEND_RAW_ACCEL |
                       DMP_FEATURE_SEND_CAL_GYRO |
                       DMP_FEATURE_GYRO_CAL);
    dmp_set_fifo_rate(100);

    /* 6. 启动 DMP */
    ret = mpu_set_dmp_state(1);
    if (ret != 0)
        return -1;

    dmp_ready = 1;
    return 0;
}

/**
 * @brief  读取 DMP FIFO 并解算姿态
 *
 * @note   调用频率建议 100Hz (与 DMP FIFO 速率一致)
 *         当 FIFO 为空时返回 -1，调用方应稍后重试
 */
int IMU_Read(IMU_Attitude_t *att)
{
    short gyro[3], accel[3], sensors;
    unsigned char more;
    long quat[4];

    if (!dmp_ready)
        return -1;

    /* 读取 DMP FIFO */
    if (dmp_read_fifo(gyro, accel, quat, NULL, &sensors, &more) != 0)
        return -1;

    /* 检查是否有四元数 */
    if (!(sensors & INV_WXYZ_QUAT))
        return -1;

    /* 四元数 → 欧拉角 */
    quat2euler(quat, att);
    return 0;
}
