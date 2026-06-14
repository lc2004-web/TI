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

#endif
