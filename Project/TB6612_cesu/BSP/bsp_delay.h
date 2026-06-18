/*
 * bsp_delay.h
 *
 * Millisecond delay using CPU cycle counter
 * CPUCLK = 32 MHz (SYSOSC)
 */

#ifndef BSP_DELAY_H_
#define BSP_DELAY_H_

#include <stdint.h>

/**
 * @brief  Blocking millisecond delay
 * @param  ms  Milliseconds to delay (1 ~ 1000)
 */
void bsp_delay_ms(uint32_t ms);

#endif /* BSP_DELAY_H_ */
