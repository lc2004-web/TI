/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "bsp/Emm_V5.h"

/**
 * @brief   Emm_V5.0 Stepper Motor Library — Function Demo
 *          Each stage runs briefly so you can observe the motor's response.
 */
int main(void)
{
    SYSCFG_DL_init();
    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
    delay_ms(500);  // wait for driver power-up

    while(1)
    {
        //=== 1. Velocity Mode: CW 60 RPM, 1.5s ===//
        Emm_V5_Vel_Control(1, 0, 60, 20, 0);
        delay_ms(1500);

        //=== 2. Stop ===//
        Emm_V5_Stop_Now(1, 0);
        delay_ms(500);

        //=== 3. Velocity Mode: CCW 250 RPM, 1.5s ===//
        Emm_V5_Vel_Control(1, 1, 250, 60, 0);
        delay_ms(1500);

        //=== 4. Stop ===//
        Emm_V5_Stop_Now(1, 0);
        delay_ms(500);

        //=== 5. Quick Position: +3200 pulses (1 rev), 500 RPM ===//
        Emm_V5_Set_QPos_Params(1, 500, 40, 1, 0);
        delay_ms(10);
        Emm_V5_QPos_Control(1, 3200);
        delay_ms(1500);

        //=== 6. Quick Position: -3200 pulses (back) ===//
        Emm_V5_QPos_Control(1, -3200);
        delay_ms(1500);

        //=== 7. Full Position: CW 6400 pulses (2 rev), 400 RPM ===//
        Emm_V5_Pos_Control(1, 0, 400, 50, 6400, 1, 0);
        delay_ms(2000);

        //=== 8. Full Position: CCW 6400 pulses ===//
        Emm_V5_Pos_Control(1, 1, 400, 50, 6400, 1, 0);
        delay_ms(2000);

        //=== 9. Pause, then repeat ===//
        delay_ms(1000);
    }
}
