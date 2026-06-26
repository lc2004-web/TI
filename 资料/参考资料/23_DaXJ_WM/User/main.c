/*
电机A               电机B
PWA--PA16         PWMB---PA17

AIN1--PA14        BIN1---PA12
AIN2--PA15        BIN2---PA13

E1A---PA24         E2A---PA26
E1B---PA25        E2B---PA22
*/
#include "ti_msp_dl_config.h"
#include "board.h"
#include "oled.h"
#include "bmp.h"
#include "bsp_mpu6050.h"
#include "bsp_tb6612.h"
#include "inv_mpu.h"
//#include "Encoder.h"
#include "PID.h"
#include <stdlib.h>  
#include "No_Mcu_Ganv_Grayscale_Sensor_Config.h"
#include "delay.h"
#include "adc.h"
#include "led.h"
#include "key.h"
#include "flash.h"


typedef struct{
    float yaw;
} Attitude_TypeDef;

Attitude_TypeDef attitude;
uint16_t Num,Num1;
uint8_t ID;
int16_t AX, AY, AZ, GX, GY, GZ;
short accData[3];
short gyroData[3];

/*====================================*/
int16_t LeftPWM,  RightPWM;
int16_t AvePWM, DifPWM;

/* 新增：保存上一次正常输出的PWM值，用于丢线时保持 */
int16_t lastLeftPWM = 0, lastRightPWM = 0;

/*=========================================*/
// 全局变量定义
unsigned short Anolog[8] = {0};    // 存储当前模拟量值的数组
unsigned short white[8] = {0};     // 存储白色校准值的数组 
unsigned short black[8] = {0};     // 存储黑色校准值的数组
unsigned short Normal[8];          // 归一化值数组

No_MCU_Sensor sensor;              // 传感器数据结构体
unsigned char Digtal;              // 数字输出值

float ALL_Num;
uint8_t DX_flg;
uint8_t State,Last_State;
int main(void)
{
    SYSCFG_DL_init();

    MPU6050_Init();
    OLED_Init();
    LED_init();  // 初始化LED
    TB6612_Motor_Stop();
  //  Encoder_Init();
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);

   // lc_printf("Hello %s\r\n", "World");
  /*===================================================*/
    /* DMA配置 - 用于ADC数据传输 */
    // 设置DMA源地址(ADC存储器)
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0->ULLMEM.MEMRES[0]);
    // 设置DMA目标地址(ADC_VALUE缓冲区)
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC_VALUE[0]);		
    // 使能DMA通道
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);;
    // 启动ADC转换
    DL_ADC12_startConversion(ADC_VOLTAGE_INST);
    
    // 使能按键中断
    NVIC_EnableIRQ(GRAY_IN_INT_IRQN);
			
    // 从Flash存储器读取校准值
    readWhiteFromFlash();  // 读取白色校准值
    readBlackFromFlash();  // 读取黑色校准值

    // 使用校准值初始化传感器
    No_MCU_Ganv_Sensor_Init(&sensor, white, black);

    //无MCU灰度传感器硬件起振需要时间
    Tick_delay(100);
    state.value=KEY_IDLE;
		
    while(1)
    {
        // 读取 MPU6050 数据
        MPU6050ReadAcc(accData);
        AX = accData[0];
        AY = accData[1];
        AZ = accData[2];
        
        MPU6050ReadGyro(gyroData);
        GX = gyroData[0];
        GY = gyroData[1];
        GZ = gyroData[2];
      
        OLED_ShowSignedNum(0,0,(int)attitude.yaw, 4, 16, 1);

//=============================循迹信息获取==================================//
        if (state.value == KEY_IDLE||state.value == KEY_DISABLE||state.value == KEY_WAIT_LOSS ) {
            // 正常操作模式(非校准状态)
            
            // 执行无时基依赖的传感器任务
            No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
            
            // 从传感器获取当前模拟量值
            Get_Anolog_Value(&sensor, Anolog);
            
            // 将模拟量转换为数字输出
            Digtal = Get_Digtal_For_User(&sensor);
					
        } 
        else {
            // 校准模式 - 将数字输出置0，八路LED灯关闭
            Digtal = 0;
        }
				     No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
                        Get_Normalize_For_User(&sensor, Normal);
                        Digtal=Get_Digtal_For_User(&sensor);      
        // 处理按键输入
        Key_Process();
        LED_KEY_Blink_Update();
        
              unsigned char dig_str[9];
        for (int i = 0; i < 8; i++) {
            dig_str[i] = ((Digtal >> i) & 0x01) + '0';
        }
        dig_str[8] = '\0';
        OLED_ShowString(0, 56, dig_str, 8, 1);

//------------------------确定黑线位置----------------------------------------------//
    
            int32_t weighted_sum = 0;
            int16_t z = 0;
            int16_t led1,led2,led3,led4,led5,led6,led7,led8;
            if( (Digtal>>0)&1 == 1) { z++; led1 = 1; } else { led1 = 0; }
            if( (Digtal>>1)&1 == 1) { z++; led2 = 2; } else { led2 = 0; }
            if( (Digtal>>2)&1 == 1) { z++; led3 = 3; } else { led3 = 0; }
            if( (Digtal>>3)&1 == 1) { z++; led4 = 4; } else { led4 = 0; }
            if( (Digtal>>4)&1 == 1) { z++; led5 = 5; } else { led5 = 0; }
            if( (Digtal>>5)&1 == 1) { z++; led6 = 6; } else { led6 = 0; }
            if( (Digtal>>6)&1 == 1) { z++; led7 = 7; } else { led7 = 0; }
            if( (Digtal>>7)&1 == 1) { z++; led8 = 8; } else { led8 = 0; }

            weighted_sum = led1+led2+led3+led4+led5+led6+led7+led8;

            if(z == 0) {
                ALL_Num = 0;   //防止z = 0带入分母计算
            } else {
                ALL_Num = (float)weighted_sum / z;              
            }
            int index = (int)(ALL_Num * 2 + 0.5);

//-----------------------------查表对应速度--------------------------------------//

            switch (index) {
                case 2:   // ALL_Num ≈ 1        
                DifPWM =200;
                    break;
                case 3:   // ALL_Num ≈ 1.5
                    DifPWM =150;
                    break;
                case 4:   // ALL_Num ≈ 2
                    DifPWM =120;
                    break;
                case 5:   // ALL_Num ≈ 2.5
                    DifPWM =100;
                    break;
                case 6:   // ALL_Num ≈ 3
                    DifPWM =80;
                    break;
                case 7:   // ALL_Num ≈ 3.5
                    DifPWM =50;
                    break;
                case 8:   // ALL_Num ≈ 4
                    DifPWM =30;
                    break;
                case 9:   // ALL_Num ≈ 4.5
                    DifPWM =0;
                    break;
                case 10:  // ALL_Num ≈ 5
                    DifPWM =-30;
                    break;
                case 11:  // ALL_Num ≈ 5.5
                    DifPWM =-50;
                    break;
                case 12:  // ALL_Num ≈ 6
                    DifPWM =-80;
                    break;
                case 13:  // ALL_Num ≈ 6.5
                    DifPWM =-100;
                    break;
                case 14:  // ALL_Num ≈ 7
                    DifPWM= -120;
                    break;
                case 15:  // ALL_Num ≈ 7.5
                    DifPWM =-150;
                    break;
                case 16:  // ALL_Num ≈ 8
                    DifPWM =-200;
                    break;
                default:
                  

                    break;
            }

/*==================== 电机控制====================*/    
 
            if(DX_flg) //丢线状态下
            {    
                    if(Last_State >4.5){
                Motor_SetPWM(1, -80);
                    Motor_SetPWM(2, 180);        
                    }
                else if(Last_State <4.5)
                    {
                Motor_SetPWM(1, 180);
                    Motor_SetPWM(2, -80); }
                    
                else if (Last_State == 4.5)
                    {
                Motor_SetPWM(1, 120);
                    Motor_SetPWM(2, 120); 
                    }
               
                    // 如果已经脱离全白状态，则清除标志
                    if((Digtal & 0xFF) != 0xFF) {
                        DX_flg = 0;
                    }
                
                continue; 
            }

            else//未丢线状态下
             {
                   AvePWM = 160 - fabs(ALL_Num - 4.5) * 20;

                    LeftPWM  = AvePWM + DifPWM;
                    RightPWM  = AvePWM - DifPWM;

                            if (LeftPWM >= 400) {LeftPWM = 400;} else if (LeftPWM < -400) {LeftPWM = -400;}
                            if (RightPWM >=400) {RightPWM = 400;} else if (RightPWM < -400) {RightPWM = -400;}

                            Motor_SetPWM(1, LeftPWM);
                            Motor_SetPWM(2, RightPWM);                                                   
            }
                 OLED_ShowSignedNum(0,16,LeftPWM, 4, 16, 1);
                  OLED_ShowSignedNum(0,32,RightPWM, 4, 16, 1);
                 OLED_Refresh();

    }
}
//---------------------------中断检测丢线-----------------------------//
void TIMER_0_INST_IRQHandler(void)
{
    switch(DL_TimerG_getPendingInterrupt(TIMER_0_INST))
    {
    
        case DL_TIMER_IIDX_ZERO:      
           
            Num++;
            Num1++;
            if(Num >2 ) {//ms
                Num = 0;          

                    if (Digtal == 0xFF && DX_flg == 0) {   
                            DX_flg = 1;        // 全白丢线，进入丢线状态                
                        }
                    if(DX_flg == 0)
                    {
                      Last_State = State;//一直记录2ms前的寻线状态，也就是作为丢线后的转向依据。
                      State = ALL_Num;
                    }
             
            }

            break;
        default:
            break;
    }
}

/**
 * @brief 外部中断处理函数（CLK和按键）
 * @note  检测到有效按键后设置key_pressed标志
 */
void GROUP1_IRQHandler(void)
{
	    // 读取Group1的中断寄存器并清除中断标志位
    uint32_t pending = DL_GPIO_getPendingInterrupt(GPIOB);
	
    if(pending == GRAY_IN_IN_KEY_IIDX){
            /* 防抖处理 */
            if ((Tick - last_key_time) < DEBOUNCE_TIME_MS) {
                return;
            }
            /* 确认按键按下 */
            if (DL_GPIO_readPins(GRAY_IN_PORT, GRAY_IN_IN_KEY_PIN) == 0) {
                key_pressed = 1;
								long_pressed_key_time=Tick;
                last_key_time = Tick;
            }
    }
}