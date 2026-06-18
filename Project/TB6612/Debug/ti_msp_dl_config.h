/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
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

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define CPUCLK_FREQ                                                     32000000



/* Defines for PWMA */
#define PWMA_INST                                                          TIMA1
#define PWMA_INST_IRQHandler                                    TIMA1_IRQHandler
#define PWMA_INST_INT_IRQN                                      (TIMA1_INT_IRQn)
#define PWMA_INST_CLK_FREQ                                              32000000
/* GPIO defines for channel 0 */
#define GPIO_PWMA_C0_PORT                                                  GPIOB
#define GPIO_PWMA_C0_PIN                                           DL_GPIO_PIN_2
#define GPIO_PWMA_C0_IOMUX                                       (IOMUX_PINCM15)
#define GPIO_PWMA_C0_IOMUX_FUNC                      IOMUX_PINCM15_PF_TIMA1_CCP0
#define GPIO_PWMA_C0_IDX                                     DL_TIMER_CC_0_INDEX

/* Defines for PWMB */
#define PWMB_INST                                                          TIMA0
#define PWMB_INST_IRQHandler                                    TIMA0_IRQHandler
#define PWMB_INST_INT_IRQN                                      (TIMA0_INT_IRQn)
#define PWMB_INST_CLK_FREQ                                              32000000
/* GPIO defines for channel 0 */
#define GPIO_PWMB_C0_PORT                                                  GPIOA
#define GPIO_PWMB_C0_PIN                                          DL_GPIO_PIN_21
#define GPIO_PWMB_C0_IOMUX                                       (IOMUX_PINCM46)
#define GPIO_PWMB_C0_IOMUX_FUNC                      IOMUX_PINCM46_PF_TIMA0_CCP0
#define GPIO_PWMB_C0_IDX                                     DL_TIMER_CC_0_INDEX



/* Defines for UART_DEBUG */
#define UART_DEBUG_INST                                                    UART1
#define UART_DEBUG_INST_FREQUENCY                                       32000000
#define UART_DEBUG_INST_IRQHandler                              UART1_IRQHandler
#define UART_DEBUG_INST_INT_IRQN                                  UART1_INT_IRQn
#define GPIO_UART_DEBUG_RX_PORT                                            GPIOB
#define GPIO_UART_DEBUG_TX_PORT                                            GPIOB
#define GPIO_UART_DEBUG_RX_PIN                                     DL_GPIO_PIN_7
#define GPIO_UART_DEBUG_TX_PIN                                     DL_GPIO_PIN_6
#define GPIO_UART_DEBUG_IOMUX_RX                                 (IOMUX_PINCM24)
#define GPIO_UART_DEBUG_IOMUX_TX                                 (IOMUX_PINCM23)
#define GPIO_UART_DEBUG_IOMUX_RX_FUNC                  IOMUX_PINCM24_PF_UART1_RX
#define GPIO_UART_DEBUG_IOMUX_TX_FUNC                  IOMUX_PINCM23_PF_UART1_TX
#define UART_DEBUG_BAUD_RATE                                            (115200)
#define UART_DEBUG_IBRD_32_MHZ_115200_BAUD                                  (17)
#define UART_DEBUG_FBRD_32_MHZ_115200_BAUD                                  (23)





/* Port definition for Pin Group DC_MOTOR */
#define DC_MOTOR_PORT                                                    (GPIOA)

/* Defines for AIN1: GPIOA.13 with pinCMx 35 on package pin 28 */
#define DC_MOTOR_AIN1_PIN                                       (DL_GPIO_PIN_13)
#define DC_MOTOR_AIN1_IOMUX                                      (IOMUX_PINCM35)
/* Defines for AIN2: GPIOA.14 with pinCMx 36 on package pin 29 */
#define DC_MOTOR_AIN2_PIN                                       (DL_GPIO_PIN_14)
#define DC_MOTOR_AIN2_IOMUX                                      (IOMUX_PINCM36)
/* Defines for BIN1: GPIOA.16 with pinCMx 38 on package pin 31 */
#define DC_MOTOR_BIN1_PIN                                       (DL_GPIO_PIN_16)
#define DC_MOTOR_BIN1_IOMUX                                      (IOMUX_PINCM38)
/* Defines for BIN2: GPIOA.17 with pinCMx 39 on package pin 32 */
#define DC_MOTOR_BIN2_PIN                                       (DL_GPIO_PIN_17)
#define DC_MOTOR_BIN2_IOMUX                                      (IOMUX_PINCM39)
/* Defines for GS1: GPIOA.27 with pinCMx 60 on package pin 47 */
#define GRAY_SENSOR_GS1_PORT                                             (GPIOA)
#define GRAY_SENSOR_GS1_PIN                                     (DL_GPIO_PIN_27)
#define GRAY_SENSOR_GS1_IOMUX                                    (IOMUX_PINCM60)
/* Defines for GS2: GPIOA.12 with pinCMx 34 on package pin 27 */
#define GRAY_SENSOR_GS2_PORT                                             (GPIOA)
#define GRAY_SENSOR_GS2_PIN                                     (DL_GPIO_PIN_12)
#define GRAY_SENSOR_GS2_IOMUX                                    (IOMUX_PINCM34)
/* Defines for GS3: GPIOA.18 with pinCMx 40 on package pin 33 */
#define GRAY_SENSOR_GS3_PORT                                             (GPIOA)
#define GRAY_SENSOR_GS3_PIN                                     (DL_GPIO_PIN_18)
#define GRAY_SENSOR_GS3_IOMUX                                    (IOMUX_PINCM40)
/* Defines for GS4: GPIOB.3 with pinCMx 16 on package pin 15 */
#define GRAY_SENSOR_GS4_PORT                                             (GPIOB)
#define GRAY_SENSOR_GS4_PIN                                      (DL_GPIO_PIN_3)
#define GRAY_SENSOR_GS4_IOMUX                                    (IOMUX_PINCM16)
/* Defines for GS5: GPIOB.9 with pinCMx 26 on package pin 23 */
#define GRAY_SENSOR_GS5_PORT                                             (GPIOB)
#define GRAY_SENSOR_GS5_PIN                                      (DL_GPIO_PIN_9)
#define GRAY_SENSOR_GS5_IOMUX                                    (IOMUX_PINCM26)
/* Defines for GS6: GPIOA.9 with pinCMx 20 on package pin 17 */
#define GRAY_SENSOR_GS6_PORT                                             (GPIOA)
#define GRAY_SENSOR_GS6_PIN                                      (DL_GPIO_PIN_9)
#define GRAY_SENSOR_GS6_IOMUX                                    (IOMUX_PINCM20)
/* Defines for GS7: GPIOB.19 with pinCMx 45 on package pin 38 */
#define GRAY_SENSOR_GS7_PORT                                             (GPIOB)
#define GRAY_SENSOR_GS7_PIN                                     (DL_GPIO_PIN_19)
#define GRAY_SENSOR_GS7_IOMUX                                    (IOMUX_PINCM45)


/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWMA_init(void);
void SYSCFG_DL_PWMB_init(void);
void SYSCFG_DL_UART_DEBUG_init(void);


bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
