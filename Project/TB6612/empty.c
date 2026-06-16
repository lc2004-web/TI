#include "ti_msp_dl_config.h"
#include "BSP/bsp_uart.h"
#include "BSP/bsp_delay.h"
#include "BSP/bsp_tb6612.h"

static void motor_a_test(void)
{
    /* ---- Forward ---- */
    bsp_uart_puts("Motor A: CW (Forward) 50%\r\n");
    tb6612_set_direction(MOTOR_A, MOTOR_CW);
    tb6612_set_duty(MOTOR_A, 2000);
    bsp_delay_ms(2000);

    /* ---- Stop ---- */
    bsp_uart_puts("Motor A: Stop\r\n");
    tb6612_set_duty(MOTOR_A, 0);
    tb6612_set_direction(MOTOR_A, MOTOR_STOP);
    bsp_delay_ms(1000);

    /* ---- Reverse ---- */
    bsp_uart_puts("Motor A: CCW (Reverse) 50%\r\n");
    tb6612_set_direction(MOTOR_A, MOTOR_CCW);
    tb6612_set_duty(MOTOR_A, 2000);
    bsp_delay_ms(2000);

    /* ---- Stop ---- */
    bsp_uart_puts("Motor A: Stop\r\n");
    tb6612_set_duty(MOTOR_A, 0);
    tb6612_set_direction(MOTOR_A, MOTOR_STOP);
    bsp_delay_ms(1000);
}

static void motor_b_test(void)
{
    /* ---- Forward ---- */
    bsp_uart_puts("Motor B: CW (Forward) 50%\r\n");
    tb6612_set_direction(MOTOR_B, MOTOR_CW);
    tb6612_set_duty(MOTOR_B, 2000);
    bsp_delay_ms(2000);

    /* ---- Stop ---- */
    bsp_uart_puts("Motor B: Stop\r\n");
    tb6612_set_duty(MOTOR_B, 0);
    tb6612_set_direction(MOTOR_B, MOTOR_STOP);
    bsp_delay_ms(1000);

    /* ---- Reverse ---- */
    bsp_uart_puts("Motor B: CCW (Reverse) 50%\r\n");
    tb6612_set_direction(MOTOR_B, MOTOR_CCW);
    tb6612_set_duty(MOTOR_B, 2000);
    bsp_delay_ms(2000);

    /* ---- Stop ---- */
    bsp_uart_puts("Motor B: Stop\r\n");
    tb6612_set_duty(MOTOR_B, 0);
    tb6612_set_direction(MOTOR_B, MOTOR_STOP);
    bsp_delay_ms(1000);
}

int main(void)
{
    SYSCFG_DL_init();
    bsp_delay_ms(100);

    bsp_uart_puts("\r\n=== TB6612 Dual Motor Test ===\r\n");

    /* Initialize both motors */
    tb6612_init(MOTOR_A);
    tb6612_init(MOTOR_B);
    bsp_uart_puts("Motor A & B initialized.\r\n");

    while (1) {
        motor_a_test();
        motor_b_test();
    }
}
