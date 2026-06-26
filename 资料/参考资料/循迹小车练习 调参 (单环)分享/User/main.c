#include "stm32f10x.h"                  // Device header
#include <string.h>
#include "USART.h"
#include "SYSTICK.h"
#include "OLED.h"
#include "I2C.h"
#include "OLED_ZIKU.h"
#include "XUNJI.h"
#include "Car.h"
#include "TIM2.h"
#include "PID.h"
//#include "KEY.h"
#include "FLASH.h"
#include "HY_SRF05.h"

u8 Temp=0;
u8 Time;
u8 Data[6]={5};
u8 Parameter[2]={0};
u8 count=0,juli;
float flag;
uint8_t wi[4]={0x00,0x00,0x80,0x7f};


union test
{
        float a;
        char b[4];

}x;





 //memset
int main(void) //记得看readme那个文档
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	USART3_Config();
	USART1_Config();
//	EXTI_Config();
	TIM1_Init();
	I2C1_Init();
	OLED_Init();
	PID_Init();
	xunji_Init();
	OLED_Write_Display(0,0,2,16,(u8 *)xj[0]);
	OLED_Write_Display(0,16,2,16,(u8 *)xj[1]);
	HY_SRF05_Config();
	TIM2_Init();
//	TIM_Cmd(TIM2,ENABLE);
	Car_Config();
	TIM_SetCompare3(TIM4,0);
	TIM_SetCompare4(TIM4,0);
	TIM_Cmd(TIM2,DISABLE);

	while(1)
	{
		OLED_Write_Clear();
		while(Parameter[0]<6) //蓝牙遥控的循环。
		{
			OLED_Write_Display(2,32,2,16,(u8 *)yao[0]);
			OLED_Write_Display(2,48,2,16,(u8 *)yao[1]);
			OLED_Write_Display(2,64,2,16,(u8 *)yao[2]);
			OLED_Write_Display(2,80,2,16,(u8 *)yao[3]);
			TIM_SetCompare3(TIM4,0);
			TIM_SetCompare4(TIM4,0);
			TIM_Cmd(TIM2,DISABLE);
			if(Parameter[0]==6)
				break;
			if(Parameter[1]==1)
			{
				qianjin();
				OLED_Write_Display(4,48,2,16,(u8 *)qian[0]);
				OLED_Write_Display(4,64,2,16,(u8 *)qian[1]);
			}
			else if(Parameter[1]==2)
			{
				left_();
				OLED_Write_Display(4,48,2,16,(u8 *)zuo[0]);
				OLED_Write_Display(4,64,2,16,(u8 *)zuo[1]);
			}
			else if(Parameter[1]==3)
			{
				right_();
				OLED_Write_Display(4,48,2,16,(u8 *)you[0]);
				OLED_Write_Display(4,64,2,16,(u8 *)you[1]);
			}
			else if(Parameter[1]==4)
			{
				houtui();
				OLED_Write_Display(4,48,2,16,(u8 *)you[0]);
				OLED_Write_Display(4,64,2,16,(u8 *)you[1]);
			}
			else if(Parameter[1]>6)
			{
				stop();
				OLED_Write_Display(4,48,2,16,(u8 *)ting[0]);
				OLED_Write_Display(4,64,2,16,(u8 *)ting[1]);
			}
		}
		OLED_Write_Clear();
		TIM_Cmd(TIM2,ENABLE);
		OLED_Write_Display(0,0,2,16,(u8 *)xj[0]);
		OLED_Write_Display(0,16,2,16,(u8 *)xj[1]);
		TIM_Cmd(TIM1,ENABLE);
		GPIO_SetBits(GPIOA,GPIO_Pin_5);
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
		GPIO_SetBits(GPIOA,GPIO_Pin_7);
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);
		while(Parameter[0]==06) //寻迹循环
		{
			Read_Sensor();
			for(u8 i=0;i<6;i++)
			{	
			OLED_Write_Display(2,i*8,2,8,(u8 *)shuzi[Data[i]]);
			}
			if(Time==1)
			{
				PID_Calc();
//				Position_P();
				PID_OUT();
				Time=0;
			}
			if(juli==1)
			{
				juli=0;
				HY_SRF05_ceju();
			}
			if(TIM_ICUserValueStructure.Capture_FinishFlag==1)
			{
				x.a=(TIM_ICUserValueStructure.Capture_Period*(65536)+(TIM_ICUserValueStructure.Capture_CcrValue+1))/58.0;
					TIM_Cmd(TIM1,ENABLE);
					TIM1->CNT=0;
				if(x.a<25)
				{
					Parameter[0]=9;			
					TIM_ICUserValueStructure.Capture_FinishFlag=0;
					break;
				}
				TIM_ICUserValueStructure.Capture_FinishFlag=0;			
			}
		}			

		while(Parameter[0]==9)  //避障循环
		{
			TIM_Cmd(TIM2,DISABLE);
						
			TIM_SetCompare3(TIM4,4000);
			TIM_SetCompare4(TIM4,2000);
			Delay_ms(500);
			TIM_SetCompare3(TIM4,2000);
			TIM_SetCompare4(TIM4,4000);
			Delay_ms(650);
			TIM_SetCompare3(TIM4,2200);
			TIM_SetCompare4(TIM4,2500);
			while(1)
			{
				Read_Data();					
				if((Data[0]||Data[1]||Data[2]||Data[3]||Data[4]||Data[5])==1)
					break;
				TIM_SetCompare3(TIM4,2200);
				TIM_SetCompare4(TIM4,2500);
			}					
									
			Parameter[0]=6;
			Time=1;
			break;	
		}									
	}
}




