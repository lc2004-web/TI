#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "OLED_ZIKU.h"
#include "USART.h"
#include "SYSTICK.h"
#include <stdlib.h>
#include <stdio.h>
#include "FLASH.h"
#include "XUNJI.h"
#include "HY_SRF05.h"



extern u8 Time;
extern u8 left;
extern u8 Data[6];
extern u8 Temp;
extern float flag;
u8 i=0;
//u8 b=0;
static char e=0;
char buff[4]={0};
typedef struct
{
	float Sv;
	float Kp;
	float Kd;
	float Ki;
	float EK;
	float EK_1;//上次偏差
	float SEK;//历史偏差之和
	float OUT;

}PID;


PID pid;

void PID_Init()
{
	pid.Sv=0;
	pid.Kp=Read_FLASH(KP1_Addr);
	pid.Ki=Read_FLASH(KI1_Addr);
	pid.Kd=Read_FLASH(KD1_Addr);
	pid.EK_1=0;
}
//void Position_P()   //基础速度PID控制器//这里Encoder=a，Target=0;
//{
//	
//	//参数整定，需要调到适合自己小车的数值大小  //因为没有用到KI(积分)，所以这里去除了和KI相关的式子
//	 static float Bias,Last_Bias;
//	 Bias=sudu; //偏差=实际值-目标值
//	 pid.OUT2=4000-pid.Kp2*Bias+pid.Kd2*(Bias-Last_Bias);//位置式PID控制器//这里7200是小车电机PWM的最大值，也就是主函数TIM2的传参传入的数，当error为零时，表示小车处于黑线的中央，没有偏移，所以这时a=0，使得最后返回的PWM值为7200，使电机达到最大速度
//	 Last_Bias=Bias;                                       //保存上一次偏差 
//                                          //返回PWM值
//}

void PID_Calc()
{
	pid.EK=pid.Sv-flag;
	pid.SEK+=pid.EK;
//	pid.OUT=pid.Kp*pid.EK+pid.Kp*(pid.T/pid.Ti)*pid.SEK+pid.Kp*(pid.Td/pid.T)*(pid.EK-pid.EK_1)+pid.OUT0;
	pid.OUT=pid.Kp*pid.EK+pid.Ki*pid.SEK+pid.Kd*(pid.EK-pid.EK_1);
//	pid.OUT=pid.Kp*pid.EK;
	pid.EK_1=pid.EK;
}
//int Position_PID (int Encoder,int Target)   //差速PID控制器//这里Encoder=error，Target=0;
//{ 	
//	 float Position_KP=200,Position_KI=0.1,Position_KD=1000;
//	 static float Bias,Pwm,Integral_bias,Last_Bias;
//	 Bias=Encoder-Target;                                  //计算偏差
//	 Integral_bias+=Bias;	                                 //求出偏差的积分
////	 Pwm=Position_KP*Bias+Position_KI*Integral_bias+Position_KD*(Bias-Last_Bias);       //位置式PID控制器
//	 Pwm=Position_KP*Bias+Position_KD*(Bias-Last_Bias)+200;       //位置式PID控制器

//	Last_Bias=Bias;                                       //保存上一次偏差 
//	 return Pwm;                                           //返回PWM值
//}
void PID_OUT()
{
	float  OUT1,OUT2;
//	int OUT3;
	OUT1=3000+pid.OUT;
	OUT2=3000-pid.OUT;
	if(OUT1>7200)
		OUT1=7200;
	if(OUT1<0)
		OUT1=0;	
	if(OUT2>7200)
		OUT2=7200;
	if(OUT2<0)
		OUT2=0;
	TIM_SetCompare3(TIM4,OUT1);
	TIM_SetCompare4(TIM4,OUT2);
//	b++;

	if(left==1) //下面的这些代码就主要是检测那个大弯的。
	{
		i=0;
		left=0;
	}
	if(i<23)
		i++;

	if(flag>2)
	{
		e=1;
	}
	else if(flag<-2)
		e=-1;
	if(Temp==10)
	{
		Temp=0;
		if(e<0 || (i<20&&e>0))
		{
			TIM_Cmd(TIM2,DISABLE);
			TIM_SetCompare3(TIM4,0);
			TIM_SetCompare4(TIM4,4000);
			while(1)
			{
				Read_Data();
				if((Data[0]||Data[1]||Data[2]||Data[3]||Data[4]||Data[5])==1)
				{
					TIM_SetCompare4(TIM4,0);
//					Delay_ms(10);
					TIM2->CNT=40;
					TIM_Cmd(TIM2,ENABLE);
//					Time=1;
					return;				
				}
			}
	
		}

		else if(e>0)
		{
			TIM_Cmd(TIM2,DISABLE);
			TIM_SetCompare3(TIM4,4000);
			TIM_SetCompare4(TIM4,0);
			while(1)
			{
				Read_Data();
				if((Data[0]||Data[1]||Data[2]||Data[3]||Data[4]||Data[5])==1)
				{
					TIM_SetCompare3(TIM4,0);
//					Delay_ms(10);
					TIM2->CNT=40;
					TIM_Cmd(TIM2,ENABLE);
//					Time=1;
					return;				
				}
			}
	
		}
		
	}


//	if(b>=10&&TIM_ICUserValueStructure.Capture_FinishFlag==1)
//	{
//	HY_SRF05_ceju();
////	sprintf(buff,"%f",flag);
////	Usart_SendString(USART3,buff);
//	b=0;
////	Usart_SendArray(USART3,Data,6);
////	Usart_SendString(USART1,"             ");
//	}
}

