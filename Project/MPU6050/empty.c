/*
 * 舵机驱动测试 — PA8 输出 50Hz PWM
 * 舵机在 0° ~ 180° 之间往复扫描
 */
#include "ti_msp_dl_config.h"
#include "BSP/bsp_servo.h"

void SysTick_Handler(void) {}

int main(void)
{
    uint8_t  angle = 0;
    int8_t   dir   = 1;   /* 1=正转, -1=反转 */

    SYSCFG_DL_init();
    SERVO_Init();

    /* 主循环: 舵机 0° ↔ 180° 往复扫描 */
    while (1)
    {
        SERVO_SetAngle(angle);

        angle += (uint8_t)(dir * 2);  /* 每次步进 2° */

        if (angle >= 180) { angle = 180; dir = -1; }
        if (angle == 0)   { dir = 1; }

        DL_GPIO_togglePins(LED_PORT, LED_LED0_PIN);
        delay_cycles(CPUCLK_FREQ / 100); /* ~10ms */
    }
}
