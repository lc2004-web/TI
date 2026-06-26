/**
 * @file  bsp_siic.h
 * @brief 硬件 I2C 封装 — 适配 inv_mpu 库所需的软件 I2C 接口
 *        实际使用 DriverLib DL_I2C (I2C0: PA0=SDA, PA1=SCL)
 */
#ifndef BSP_SIIC_H
#define BSP_SIIC_H

#include "ti_msp_dl_config.h"

typedef enum {
    IIC_OK       = 0x00U,
    IIC_ERR      = 0x01U,
    IIC_BUSY     = 0x02U,
    IIC_TIMEOUT  = 0x03U,
    I2C_BUS_ERROR,
    I2C_ARBITRATION_LOST,
    I2C_ADDR_NACK
} IIC_Status_t;

typedef struct {
    void (*init)(void);
    IIC_Status_t (*write)(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    IIC_Status_t (*read)(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    IIC_Status_t (*write_reg)(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    IIC_Status_t (*read_reg)(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    void (*delay_ms)(uint16_t ms);
} IICInterface_t, *pIICInterface_t;

extern IICInterface_t User_sIICDev;

/* ---- 公开的 I2C 探测 API (直接使用硬件 I2C) ---- */

/**
 * @brief  读取 WHO_AM_I 寄存器, 检测 I2C 设备是否存在
 * @param  addr  7-bit I2C 地址
 * @param  reg   要读取的寄存器地址
 * @param  data  输出: 读取到的字节
 * @retval  0  成功
 * @retval -1  通信失败 (设备不存在或无应答)
 */
int BSP_I2C_ReadReg(uint8_t addr, uint8_t reg, uint8_t *data);

/**
 * @brief  简单写一个字节到 I2C 设备寄存器
 * @param  addr  7-bit I2C 地址
 * @param  reg   寄存器地址
 * @param  data  要写入的字节
 * @retval  0  成功
 * @retval -1  通信失败
 */
int BSP_I2C_WriteReg(uint8_t addr, uint8_t reg, uint8_t data);

extern int g_i2c_err_line;  /* 调试: hw_i2c 失败行号 */

#endif
