/*
 * bsp_delay.c
 *
 * Millisecond delay implementation using DL_Common_delayCycles().
 * CPUCLK_FREQ = 32000000 (32 MHz SYSOSC), so 1 ms = 32000 cycles.
 */

#include "bsp_delay.h"
#include "ti_msp_dl_config.h"

#define CYCLES_PER_MS  (CPUCLK_FREQ / 1000)  /* 32000 */

void bsp_delay_ms(uint32_t ms)
{
    uint32_t i;
    for (i = 0; i < ms; i++) {
        delay_cycles(CYCLES_PER_MS);
    }
}
