/**
 * @file  bsp_uart1.c
 * @brief UART1 (PB6/PB7) 中断接收 + 环形缓冲 + ASCII 帧解析
 *
 * 协议格式:
 *   有效: "X:+0123,Y:+0045\r\n"
 *   丢失: "X:LOST,Y:LOST\r\n"
 *
 * 帧长: ≤32 字节, 以 '\n' 断帧
 */

#include "bsp_uart1.h"
#include <string.h>

/* ---- 环形缓冲 ---- */
#define RX_BUF_SIZE     128

static volatile uint8_t  rx_buf[RX_BUF_SIZE];
static volatile uint8_t  rx_wr;
static volatile uint8_t  rx_rd;

/* ---- 帧缓冲 ---- */
#define FRAME_MAX        32
static char frame_buf[FRAME_MAX];
static uint8_t frame_len;

/* ---- 工具: 字符串转有符号整数 ---- */
static int16_t atoi16(const char *s)
{
    int16_t val = 0;
    int8_t  sign = 1;

    if (*s == '-') { sign = -1; s++; }
    else if (*s == '+') { s++; }

    while (*s >= '0' && *s <= '9') {
        val = val * 10 + (*s - '0');
        s++;
    }
    return val * sign;
}

/* ================================================================
 *  ISR
 * ================================================================ */

void UART_1_INST_IRQHandler(void)
{
    if (DL_UART_getPendingInterrupt(UART_1_INST) == DL_UART_IIDX_RX) {
        rx_buf[rx_wr] = (uint8_t)DL_UART_Main_receiveData(UART_1_INST);
        rx_wr++;
        if (rx_wr >= RX_BUF_SIZE) rx_wr = 0;
    }
    DL_UART_clearInterruptStatus(UART_1_INST, DL_UART_IIDX_RX);
}

/* ================================================================
 *  API
 * ================================================================ */

void UART1_Init(void)
{
    rx_wr = 0;
    rx_rd = 0;
    frame_len = 0;
}

bool UART1_Parse(track_coord_t *coord)
{
    char c;

    while (rx_rd != rx_wr) {
        c = (char)rx_buf[rx_rd];
        rx_rd++;
        if (rx_rd >= RX_BUF_SIZE) rx_rd = 0;

        if (c == '\r') continue;

        if (c == '\n') {
            frame_buf[frame_len] = '\0';
            frame_len = 0;

            /* "X:LOST,Y:LOST" */
            if (strncmp(frame_buf, "X:LOST", 6) == 0) {
                coord->valid = false;
                coord->x = 0;
                coord->y = 0;
                return true;
            }

            /* "X:+0123,Y:+0045" → 找到 X: 和 Y: */
            char *px = strchr(frame_buf, 'X');
            char *py = strchr(frame_buf, 'Y');
            if (px && py && px[1] == ':') {
                coord->x = atoi16(px + 2);
                coord->y = atoi16(strchr(py, ':') + 1);
                coord->valid = true;
                return true;
            }

            /* 解析失败 */
            coord->valid = false;
            coord->x = 0;
            coord->y = 0;
            return true;
        }

        if (frame_len < FRAME_MAX - 1) {
            frame_buf[frame_len++] = c;
        }
    }

    return false;
}
