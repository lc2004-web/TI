/*
 * Copyright (c) 2023, Texas Instruments Incorporated
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
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM0 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

DL_TimerA_backupConfig gPWMABackup;
DL_TimerA_backupConfig gPWMBBackup;

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_PWMA_init();
    SYSCFG_DL_PWMB_init();
    SYSCFG_DL_MOTOR_SPEED_init();
    SYSCFG_DL_MPU6050_init();
    SYSCFG_DL_UART_DEBUG_init();
    SYSCFG_DL_SYSTICK_init();
    /* Ensure backup structures have no valid state */
	gPWMABackup.backupRdy 	= false;
	gPWMBBackup.backupRdy 	= false;



}
/*
 * User should take care to save and restore register configuration in application.
 * See Retention Configuration section for more details.
 */
SYSCONFIG_WEAK bool SYSCFG_DL_saveConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_saveConfiguration(PWMA_INST, &gPWMABackup);
	retStatus &= DL_TimerA_saveConfiguration(PWMB_INST, &gPWMBBackup);

    return retStatus;
}


SYSCONFIG_WEAK bool SYSCFG_DL_restoreConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_restoreConfiguration(PWMA_INST, &gPWMABackup, false);
	retStatus &= DL_TimerA_restoreConfiguration(PWMB_INST, &gPWMBBackup, false);

    return retStatus;
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_TimerA_reset(PWMA_INST);
    DL_TimerA_reset(PWMB_INST);
    DL_TimerG_reset(MOTOR_SPEED_INST);
    DL_I2C_reset(MPU6050_INST);
    DL_UART_Main_reset(UART_DEBUG_INST);


    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_TimerA_enablePower(PWMA_INST);
    DL_TimerA_enablePower(PWMB_INST);
    DL_TimerG_enablePower(MOTOR_SPEED_INST);
    DL_I2C_enablePower(MPU6050_INST);
    DL_UART_Main_enablePower(UART_DEBUG_INST);

    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{

    DL_GPIO_initPeripheralOutputFunction(GPIO_PWMA_C0_IOMUX,GPIO_PWMA_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWMA_C0_PORT, GPIO_PWMA_C0_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_PWMB_C0_IOMUX,GPIO_PWMB_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWMB_C0_PORT, GPIO_PWMB_C0_PIN);

    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_MPU6050_IOMUX_SDA,
        GPIO_MPU6050_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_MPU6050_IOMUX_SCL,
        GPIO_MPU6050_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_MPU6050_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_MPU6050_IOMUX_SCL);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_DEBUG_IOMUX_TX, GPIO_UART_DEBUG_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_DEBUG_IOMUX_RX, GPIO_UART_DEBUG_IOMUX_RX_FUNC);

    DL_GPIO_initDigitalOutput(DC_MOTOR_AIN1_IOMUX);

    DL_GPIO_initDigitalOutput(DC_MOTOR_AIN2_IOMUX);

    DL_GPIO_initDigitalOutput(DC_MOTOR_BIN1_IOMUX);

    DL_GPIO_initDigitalOutput(DC_MOTOR_BIN2_IOMUX);

    DL_GPIO_initDigitalInputFeatures(ENCODER_A_AA_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(ENCODER_A_AB_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(ENCODER_B_BA_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(ENCODER_B_BB_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_clearPins(GPIOA, DC_MOTOR_AIN1_PIN |
		DC_MOTOR_AIN2_PIN |
		DC_MOTOR_BIN1_PIN |
		DC_MOTOR_BIN2_PIN);
    DL_GPIO_enableOutput(GPIOA, DC_MOTOR_AIN1_PIN |
		DC_MOTOR_AIN2_PIN |
		DC_MOTOR_BIN1_PIN |
		DC_MOTOR_BIN2_PIN);
    DL_GPIO_setUpperPinsPolarity(GPIOA, DL_GPIO_PIN_26_EDGE_RISE_FALL);
    DL_GPIO_clearInterruptStatus(GPIOA, ENCODER_A_AA_PIN);
    DL_GPIO_enableInterrupt(GPIOA, ENCODER_A_AA_PIN);
    DL_GPIO_setUpperPinsPolarity(ENCODER_B_PORT, DL_GPIO_PIN_24_EDGE_RISE_FALL);
    DL_GPIO_clearInterruptStatus(ENCODER_B_PORT, ENCODER_B_BA_PIN);
    DL_GPIO_enableInterrupt(ENCODER_B_PORT, ENCODER_B_BA_PIN);

}


SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{

	//Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);

    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    /* Set default configuration */
    DL_SYSCTL_disableHFXT();
    DL_SYSCTL_disableSYSPLL();
    DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_1);
    DL_SYSCTL_setMCLKDivider(DL_SYSCTL_MCLK_DIVIDER_DISABLE);

}


/*
 * Timer clock configuration to be sourced by  / 1 (32000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   32000000 Hz = 32000000 Hz / (1 * (0 + 1))
 */
static const DL_TimerA_ClockConfig gPWMAClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 0U
};

static const DL_TimerA_PWMConfig gPWMAConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN_UP,
    .period = 4000,
    .isTimerWithFourCC = true,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_PWMA_init(void) {

    DL_TimerA_setClockConfig(
        PWMA_INST, (DL_TimerA_ClockConfig *) &gPWMAClockConfig);

    DL_TimerA_initPWMMode(
        PWMA_INST, (DL_TimerA_PWMConfig *) &gPWMAConfig);

    // Set Counter control to the smallest CC index being used
    DL_TimerA_setCounterControl(PWMA_INST,DL_TIMER_CZC_CCCTL0_ZCOND,DL_TIMER_CAC_CCCTL0_ACOND,DL_TIMER_CLC_CCCTL0_LCOND);

    DL_TimerA_setCaptureCompareOutCtl(PWMA_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_0_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(PWMA_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
    DL_TimerA_setCaptureCompareValue(PWMA_INST, 0, DL_TIMER_CC_0_INDEX);

    DL_TimerA_enableClock(PWMA_INST);


    
    DL_TimerA_setCCPDirection(PWMA_INST , DL_TIMER_CC0_OUTPUT );


}
/*
 * Timer clock configuration to be sourced by  / 1 (32000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   32000000 Hz = 32000000 Hz / (1 * (0 + 1))
 */
static const DL_TimerA_ClockConfig gPWMBClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 0U
};

static const DL_TimerA_PWMConfig gPWMBConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN_UP,
    .period = 4000,
    .isTimerWithFourCC = true,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_PWMB_init(void) {

    DL_TimerA_setClockConfig(
        PWMB_INST, (DL_TimerA_ClockConfig *) &gPWMBClockConfig);

    DL_TimerA_initPWMMode(
        PWMB_INST, (DL_TimerA_PWMConfig *) &gPWMBConfig);

    // Set Counter control to the smallest CC index being used
    DL_TimerA_setCounterControl(PWMB_INST,DL_TIMER_CZC_CCCTL0_ZCOND,DL_TIMER_CAC_CCCTL0_ACOND,DL_TIMER_CLC_CCCTL0_LCOND);

    DL_TimerA_setCaptureCompareOutCtl(PWMB_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_0_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(PWMB_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
    DL_TimerA_setCaptureCompareValue(PWMB_INST, 0, DL_TIMER_CC_0_INDEX);

    DL_TimerA_enableClock(PWMB_INST);


    
    DL_TimerA_setCCPDirection(PWMB_INST , DL_TIMER_CC0_OUTPUT );


}



/*
 * Timer clock configuration to be sourced by BUSCLK /  (32000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   320000 Hz = 32000000 Hz / (1 * (99 + 1))
 */
static const DL_TimerG_ClockConfig gMOTOR_SPEEDClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale    = 99U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * MOTOR_SPEED_INST_LOAD_VALUE = (50ms * 320000 Hz) - 1
 */
static const DL_TimerG_TimerConfig gMOTOR_SPEEDTimerConfig = {
    .period     = MOTOR_SPEED_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC_UP,
    .startTimer = DL_TIMER_STOP,
};

SYSCONFIG_WEAK void SYSCFG_DL_MOTOR_SPEED_init(void) {

    DL_TimerG_setClockConfig(MOTOR_SPEED_INST,
        (DL_TimerG_ClockConfig *) &gMOTOR_SPEEDClockConfig);

    DL_TimerG_initTimerMode(MOTOR_SPEED_INST,
        (DL_TimerG_TimerConfig *) &gMOTOR_SPEEDTimerConfig);
    DL_TimerG_enableInterrupt(MOTOR_SPEED_INST , DL_TIMERG_INTERRUPT_LOAD_EVENT);
    DL_TimerG_enableClock(MOTOR_SPEED_INST);





}


static const DL_I2C_ClockConfig gMPU6050ClockConfig = {
    .clockSel = DL_I2C_CLOCK_BUSCLK,
    .divideRatio = DL_I2C_CLOCK_DIVIDE_1,
};

SYSCONFIG_WEAK void SYSCFG_DL_MPU6050_init(void) {

    DL_I2C_setClockConfig(MPU6050_INST,
        (DL_I2C_ClockConfig *) &gMPU6050ClockConfig);
    DL_I2C_setAnalogGlitchFilterPulseWidth(MPU6050_INST,
        DL_I2C_ANALOG_GLITCH_FILTER_WIDTH_50NS);
    DL_I2C_enableAnalogGlitchFilter(MPU6050_INST);
    DL_I2C_setDigitalGlitchFilterPulseWidth(MPU6050_INST,
        DL_I2C_DIGITAL_GLITCH_FILTER_WIDTH_CLOCKS_1);

    /* Configure Controller Mode */
    DL_I2C_resetControllerTransfer(MPU6050_INST);
    /* Set frequency to 100000 Hz*/
    DL_I2C_setTimerPeriod(MPU6050_INST, 31);
    DL_I2C_setControllerTXFIFOThreshold(MPU6050_INST, DL_I2C_TX_FIFO_LEVEL_BYTES_7);
    DL_I2C_setControllerRXFIFOThreshold(MPU6050_INST, DL_I2C_RX_FIFO_LEVEL_BYTES_8);
    DL_I2C_enableControllerClockStretching(MPU6050_INST);


    /* Enable module */
    DL_I2C_enableController(MPU6050_INST);


}

static const DL_UART_Main_ClockConfig gUART_DEBUGClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_DEBUGConfig = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_DEBUG_init(void)
{
    DL_UART_Main_setClockConfig(UART_DEBUG_INST, (DL_UART_Main_ClockConfig *) &gUART_DEBUGClockConfig);

    DL_UART_Main_init(UART_DEBUG_INST, (DL_UART_Main_Config *) &gUART_DEBUGConfig);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115211.52
     */
    DL_UART_Main_setOversampling(UART_DEBUG_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_DEBUG_INST, UART_DEBUG_IBRD_32_MHZ_115200_BAUD, UART_DEBUG_FBRD_32_MHZ_115200_BAUD);


    /* Configure FIFOs */
    DL_UART_Main_enableFIFOs(UART_DEBUG_INST);
    DL_UART_Main_setRXFIFOThreshold(UART_DEBUG_INST, DL_UART_RX_FIFO_LEVEL_1_2_FULL);
    DL_UART_Main_setTXFIFOThreshold(UART_DEBUG_INST, DL_UART_TX_FIFO_LEVEL_1_2_EMPTY);

    DL_UART_Main_enableMajorityVoting(UART_DEBUG_INST);

    DL_UART_Main_enable(UART_DEBUG_INST);
}

SYSCONFIG_WEAK void SYSCFG_DL_SYSTICK_init(void)
{
    /*
     * Initializes the SysTick period to 1.00 ms,
     * enables the interrupt, and starts the SysTick Timer
     */
    DL_SYSTICK_config(32000);
}

