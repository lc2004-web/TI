#include "ti_msp_dl_config.h"
#include "BSP/bsp_uart.h"
#include "BSP/bsp_delay.h"
#include "BSP/bsp_tb6612.h"
#include "BSP/bsp_encoder.h"

/* Motor A test — forward & reverse while printing encoder data */
static void motor_a_test(void)
{
    bsp_uart_puts("\r\n--- Motor A: CW (Forward) 50% ---\r\n");
    tb6612_set_direction(MOTOR_A, MOTOR_CW);
    tb6612_set_duty(MOTOR_A, 2000);

    for (int i = 0; i < 20; i++) {
        bsp_delay_ms(200);
        bsp_uart_printf("  A: cnt=%d  rpm=%d\r\n",
                        encoder_get_count(ENCODER_A),
                        (int)encoder_get_speed_rpm(ENCODER_A));
    }

    bsp_uart_puts("--- Motor A: Stop ---\r\n");
    tb6612_set_duty(MOTOR_A, 0);
    tb6612_set_direction(MOTOR_A, MOTOR_STOP);
    bsp_delay_ms(500);

    bsp_uart_puts("--- Motor A: CCW (Reverse) 50% ---\r\n");
    tb6612_set_direction(MOTOR_A, MOTOR_CCW);
    tb6612_set_duty(MOTOR_A, 2000);

    for (int i = 0; i < 20; i++) {
        bsp_delay_ms(200);
        bsp_uart_printf("  A: cnt=%d  rpm=%d\r\n",
                        encoder_get_count(ENCODER_A),
                        (int)encoder_get_speed_rpm(ENCODER_A));
    }

    bsp_uart_puts("--- Motor A: Stop ---\r\n");
    tb6612_set_duty(MOTOR_A, 0);
    tb6612_set_direction(MOTOR_A, MOTOR_STOP);
    bsp_delay_ms(500);
}

/* Motor B test — forward & reverse while printing encoder data */
static void motor_b_test(void)
{
    bsp_uart_puts("\r\n--- Motor B: CW (Forward) 50% ---\r\n");
    tb6612_set_direction(MOTOR_B, MOTOR_CW);
    tb6612_set_duty(MOTOR_B, 2000);

    for (int i = 0; i < 20; i++) {
        bsp_delay_ms(200);
        bsp_uart_printf("  B: cnt=%d  rpm=%d\r\n",
                        encoder_get_count(ENCODER_B),
                        (int)encoder_get_speed_rpm(ENCODER_B));
    }

    bsp_uart_puts("--- Motor B: Stop ---\r\n");
    tb6612_set_duty(MOTOR_B, 0);
    tb6612_set_direction(MOTOR_B, MOTOR_STOP);
    bsp_delay_ms(500);

    bsp_uart_puts("--- Motor B: CCW (Reverse) 50% ---\r\n");
    tb6612_set_direction(MOTOR_B, MOTOR_CCW);
    tb6612_set_duty(MOTOR_B, 2000);

    for (int i = 0; i < 20; i++) {
        bsp_delay_ms(200);
        bsp_uart_printf("  B: cnt=%d  rpm=%d\r\n",
                        encoder_get_count(ENCODER_B),
                        (int)encoder_get_speed_rpm(ENCODER_B));
    }

    bsp_uart_puts("--- Motor B: Stop ---\r\n");
    tb6612_set_duty(MOTOR_B, 0);
    tb6612_set_direction(MOTOR_B, MOTOR_STOP);
    bsp_delay_ms(500);
}

int main(void)
{
    SYSCFG_DL_init();
    bsp_delay_ms(100);

    bsp_uart_puts("\r\n========================================\r\n");
    bsp_uart_puts("  TB6612 Dual Motor + Encoder Test     \r\n");
    bsp_uart_puts("========================================\r\n");

    /* Initialize motor drivers */
    tb6612_init(MOTOR_A);
    tb6612_init(MOTOR_B);
    bsp_uart_puts("TB6612 motors initialized.\r\n");

    /* Initialize encoder speed measurement */
    encoder_init();
    bsp_uart_puts("Encoder speed measurement enabled.\r\n");
    bsp_uart_printf("Encoder PPR: %d (2x res: %d)\r\n\r\n",
                    ENCODER_PPR, ENCODER_2X_RES);

    while (1) {
        motor_a_test();
        motor_b_test();
    }
}
