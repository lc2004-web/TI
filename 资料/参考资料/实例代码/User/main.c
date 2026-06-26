#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"

void delayMs(int delay);
void GPIO_Config(void);
uint8_t Read_8Road_Sensor(void);  //读取8路PA0~PA7

int main(void)
{
    /*模块初始化*/
    GPIO_Config();
    OLED_Init();

    while (1)
    {
        uint8_t sensorData = Read_8Road_Sensor();
        
        //依次在第1行1~8列显示8路状态
        OLED_ShowChar(1, 1, (sensorData & (1<<0)) ? '1' : '0');
        OLED_ShowChar(1, 2, (sensorData & (1<<1)) ? '1' : '0');
        OLED_ShowChar(1, 3, (sensorData & (1<<2)) ? '1' : '0');
        OLED_ShowChar(1, 4, (sensorData & (1<<3)) ? '1' : '0');
        OLED_ShowChar(1, 5, (sensorData & (1<<4)) ? '1' : '0');
        OLED_ShowChar(1, 6, (sensorData & (1<<5)) ? '1' : '0');
        OLED_ShowChar(1, 7, (sensorData & (1<<6)) ? '1' : '0');
        OLED_ShowChar(1, 8, (sensorData & (1<<7)) ? '1' : '0');

        delayMs(10);
    }
}

//配置PA0~PA7全部为上拉输入
void GPIO_Config(void)
{
    //开启GPIOA时钟
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    //PA0~PA7由CRL寄存器控制(引脚0-7对应CRL)
    //每4位控制一个引脚，一次性清空PA0~PA7所有配置位
    GPIOA->CRL &= 0x00000000;
    //0x8 = CNF=10(上拉输入) MODE=00
    //每个引脚赋值0x8，8个引脚 0x88888888
    GPIOA->CRL = 0x88888888;
    //ODR全部置1，打开内部上拉电阻
    GPIOA->ODR |= 0x00FF;
}

//一次性读取PA0~PA7 8路传感器，返回8位二进制数值
uint8_t Read_8Road_Sensor(void)
{
    //IDR低8位就是PA0~PA7电平
    return (uint8_t)(GPIOA->IDR & 0x00FF);
}

//简易毫秒延时
void delayMs(int delay)
{
    for (int i = 0; i < delay; i++)
    {
        for (volatile int j = 0; j < 8000; j++);
    }
}