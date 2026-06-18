#ifndef __USART_H
#define __USART_H

#include "fifo.h"

/**********************************************************
*** Emm_V5.0 closed-loop stepper driver library
*** Author: ZHANGDATOU
*** Technical Support: Zhangdatou Closed-loop Servo
*** Taobao Store: https://zhangdatou.taobao.com
*** CSDN Blog: https://blog.csdn.net/zhangdatou666
*** QQ Group: 262438510
**********************************************************/

extern __IO uint8_t rxCmd[FIFO_SIZE];
extern __IO uint8_t rxCount;

void usart_getCmd(void);
void usart_SendCmd(__IO uint8_t *cmd, uint8_t len);
void usart_SendByte(uint16_t data);

#endif
