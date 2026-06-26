/*
 * bsp_uart.h
 *
 * UART Debug Print BSP for TB6612 Motor Driver Project
 * PB6 - UART1 TX
 * PB7 - UART1 RX
 * Baud Rate: 115200
 */

#ifndef BSP_UART_H_
#define BSP_UART_H_

#include <stdint.h>
#include <stdarg.h>

/**
 * @brief  Send a single byte via UART (blocking)
 * @param  data  Byte to send
 */
void bsp_uart_send_byte(uint8_t data);

/**
 * @brief  Send a null-terminated string via UART (blocking)
 * @param  str  Pointer to string
 */
void bsp_uart_puts(const char *str);

/**
 * @brief  Lightweight formatted debug print via UART
 *
 * Supports format specifiers:
 *   %s - string
 *   %c - character
 *   %d - signed decimal integer
 *   %u - unsigned decimal integer
 *   %x - hex lowercase
 *   %X - hex uppercase
 *   %% - percent sign
 *
 * @param  fmt  Format string
 * @param  ...  Variable arguments
 */
void bsp_uart_printf(const char *fmt, ...);

#endif /* BSP_UART_H_ */
