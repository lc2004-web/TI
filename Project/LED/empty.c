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

#include "ti_msp_dl_config.h"

/* ====================================================================
 * 外部晶振 (HFXT) 测试程序 — MSPM0G3507
 *
 * 原理：
 *   尝试启用 PA5(HFXIN) / PA6(HFXOUT) 上的外部高频晶振。
 *   通过 LED 闪烁快慢指示晶振是否起振成功。
 *
 * 结果解读：
 *   LED 快速闪烁 (~100ms) → 晶振起振成功，焊接正常 ✓
 *   LED 慢速闪烁 (~1000ms) → 晶振未起振，可能虚焊/短路/未焊接 ✗
 *
 * 晶振引脚（MSPM0G3507 48-Pin LQFP）：
 *   PA5 = HFXIN  = IOMUX PINCM10
 *   PA6 = HFXOUT = IOMUX PINCM11
 *
 * 频率范围设置（根据实际晶振修改 HFXT_RANGE）：
 *   DL_SYSCTL_HFXT_RANGE_4_8_MHZ   —  4 ~ 8 MHz
 *   DL_SYSCTL_HFXT_RANGE_8_16_MHZ  —  8 ~ 16 MHz  (默认)
 *   DL_SYSCTL_HFXT_RANGE_16_32_MHZ — 16 ~ 32 MHz
 *   DL_SYSCTL_HFXT_RANGE_32_48_MHZ — 32 ~ 48 MHz
 * ====================================================================
 */

/* 根据实际焊接的晶振频率修改此宏 */
#define HFXT_RANGE  DL_SYSCTL_HFXT_RANGE_32_48_MHZ

/* 超时计数值 (约 5 秒 @ 32MHz SYSOSC) */
#define HFXT_TIMEOUT  5000000UL

/* Simple delay using busy loops — ~1ms per count at default 32MHz SYSOSC */
static void delay_ms(uint32_t ms)
{
    volatile uint32_t i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 4000; j++) {
            __asm("nop");
        }
    }
}

int main(void)
{
    volatile uint32_t timeout;
    bool hfxt_ok = false;

    /* 系统默认初始化（SYSOSC 32MHz, HFXT 禁用） */
    SYSCFG_DL_init();

    /* ----------------------------------------------------------------
     * 步骤1: 将 PA5/PA6 的 IOMUX 配置为模拟模式
     *        （断开数字功能，避免干扰晶振模拟电路）
     * ---------------------------------------------------------------- */
    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM10);  /* PA5 = HFXIN */
    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM11);  /* PA6 = HFXOUT */

    /* ----------------------------------------------------------------
     * 步骤2: 配置 HFXT 参数
     * ---------------------------------------------------------------- */
    DL_SYSCTL_disableHFXT();                          /* 先禁用以确保干净状态 */
    DL_SYSCTL_setHFXTFrequencyRange(HFXT_RANGE);      /* 设置频率范围 */
    DL_SYSCTL_setHFXTStartupTime(0xFF);               /* 最长启动时间 ~16ms */

    /* ----------------------------------------------------------------
     * 步骤3: 启用 HFXT 晶振
     * ---------------------------------------------------------------- */
    SYSCTL->SOCLOCK.HSCLKEN |= SYSCTL_HSCLKEN_HFXTEN_ENABLE;
    DL_SYSCTL_enableHFCLKStartupMonitor();

    /* ----------------------------------------------------------------
     * 步骤4: 等待晶振起振（带超时）
     * ---------------------------------------------------------------- */
    timeout = HFXT_TIMEOUT;
    while (timeout > 0) {
        if ((DL_SYSCTL_getClockStatus() & DL_SYSCTL_CLK_STATUS_HFCLK_GOOD)
                == DL_SYSCTL_CLK_STATUS_HFCLK_GOOD) {
            hfxt_ok = true;
            break;
        }
        timeout--;
    }

    /* ----------------------------------------------------------------
     * 步骤5: 通过 LED 闪烁快慢指示结果
     * ---------------------------------------------------------------- */
    while (1) {
        /* LED ON */
        DL_GPIO_setPins(LED_PORT, LED_PIN_14_PIN);

        if (hfxt_ok) {
            delay_ms(100);    /* 快速：晶振 OK */
        } else {
            delay_ms(1000);   /* 慢速：晶振失败 */
        }

        /* LED OFF */
        DL_GPIO_clearPins(LED_PORT, LED_PIN_14_PIN);

        if (hfxt_ok) {
            delay_ms(100);
        } else {
            delay_ms(1000);
        }
    }
}
