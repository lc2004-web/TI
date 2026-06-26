#include "stm32f10x.h"                  // Device header
#include "SYSTICK.h"
#include "HY_SRF05.h"


TIM_ICUserValueTypeDef TIM_ICUserValueStructure = {0,0,0,0};

static void HY_SRF05_TIM3()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure; 

	TIM_TimeBaseStructure.TIM_Period=65535;	
	TIM_TimeBaseStructure.TIM_Prescaler= 71;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInit(TIM3,&TIM_ICInitStructure);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update | TIM_IT_CC4);	

	TIM_ITConfig (TIM3, TIM_IT_Update | TIM_IT_CC4, ENABLE );

	TIM_Cmd(TIM3, ENABLE);		
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void HY_SRF05_Config()
{
	GPIO_InitTypeDef GPiO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//设置PB5为Trig引脚
	GPiO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPiO_InitStructure.GPIO_Pin = Trig;
	GPiO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	
	GPIO_Init(GPIOB,&GPiO_InitStructure);

	GPIO_ResetBits(GPIOB,Trig);//将Trig引脚设为低电平。
	
	//设置PB1引脚为Echo
	GPiO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPiO_InitStructure.GPIO_Pin = Echo;

	GPIO_Init(GPIOB,&GPiO_InitStructure);
	HY_SRF05_TIM3();
}


void HY_SRF05_ceju()
{
	GPIO_SetBits(GPIOB,Trig);
	Delay_us(15);
	GPIO_ResetBits(GPIOB,Trig);
}




