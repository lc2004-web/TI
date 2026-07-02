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
 
#include "Emm_V5.h"

/**
	*	@brief		MAIN函数
	*	@param		无
	*	@retval		无
	*/
int main(void)
{
/**********************************************************
***	初始化板载外设
**********************************************************/	
	// 初始化系统引脚配置
	SYSCFG_DL_init();
	// 清除串口中断标志
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
	// 使能串口中断
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

/**********************************************************
***	上电延时500毫秒等待闭环初始化完毕
**********************************************************/	
	delay_ms(500);

/**********************************************************
***	设置快速位置模式的运动参数
	* 方向CW，速度1000RPM，加速度0（直接启动），脉冲数3200（16细分下发送3200个脉冲电机转一圈），绝对模式运动
	* 注：延时10毫秒是为了防止指令粘包，如果多个电机之间要同步启动不想延时，可以用多电机命令
**********************************************************/	
	Emm_V5_Set_QPos_Params(1, 1000, 0, 1, 0); delay_ms(10);

/**********************************************************
***	快速位置模式：运动到3200（360°）处
	* 注：延时10毫秒是为了防止指令粘包，如果多个电机之间要同步启动不想延时，可以用多电机命令
**********************************************************/	
  Emm_V5_QPos_Control(1, 3200); delay_ms(10);

/**********************************************************
***	延时2秒，等待运动完成
**********************************************************/	
  delay_ms(2000);

/**********************************************************
***	快速位置模式：运动到-3200（-360°）处
	* 注：延时10毫秒是为了防止指令粘包，如果多个电机之间要同步启动不想延时，可以用多电机命令
**********************************************************/	
  Emm_V5_QPos_Control(1, -3200); delay_ms(10);

/**********************************************************
***	等待返回命令，命令数据缓存在数组rxCmd上，长度为rxCount
**********************************************************/	
	delay_ms(10); usart_getCmd();

/**********************************************************
***	WHILE循环
**********************************************************/	
	while(1)
	{
	}
}
