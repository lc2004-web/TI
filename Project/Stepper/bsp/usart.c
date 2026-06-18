#include "usart.h"

/**********************************************************
*** Emm_V5.0 closed-loop stepper driver library
*** Author: ZHANGDATOU
*** Technical Support: Zhangdatou Closed-loop Servo
*** Taobao Store: https://zhangdatou.taobao.com
*** CSDN Blog: https://blog.csdn.net/zhangdatou666
*** QQ Group: 262438510
**********************************************************/

__IO uint8_t rxCmd[FIFO_SIZE] = {0};
__IO uint8_t rxCount = 0;

/**
  * @brief   UART_0 interrupt handler
  * @param   None
  * @retval  None
  */
void UART_0_INST_IRQHandler(void)
{
    /**********************************************************
    *** UART receive interrupt
    **********************************************************/
    if(DL_UART_getPendingInterrupt(UART_0_INST) == DL_UART_IIDX_RX)
    {
        // One frame of data received, push into buffer
        fifo_enQueue((uint8_t)DL_UART_Main_receiveData(UART_0_INST));
    }

    // Clear UART receive interrupt flag
    DL_UART_clearInterruptStatus(UART_0_INST, DL_UART_IIDX_RX);
}

/**
  * @brief   Read received command from FIFO buffer
  * @param   None
  * @retval  None
  */
void usart_getCmd(void)
{
    __IO uint16_t i = 0;

    // Read one frame of received data
    rxCount = fifo_queueLength();
    for(i=0; i < rxCount; i++) { rxCmd[i] = fifo_deQueue(); }
}

/**
  * @brief   USART send multiple bytes
  * @param   cmd: byte array to send
  * @param   len: number of bytes
  * @retval  None
  */
void usart_SendCmd(__IO uint8_t *cmd, uint8_t len)
{
    __IO uint8_t i = 0;

    for(i=0; i < len; i++) { usart_SendByte(cmd[i]); }
}

/**
  * @brief   USART send one byte
  * @param   data: byte to send
  * @retval  None
  */
void usart_SendByte(uint16_t data)
{
    __IO uint16_t t0 = 0;

    // Wait when UART is busy, send when not busy
    while(DL_UART_isBusy(UART_0_INST) == true)
    {
        ++t0; if(t0 > 8000) { return; } // Timeout exit, prevent blocking
    }

    // Send the byte
    DL_UART_Main_transmitData(UART_0_INST, data);
}
