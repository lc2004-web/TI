/**
 * @file  bsp_siic.c
 * @brief 硬件 I2C 实现, 实现 IICInterface_t 接口
 */

#include "bsp_siic.h"

/* ---- 硬件 I2C 读写 (已验证) ---- */

int g_i2c_err_line = 0;  /* 调试: hw_i2c_write 失败行号 */

static int hw_i2c_read(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    volatile uint32_t timeout;

    g_i2c_err_line = __LINE__ + 1;
    timeout = 100000;
    while (!(DL_I2C_getControllerStatus(MPU6050_INST) & DL_I2C_CONTROLLER_STATUS_IDLE))
        if (--timeout == 0) { g_i2c_err_line = __LINE__; return -1; }

    DL_I2C_transmitControllerData(MPU6050_INST, reg);
    DL_I2C_startControllerTransfer(MPU6050_INST, addr,
        DL_I2C_CONTROLLER_DIRECTION_TX, 1);

    g_i2c_err_line = __LINE__ + 1;
    timeout = 100000;
    while (DL_I2C_getControllerStatus(MPU6050_INST) & DL_I2C_CONTROLLER_STATUS_BUSY)
        if (--timeout == 0) { g_i2c_err_line = __LINE__; return -2; }
    timeout = 100000;
    while (!(DL_I2C_getControllerStatus(MPU6050_INST) & DL_I2C_CONTROLLER_STATUS_IDLE))
        if (--timeout == 0) { g_i2c_err_line = __LINE__; return -3; }

    DL_I2C_startControllerTransfer(MPU6050_INST, addr,
        DL_I2C_CONTROLLER_DIRECTION_RX, len);

    for (uint16_t i = 0; i < len; i++)
    {
        g_i2c_err_line = __LINE__ + 1;
        timeout = 100000;
        while (DL_I2C_isControllerRXFIFOEmpty(MPU6050_INST))
        {
            if (DL_I2C_getControllerStatus(MPU6050_INST) & DL_I2C_CONTROLLER_STATUS_ERROR)
                { g_i2c_err_line = __LINE__; return -4; }
            if (--timeout == 0) { g_i2c_err_line = __LINE__; return -5; }
        }
        buf[i] = DL_I2C_receiveControllerData(MPU6050_INST);
    }

    g_i2c_err_line = __LINE__ + 1;
    timeout = 100000;
    while (DL_I2C_getControllerStatus(MPU6050_INST) & DL_I2C_CONTROLLER_STATUS_BUSY)
        if (--timeout == 0) { g_i2c_err_line = __LINE__; return -6; }
    timeout = 100000;
    while (!(DL_I2C_getControllerStatus(MPU6050_INST) & DL_I2C_CONTROLLER_STATUS_IDLE))
        if (--timeout == 0) { g_i2c_err_line = __LINE__; return -7; }

    return 0;
}

static int hw_i2c_write(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    volatile uint32_t timeout;

    g_i2c_err_line = __LINE__ + 1;
    timeout = 100000;
    while (!(DL_I2C_getControllerStatus(MPU6050_INST) & DL_I2C_CONTROLLER_STATUS_IDLE))
        if (--timeout == 0) { g_i2c_err_line = __LINE__; return -1; }

    DL_I2C_transmitControllerData(MPU6050_INST, reg);
    DL_I2C_startControllerTransfer(MPU6050_INST, addr,
        DL_I2C_CONTROLLER_DIRECTION_TX, len + 1);

    for (uint16_t i = 0; i < len; i++)
    {
        g_i2c_err_line = __LINE__ + 1;
        timeout = 100000;
        while (DL_I2C_isControllerTXFIFOFull(MPU6050_INST))
            if (--timeout == 0) { g_i2c_err_line = __LINE__; return -2; }
        DL_I2C_transmitControllerData(MPU6050_INST, buf[i]);
    }

    g_i2c_err_line = __LINE__ + 1;
    timeout = 100000;
    while (DL_I2C_getControllerStatus(MPU6050_INST) & DL_I2C_CONTROLLER_STATUS_BUSY) {
        if (DL_I2C_getControllerStatus(MPU6050_INST) & DL_I2C_CONTROLLER_STATUS_ERROR)
            { g_i2c_err_line = __LINE__; return -8; }  /* NACK */
        if (--timeout == 0) { g_i2c_err_line = __LINE__; return -3; }
    }
    timeout = 100000;
    while (!(DL_I2C_getControllerStatus(MPU6050_INST) & DL_I2C_CONTROLLER_STATUS_IDLE)) {
        if (DL_I2C_getControllerStatus(MPU6050_INST) & DL_I2C_CONTROLLER_STATUS_ERROR)
            { g_i2c_err_line = __LINE__; return -8; }  /* NACK */
        if (--timeout == 0) { g_i2c_err_line = __LINE__; return -4; }
    }

    return 0;
}

/* ---- 接口适配: inv_mpu 期望 addr << 1 格式 ---- */

static IIC_Status_t siic_read_reg(uint16_t DevAddress, uint16_t MemAddress,
                                   uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    (void)Timeout;
    if (hw_i2c_read((uint8_t)(DevAddress >> 1), (uint8_t)MemAddress, Size, pData) == 0)
        return IIC_OK;
    return IIC_ERR;
}

static IIC_Status_t siic_write_reg(uint16_t DevAddress, uint16_t MemAddress,
                                    uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    (void)Timeout;
    if (hw_i2c_write((uint8_t)(DevAddress >> 1), (uint8_t)MemAddress, Size, pData) == 0)
        return IIC_OK;
    return IIC_ERR;
}

static IIC_Status_t siic_read(uint16_t DevAddress, uint8_t *pData,
                               uint16_t Size, uint32_t Timeout)
{
    (void)Timeout;
    /* 无寄存器地址的读 — inv_mpu 不使用, 占位 */
    (void)DevAddress;
    (void)pData;
    (void)Size;
    return IIC_ERR;
}

static IIC_Status_t siic_write(uint16_t DevAddress, uint8_t *pData,
                                uint16_t Size, uint32_t Timeout)
{
    (void)Timeout;
    (void)DevAddress;
    (void)pData;
    (void)Size;
    return IIC_ERR;
}

static void siic_delay_ms(uint16_t ms)
{
    delay_cycles((uint32_t)ms * (CPUCLK_FREQ / 1000));
}

static void siic_init(void)
{
    /* I2C 已由 SYSCFG_DL_init() 初始化 */
}

/* ---- 公开 API: 直接读写 I2C 寄存器 ---- */

int BSP_I2C_ReadReg(uint8_t addr, uint8_t reg, uint8_t *data)
{
    return hw_i2c_read(addr, reg, 1, data);
}

int BSP_I2C_WriteReg(uint8_t addr, uint8_t reg, uint8_t data)
{
    return hw_i2c_write(addr, reg, 1, &data);
}

/* ---- 全局实例 ---- */
IICInterface_t User_sIICDev = {
    .init      = siic_init,
    .write     = siic_write,
    .read      = siic_read,
    .write_reg = siic_write_reg,
    .read_reg  = siic_read_reg,
    .delay_ms  = siic_delay_ms,
};

