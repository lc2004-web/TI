#include "delay.h"

/**********************************************************
*** Emm_V5.0 closed-loop stepper driver library
*** Author: ZHANGDATOU
*** Technical Support: Zhangdatou Closed-loop Servo
*** Taobao Store: https://zhangdatou.taobao.com
*** CSDN Blog: https://blog.csdn.net/zhangdatou666
*** QQ Group: 262438510
**********************************************************/

/**
  * @brief    Millisecond-level delay (using SysTick)
  * @param    i32Cnt: delay count in ms
  * @retval   None
  */
#define SystemCoreClock     32000000
void delay_ms(int32_t i32Cnt)
{
    __IO int32_t i32end = 0;

    SysTick->LOAD = 0xFFFFFF;
    SysTick->VAL  = 0;
    SysTick->CTRL = (SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk);

    while(i32Cnt > 0)
    {
        SysTick->VAL = 0;
        i32end = 0x1000000 - (SystemCoreClock / 1000); // Default 32MHz divided
        while(SysTick->VAL > i32end);
        --i32Cnt;
    }

    SysTick->CTRL = (SysTick->CTRL & (~SysTick_CTRL_ENABLE_Msk));
}

/**
  * @brief    Simple counter-based delay
  * @param    i32Cnt: delay counter value
  * @retval   None
  */
void delay_cnt(int32_t i32Cnt)
{
    while(i32Cnt > 0) { i32Cnt--; }
}
