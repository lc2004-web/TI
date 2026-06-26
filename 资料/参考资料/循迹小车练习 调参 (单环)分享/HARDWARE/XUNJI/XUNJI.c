#include "stm32f10x.h"                  // Device header

extern u8 Data[6];
extern u8 Temp;
u8 left;
extern float flag;
void xunji_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2  \
									| GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_12;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	


}

void Read_Data()
{
	Data[0]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
	Data[1]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1);
	Data[2]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2);
	Data[3]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3);
	Data[4]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);
	Data[5]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12);	
}
void Read_Sensor()
{
	Data[0]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
	Data[1]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1);
	Data[2]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2);
	Data[3]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3);
	Data[4]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);
	Data[5]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12);
	//error大于0，表示小车偏右，error小于0，表示小车偏左

	if(Data[0]==0&&Data[1]==0&&Data[2]==1&&Data[3]==1&&Data[4]==0&&Data[5]==0)
	{
		flag=0;
	}
	else if(Data[0]==1&&Data[1]==1&&Data[2]==1&&Data[3]==1&&Data[4]==1&&Data[5]==1)
	{
		flag=0;
	}
	else if(Data[0]==0&&Data[1]==0&&Data[2]==1&&Data[3]==0&&Data[4]==0&&Data[5]==0)
	{
		flag=1;
	}
	else if(Data[0]==0&&Data[1]==1&&Data[2]==1&&Data[3]==0&&Data[4]==0&&Data[5]==0)
	{
		flag=2;
	}
	else if(Data[0]==0&&Data[1]==1&&Data[2]==1&&Data[3]==1&&Data[4]==0&&Data[5]==0)
	{
		flag=1;
	}
	else if(Data[0]==0&&Data[1]==1&&Data[2]==0&&Data[3]==0&&Data[4]==0&&Data[5]==0)
	{
		flag=3;
	}
	else if(Data[0]==1&&Data[1]==1&&Data[2]==0&&Data[3]==0&&Data[4]==0&&Data[5]==0)
	{
		flag=5;
	}
	else if(Data[0]==1&&Data[1]==1&&Data[2]==1&&Data[3]==0&&Data[4]==0&&Data[5]==0)
	{
		flag=6;
	}
	else if(Data[0]==1&&Data[1]==1&&Data[2]==1&&Data[3]==1&&Data[4]==0&&Data[5]==0)
	{
		flag=6;
	}
	else if(Data[0]==1&&Data[1]==0&&Data[2]==0&&Data[3]==0&&Data[4]==0&&Data[5]==0)
	{
		flag=5;
	}
	else if(Data[0]==1&&Data[1]==0&&Data[2]==1&&Data[3]==0&&Data[4]==0&&Data[5]==0)
	{
		flag=7,left=1;
	}
	else if(Data[0]==1&&Data[1]==0&&Data[2]==1&&Data[3]==1&&Data[4]==0&&Data[5]==0)
	{
//		flag=7;
		flag=7,left=1;
	}
	else if(Data[0]==1&&Data[1]==1&&Data[2]==1&&Data[3]==1&&Data[4]==0&&Data[5]==0)
	{
		flag=6;
	}
	else if(Data[0]==1&&Data[1]==1&&Data[2]==1&&Data[3]==1&&Data[4]==1&&Data[5]==0)
	{
		flag=7;
	}
	else if(Data[0]==0&&Data[1]==0&&Data[2]==0&&Data[3]==1&&Data[4]==0&&Data[5]==0)
	{
		flag=-1;
	}
	else if(Data[0]==0&&Data[1]==0&&Data[2]==0&&Data[3]==1&&Data[4]==1&&Data[5]==0)
	{
		flag=-2;
	}
	else if(Data[0]==0&&Data[1]==0&&Data[2]==1&&Data[3]==1&&Data[4]==1&&Data[5]==0)
	{
		flag=-2;
	}
	else if(Data[0]==0&&Data[1]==0&&Data[2]==0&&Data[3]==0&&Data[4]==1&&Data[5]==0)
	{
		flag=-3;
	}	
	else if(Data[0]==0&&Data[1]==0&&Data[2]==0&&Data[3]==0&&Data[4]==1&&Data[5]==1)
	{
		flag=-5;
	}	
	else if(Data[0]==0&&Data[1]==0&&Data[2]==0&&Data[3]==1&&Data[4]==1&&Data[5]==1)
	{
		flag=-5;
	}	
	else if(Data[0]==0&&Data[1]==0&&Data[2]==0&&Data[3]==0&&Data[4]==0&&Data[5]==1)
	{
		flag=-5;
	}
	else if(Data[0]==0&&Data[1]==0&&Data[2]==1&&Data[3]==1&&Data[4]==1&&Data[5]==1)
	{
		flag=-6;
	}
	else if(Data[0]==0&&Data[1]==0&&Data[2]==0&&Data[3]==1&&Data[4]==0&&Data[5]==1)
	{
		flag=-7;
	}
	else if(Data[0]==0&&Data[1]==0&&Data[2]==1&&Data[3]==1&&Data[4]==0&&Data[5]==1)
	{
		flag=-7;
	}	
	else if(Data[0]==0&&Data[1]==0&&Data[2]==1&&Data[3]==1&&Data[4]==1&&Data[5]==1)
	{
		flag=-7;
	}
	else if(Data[0]==0&&Data[1]==1&&Data[2]==1&&Data[3]==1&&Data[4]==1&&Data[5]==1)
	{
		flag=-7;
	}
	else if(Data[0]==0&&Data[1]==0&&Data[2]==0&&Data[3]==0&&Data[4]==0&&Data[5]==0)
	{
		Temp=10;
	}

}