#include "stm32f10x.h"                  // Device header
#include "OLED_ZIKU.h"
#include "OLED.h"
#include "SYSTICK.h"
#include "XUNJI.h"
#include "PID.h"
#include "USART.h"


extern u8 Data[6];
extern u8 Temp;
extern u8 func_index;
extern void (*current_operation_index)();



//union 
//      {
//       float sdata;
//       char  cdata[4];

//      }a;


//union   {
//        float ii;
//        unsigned char cc[4];

//        }yyy;


void Home()
{
//	OLED_Write_Display(0,0,2,16,(u8 *)xj[0]);
//	OLED_Write_Display(0,16,2,16,(u8 *)xj[1]);

		Read_Sensor();
		for(u8 i=0;i<6;i++)
		{
			
			OLED_Write_Display(2,i*8,2,8,(u8 *)shuzi[Data[i]]);

		}
}
void Menu_1()
{
	for(u8 i=2;i<6;i++)
	{	
	OLED_Write_Display(2,i*8,2,8,(u8 *)kg);
	}
	OLED_Write_Display(0,0,2,16,(u8 *)xj[0]);
	OLED_Write_Display(0,16,2,16,(u8 *)xj[1]);
	OLED_Write_Display(2,0,2,8,(u8 *)KP[0]);
	OLED_Write_Display(2,8,2,8,(u8 *)KP[1]);
	OLED_Write_Display(4,0,2,8,(u8 *)KP[2]);
	OLED_Write_Display(4,8,2,8,(u8 *)KP[3]);
	OLED_Write_Display(6,0,2,8,(u8 *)KP[4]);
	OLED_Write_Display(6,8,2,8,(u8 *)KP[5]);
	OLED_Write_Display(0,112,2,16,(u8 *)gb);
	OLED_Write_Display(6,112,2,16,(u8 *)kg);
}

void Menu_2()
{
	OLED_Write_Display(0,0,2,16,(u8 *)xj[0]);
	OLED_Write_Display(0,16,2,16,(u8 *)xj[1]);
	OLED_Write_Display(2,0,2,8,(u8 *)KP[0]);
	OLED_Write_Display(2,8,2,8,(u8 *)KP[1]);
	OLED_Write_Display(4,0,2,8,(u8 *)KP[2]);
	OLED_Write_Display(4,8,2,8,(u8 *)KP[3]);
	OLED_Write_Display(6,0,2,8,(u8 *)KP[4]);
	OLED_Write_Display(6,8,2,8,(u8 *)KP[5]);
	OLED_Write_Display(2,112,2,16,(u8 *)gb);
	OLED_Write_Display(0,112,2,16,(u8 *)kg);	
}
void Menu_3()
{
	OLED_Write_Display(0,0,2,16,(u8 *)xj[0]);
	OLED_Write_Display(0,16,2,16,(u8 *)xj[1]);
	OLED_Write_Display(2,0,2,8,(u8 *)KP[0]);
	OLED_Write_Display(2,8,2,8,(u8 *)KP[1]);
	OLED_Write_Display(4,0,2,8,(u8 *)KP[2]);
	OLED_Write_Display(4,8,2,8,(u8 *)KP[3]);
	OLED_Write_Display(6,0,2,8,(u8 *)KP[4]);
	OLED_Write_Display(6,8,2,8,(u8 *)KP[5]);
	OLED_Write_Display(4,112,2,16,(u8 *)gb);
	OLED_Write_Display(2,112,2,16,(u8 *)kg);
}

void Menu_4()
{
	OLED_Write_Display(0,0,2,16,(u8 *)xj[0]);
	OLED_Write_Display(0,16,2,16,(u8 *)xj[1]);
	OLED_Write_Display(2,0,2,8,(u8 *)KP[0]);
	OLED_Write_Display(2,8,2,8,(u8 *)KP[1]);
	OLED_Write_Display(4,0,2,8,(u8 *)KP[2]);
	OLED_Write_Display(4,8,2,8,(u8 *)KP[3]);
	OLED_Write_Display(6,0,2,8,(u8 *)KP[4]);
	OLED_Write_Display(6,8,2,8,(u8 *)KP[5]);
	OLED_Write_Display(6,112,2,16,(u8 *)gb);
	OLED_Write_Display(4,112,2,16,(u8 *)kg);

}


void Menu_1_1()
{
	
	Read_Sensor();
	for(u8 i=0;i<6;i++)
	{	
	OLED_Write_Display(2,i*8,2,8,(u8 *)shuzi[Data[i]]);
	}
	TIM_Cmd(TIM2,ENABLE);

}
void Menu_2_KP()
{
	OLED_Write_Clear();
	OLED_Write_Display(2,0,2,16,(u8 *)qing[0]);
	OLED_Write_Display(2,16,2,16,(u8 *)qing[1]);
	OLED_Write_Display(2,32,2,16,(u8 *)qing[2]);
	OLED_Write_Display(2,48,2,16,(u8 *)qing[3]);
	OLED_Write_Display(2,64,2,16,(u8 *)qing[4]);
	Delay_ms(100);
	while(Temp==0);
	OLED_Write_Clear();
	OLED_Write_Display(2,52,2,8,(u8 *)ok[0]);
	OLED_Write_Display(2,60,2,8,(u8 *)ok[1]);
	OLED_Write_Display(2,68,2,8,(u8 *)ok[2]);
	pid.Kp=Temp*10;
	Usart_SendByte(USART3,Temp);
	Temp=0;
	Delay_ms(1000);
	func_index=1;
	OLED_Write_Clear();
}
void Menu_3_KI()
{
	OLED_Write_Clear();
	OLED_Write_Display(2,0,2,16,(u8 *)qing[0]);
	OLED_Write_Display(2,16,2,16,(u8 *)qing[1]);
	OLED_Write_Display(2,32,2,16,(u8 *)qing[2]);
	OLED_Write_Display(2,48,2,16,(u8 *)qing[3]);
	OLED_Write_Display(2,64,2,16,(u8 *)qing[4]);
	Delay_ms(100);
	while(Temp==0);
	OLED_Write_Clear();
	OLED_Write_Display(2,52,2,8,(u8 *)ok[0]);
	OLED_Write_Display(2,60,2,8,(u8 *)ok[1]);
	OLED_Write_Display(2,68,2,8,(u8 *)ok[2]);
	pid.Ki=Temp/10;
	Usart_SendByte(USART3,Temp);
	Temp=0;
	Delay_ms(1000);
	func_index=1;
	OLED_Write_Clear();

}
void Menu_4_KD()
{
	OLED_Write_Clear();
	OLED_Write_Display(2,0,2,16,(u8 *)qing[0]);
	OLED_Write_Display(2,16,2,16,(u8 *)qing[1]);
	OLED_Write_Display(2,32,2,16,(u8 *)qing[2]);
	OLED_Write_Display(2,48,2,16,(u8 *)qing[3]);
	OLED_Write_Display(2,64,2,16,(u8 *)qing[4]);
	Delay_ms(100);
	while(Temp==0);
	OLED_Write_Clear();
	OLED_Write_Display(2,52,2,8,(u8 *)ok[0]);
	OLED_Write_Display(2,60,2,8,(u8 *)ok[1]);
	OLED_Write_Display(2,68,2,8,(u8 *)ok[2]);
	pid.Kd=Temp*100;
	Usart_SendByte(USART3,Temp);
	Temp=0;
	Delay_ms(1000);
	func_index=1;
	OLED_Write_Clear();
}

