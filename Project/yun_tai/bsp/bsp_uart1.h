/**
 * @file  bsp_uart1.h
 * @brief UART1 接收 — 环形缓冲 + 坐标帧解析
 *
 * 协议: "X:+0123,Y:+0045\r\n" 或 "X:LOST,Y:LOST\r\n"
 * 波特率: 115200, 每帧 ≤32 字节
 */

#ifndef BSP_UART1_H
#define BSP_UART1_H

#include "ti_msp_dl_config.h"
#include <stdbool.h>
#include <stdint.h>

/* ---- 数据类型 ---- */

typedef struct {
    bool    valid;      /* true=有效坐标, false=目标丢失 */
    int16_t x;          /* 水平像素偏移 */
    int16_t y;          /* 垂直像素偏移 */
} track_coord_t;

/* ---- API ---- */

void UART1_Init(void);
bool UART1_Parse(track_coord_t *coord);

#endif /* BSP_UART1_H */
