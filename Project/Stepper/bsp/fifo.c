#include "fifo.h"

/**********************************************************
*** Emm_V5.0 closed-loop stepper driver library
*** Author: ZHANGDATOU
*** Technical Support: Zhangdatou Closed-loop Servo
*** Taobao Store: https://zhangdatou.taobao.com
*** CSDN Blog: https://blog.csdn.net/zhangdatou666
*** QQ Group: 262438510
**********************************************************/

__IO FIFO_t rxFIFO = {0};

/**
  * @brief   Initialize queue
  * @param   None
  * @retval  None
  */
void fifo_initQueue(void)
{
    rxFIFO.ptrRead  = 0;
    rxFIFO.ptrWrite = 0;
}

/**
  * @brief   Enqueue
  * @param   data: data to enqueue
  * @retval  None
  */
void fifo_enQueue(uint16_t data)
{
    rxFIFO.buffer[rxFIFO.ptrWrite] = data;

    ++rxFIFO.ptrWrite;

    if(rxFIFO.ptrWrite >= FIFO_SIZE)
    {
        rxFIFO.ptrWrite = 0;
    }
}

/**
  * @brief   Dequeue
  * @param   None
  * @retval  dequeued element
  */
uint16_t fifo_deQueue(void)
{
    uint16_t element = 0;

    element = rxFIFO.buffer[rxFIFO.ptrRead];

    ++rxFIFO.ptrRead;

    if(rxFIFO.ptrRead >= FIFO_SIZE)
    {
        rxFIFO.ptrRead = 0;
    }

    return element;
}

/**
  * @brief   Check if queue is empty
  * @param   None
  * @retval  true if empty, false otherwise
  */
bool fifo_isEmpty(void)
{
    if(rxFIFO.ptrRead == rxFIFO.ptrWrite)
    {
        return true;
    }

    return false;
}

/**
  * @brief   Get queue length
  * @param   None
  * @retval  number of elements in queue
  */
uint16_t fifo_queueLength(void)
{
    if(rxFIFO.ptrRead <= rxFIFO.ptrWrite)
    {
        return (rxFIFO.ptrWrite - rxFIFO.ptrRead);
    }
    else
    {
        return (FIFO_SIZE - rxFIFO.ptrRead + rxFIFO.ptrWrite);
    }
}
