#ifndef __USART_H
#define __USART_H


void USART3_Config();
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);
void USART1_Config();
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num);



#endif
