#include <stdio.h>

#include "stm32f10x.h"

static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void USART3_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//¿ªÆôUSART1ºÍGPIOAµÄÊ±ÖÓ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 ,ENABLE);

	//ÅäÖÃPA9ÎªTX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //¸´ÓÃÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	//ÅäÖÃPA10ÎªRX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //¸¡¿ÕÊäÈëÄ£Ê½ö
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 9600;//ÅäÖÃ²¨ÌØÂÊ
	//ÅäÖÃÓ²¼şÁ÷¿ØÖÆ£º±¾Àı³Ì²»²ÉÓÃ Ó²¼şÁ÷
	USART_InitStructure.USART_HardwareFlowControl =
						USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;//²»ÉèÖÃÆæÅ¼Ğ£ÑéÎ»
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//Í£Ö¹Î» ÉèÖÃÎª1Î»
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//´®¿Ú´«ÊäµÄ²½³¤£º8Î»
	USART_Init(USART3,&USART_InitStructure);

	NVIC_Configuration();
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART3,ENABLE);

}
void USART1_Config()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//¿ªÆôUSART1ºÍGPIOAµÄÊ±ÖÓ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 ,ENABLE);

	//ÅäÖÃPA9ÎªTX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //¸´ÓÃÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//ÅäÖÃPA10ÎªRX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //¸¡¿ÕÊäÈëÄ£Ê½ö
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;//ÅäÖÃ²¨ÌØÂÊ
	//ÅäÖÃÓ²¼şÁ÷¿ØÖÆ£º±¾Àı³Ì²»²ÉÓÃ Ó²¼şÁ÷
	USART_InitStructure.USART_HardwareFlowControl =
						USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;//²»ÉèÖÃÆæÅ¼Ğ£ÑéÎ»
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//Í£Ö¹Î» ÉèÖÃÎª1Î»
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//´®¿Ú´«ÊäµÄ²½³¤£º8Î»
	USART_Init(USART1,&USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART1,ENABLE);

}

/*****************  ·¢ËÍÒ»¸ö×Ö½Ú **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ·¢ËÍÒ»¸ö×Ö½ÚÊı¾İµ½USART */
	USART_SendData(pUSARTx,ch);
		
	/* µÈ´ı·¢ËÍÊı¾İ¼Ä´æÆ÷Îª¿Õ */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/****************** ·¢ËÍ8Î»µÄÊı×é ************************/
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    /* ·¢ËÍÒ»¸ö×Ö½ÚÊı¾İµ½USART */
	    Usart_SendByte(pUSARTx,array[i]);	
  
  }
	/* µÈ´ı·¢ËÍÍê³É */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}

/*****************  ·¢ËÍ×Ö·û´® **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* µÈ´ı·¢ËÍÍê³É */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  ·¢ËÍÒ»¸ö16Î»Êı **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* È¡³ö¸ß°ËÎ» */
	temp_h = (ch&0XFF00)>>8;
	/* È¡³öµÍ°ËÎ» */
	temp_l = ch&0XFF;
	
	/* ·¢ËÍ¸ß°ËÎ» */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* ·¢ËÍµÍ°ËÎ» */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

///ÖØ¶¨Ïòc¿âº¯Êıprintfµ½´®¿Ú£¬ÖØ¶¨Ïòºó¿ÉÊ¹ÓÃprintfº¯Êı
int fputc(int ch, FILE *f)
{
		/* ·¢ËÍÒ»¸ö×Ö½ÚÊı¾İµ½´®¿Ú */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* µÈ´ı·¢ËÍÍê±Ï */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///ÖØ¶¨Ïòc¿âº¯Êıscanfµ½´®¿Ú£¬ÖØĞ´Ïòºó¿ÉÊ¹ÓÃscanf¡¢getcharµÈº¯Êı
int fgetc(FILE *f)
{
		/* µÈ´ı´®¿ÚÊäÈëÊı¾İ */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}



