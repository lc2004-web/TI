#include "stm32f10x.h"                  // Device header
#include "SYSTICK.h"

static void OLED_Write_CMD(u8 Cmd)
{
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	
	I2C_GenerateSTART(I2C1,ENABLE);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1,0X78,I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1,0x00);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_SendData(I2C1,Cmd);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTOP(I2C1,ENABLE);
}

static void OLED_Write_Data(u8 Data)
{
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	
	I2C_GenerateSTART(I2C1,ENABLE);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1,0X78,I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1,0x40);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_SendData(I2C1,Data);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTOP(I2C1,ENABLE);
}

void OLED_Write_Clear()
{
	for(u8 i=0;i<8;i++)
	{
		OLED_Write_CMD(0Xb0|i);
		OLED_Write_CMD(0x00);
		OLED_Write_CMD(0x10);
			
		for(u8 n=0;n<128;n++)
		{	
			 OLED_Write_Data(0x00); // 0xff
		}
	}
}

void OLED_Init()
{
	Delay_ms(200);
	const u8 oledcmd[]={0XAE,0X00,0X10,0X40,0X81,0XCF,0XA1,0XC8,0XA6,0XA8,0X3F,0XD3,0X00,0XD5,0X80,
											0XD9,0XF1,0XDA,0X12,0XDB,0X40,0X20,0X02,0X8D,0X14,0XA4,0XA6,0XAF,0XAF,0XAF};
	for(u8 i=0;i<sizeof(oledcmd);i++)
	{
		OLED_Write_CMD(oledcmd[i]);
		
	}
	OLED_Write_Clear();
}

void OLED_Write_ON()
{
	OLED_Write_CMD(0x8D);
	OLED_Write_CMD(0x24);
	OLED_Write_CMD(0xAF);
}

void OLED_Write_OFF()
{
	OLED_Write_CMD(0x8D);
	OLED_Write_CMD(0x24);
	OLED_Write_CMD(0xAE);
}

void OLED_Write_Display(u8 x,u8 y,u8 g,u8 k,u8 *dat)
{	
	for(u8 i=0;i<g;i++){ 
		
		OLED_Write_CMD((0xb0|i)+x); //0~7 64
		OLED_Write_CMD(0x10+(y>>4&0x0f)); 
		OLED_Write_CMD(y&0x0f); 
		
		for(u8 j=0;j<k;j++){ //16*8 16*16
			
			OLED_Write_Data(*dat++);
		}
	}
}
