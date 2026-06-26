/*
 * bsp_uart.c
 *
 * UART Debug Print BSP Implementation
 * TX: PB6 (UART1), RX: PB7 (UART1), Baud: 115200
 *
 * Uses SysConfig-generated UART_DEBUG_INST handle.
 * UART initialization is done by SYSCFG_DL_init() —
 * do not call any manual init here.
 */

#include "bsp_uart.h"
#include "ti_msp_dl_config.h"

/* ------------------------------------------------------------------ */
/* Low-level byte send                                                */
/* ------------------------------------------------------------------ */
void bsp_uart_send_byte(uint8_t data)
{
    DL_UART_Main_transmitDataBlocking(UART_DEBUG_INST, data);
}

/* ------------------------------------------------------------------ */
/* Simple string send                                                 */
/* ------------------------------------------------------------------ */
void bsp_uart_puts(const char *str)
{
    while (*str) {
        bsp_uart_send_byte((uint8_t)*str++);
    }

    /* Wait for all data to finish transmitting */
    while (DL_UART_Main_isBusy(UART_DEBUG_INST)) {}
}

/* ------------------------------------------------------------------ */
/* Lightweight formatted print                                        */
/* ------------------------------------------------------------------ */
static void uart_print_uint(uint32_t val, uint8_t base, uint8_t uppercase)
{
    char buf[12];
    uint8_t i = sizeof(buf);
    const char hex_lower[] = "0123456789abcdef";
    const char hex_upper[] = "0123456789ABCDEF";
    const char *digits = uppercase ? hex_upper : hex_lower;

    if (val == 0) {
        bsp_uart_send_byte('0');
        return;
    }

    buf[--i] = '\0';
    while (val > 0) {
        buf[--i] = digits[val % base];
        val /= base;
    }
    bsp_uart_puts(&buf[i]);
}

void bsp_uart_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt != '%') {
            bsp_uart_send_byte((uint8_t)*fmt++);
            continue;
        }

        fmt++; /* skip '%' */
        switch (*fmt) {
        case 's': {
            const char *s = va_arg(args, const char *);
            bsp_uart_puts(s ? s : "(null)");
            break;
        }
        case 'c': {
            char c = (char)va_arg(args, int);
            bsp_uart_send_byte((uint8_t)c);
            break;
        }
        case 'd': {
            int32_t val = va_arg(args, int32_t);
            if (val < 0) {
                bsp_uart_send_byte('-');
                val = -val;
            }
            uart_print_uint((uint32_t)val, 10, 0);
            break;
        }
        case 'u':
            uart_print_uint(va_arg(args, uint32_t), 10, 0);
            break;
        case 'x':
            uart_print_uint(va_arg(args, uint32_t), 16, 0);
            break;
        case 'X':
            uart_print_uint(va_arg(args, uint32_t), 16, 1);
            break;
        case '%':
            bsp_uart_send_byte('%');
            break;
        default:
            bsp_uart_send_byte('%');
            bsp_uart_send_byte((uint8_t)*fmt);
            break;
        }
        fmt++;
    }

    va_end(args);

    /* Wait for all data to finish transmitting */
    while (DL_UART_Main_isBusy(UART_DEBUG_INST)) {}
}
