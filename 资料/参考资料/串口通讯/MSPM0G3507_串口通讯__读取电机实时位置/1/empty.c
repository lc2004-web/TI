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

// 定义实时位置全局变量
float pos = 0.0f, Motor_Cur_Pos = 0.0f;

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
***	位置模式：方向CW，速度1000RPM，加速度0（不使用加减速直接启动），脉冲数3200（16细分下发送3200个脉冲电机转一圈），相对运动
**********************************************************/	
  Emm_V5_Pos_Control(1, 0, 1000, 0, 3200, 0, 0);
	
/**********************************************************
***	等待返回命令，命令数据缓存在数组rxCmd上，长度为rxCount
**********************************************************/	
	delay_ms(10); usart_getCmd();

/**********************************************************
***	延时2秒，等待运动完成
**********************************************************/	
  delay_ms(2000);

/**********************************************************
***	读取电机实时位置
**********************************************************/	
  Emm_V5_Read_Sys_Params(1, S_CPOS);

/**********************************************************
***	等待返回命令，命令数据缓存在数组rxCmd上，长度为rxCount
**********************************************************/	
	delay_ms(10); usart_getCmd();

/**********************************************************
***	校验地址、功能码、返回数据长度，校验成功则计算当前位置角度
**********************************************************/	
  if(rxCmd[0] == 1 && rxCmd[1] == 0x36 && rxCount == 8)
  {
    // 拼接成uint32_t类型
    pos = (uint32_t)(
                      ((uint32_t)rxCmd[3] << 24)    |
                      ((uint32_t)rxCmd[4] << 16)    |
                      ((uint32_t)rxCmd[5] << 8)     |
                      ((uint32_t)rxCmd[6] << 0)
                    );

    // 转换成角度
    Motor_Cur_Pos = (float)pos * 360.0f / 65536.0f;

    // 符号
    if(rxCmd[2]) { Motor_Cur_Pos = -Motor_Cur_Pos; }
  }

/**********************************************************
***	WHILE循环
**********************************************************/	
	while(1)
	{
	}
}
