/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */
#include "bsp_tb6612.h"
#include "board.h"

/******************************************************************
 * 函 数 名 称：TB6612_Motor_Stop
 * 函 数 说 明：A端和B端电机停止
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
void TB6612_Motor_Stop(void)
{
DL_GPIO_setPins(Motor_PORT,Motor_AIN1_PIN);
DL_GPIO_setPins(Motor_PORT,Motor_AIN2_PIN);
DL_GPIO_setPins(Motor_PORT,Motor_BIN1_PIN);
DL_GPIO_setPins(Motor_PORT,Motor_BIN2_PIN);
}

/******************************************************************
 * 函 数 名 称：AO_Control
 * 函 数 说 明：A端口电机控制
 * 函 数 形 参：旋转方向  大于0 正转  反转   speed旋转速度，范围（0 ~ per-1）
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：speed 0-1000
******************************************************************/
void Motor_SetPWM (uint8_t n, int16_t speed)
{
    if(n == 1)
    {
            if( speed >=0 )
    {

           DL_GPIO_clearPins(Motor_PORT,Motor_AIN2_PIN);
        DL_GPIO_setPins(Motor_PORT,Motor_AIN1_PIN);
        if(speed > 1000) speed = 999;
       DL_TimerG_setCaptureCompareValue(PWM_0_INST, speed, GPIO_PWM_0_C1_IDX);
    }
    else
    {
            DL_GPIO_clearPins(Motor_PORT,Motor_AIN1_PIN);
        DL_GPIO_setPins(Motor_PORT,Motor_AIN2_PIN);

        if(speed < -1000) speed = -999;
        DL_TimerG_setCaptureCompareValue(PWM_0_INST, -speed, GPIO_PWM_0_C1_IDX);
    }

    }

    else if (n == 2)
    {
            if(speed >=0 )
    {
            DL_GPIO_setPins(Motor_PORT,Motor_BIN1_PIN);
        DL_GPIO_clearPins(Motor_PORT,Motor_BIN2_PIN);

        if(speed > 1000) speed = 999;
        DL_TimerG_setCaptureCompareValue(PWM_0_INST, speed, GPIO_PWM_0_C0_IDX);
    }
    else
    {

             DL_GPIO_clearPins(Motor_PORT,Motor_BIN1_PIN);
        DL_GPIO_setPins(Motor_PORT,Motor_BIN2_PIN);
        if(speed < -1000) speed = -999;
         DL_TimerG_setCaptureCompareValue(PWM_0_INST,-speed, GPIO_PWM_0_C0_IDX);
    }
    }

}

