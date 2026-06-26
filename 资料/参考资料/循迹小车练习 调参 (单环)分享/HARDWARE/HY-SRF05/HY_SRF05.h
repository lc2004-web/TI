#ifndef __HY_SRF05_H
#define __HY_SRF05_H



#define Trig  GPIO_Pin_5    //触发端  PB5
#define Echo  GPIO_Pin_1	//接收端  PB1

// 定时器输入捕获用户自定义变量结构体声明
typedef struct
{   
	uint8_t   Capture_FinishFlag;   // 捕获结束标志位
	uint8_t   Capture_StartFlag;    // 捕获开始标志位
	uint16_t  Capture_CcrValue;     // 捕获寄存器的值
	uint16_t  Capture_Period;       // 自动重装载寄存器更新标志 
}TIM_ICUserValueTypeDef;

extern TIM_ICUserValueTypeDef TIM_ICUserValueStructure;


void HY_SRF05_ceju();

void HY_SRF05_Config();

#endif
