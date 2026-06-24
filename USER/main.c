#include "stm32f10x.h"
#include "stm32f10x_usart.h"   // 串口功能
#include "stm32f10x_gpio.h"    // 引脚功能
#include "stm32f10x_rcc.h"     // 时钟功能

void USART1_SendChar(char ch)
  {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, ch);
	
	}  

	 void USART1_SendString(char *str)
  {
        while (*str)
        {
                USART1_SendChar(*str);
                str++;
        }
  }

int main (void )
{
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
   GPIO_InitTypeDef GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   USART_InitTypeDef USART_InitStructure;
   USART_InitStructure.USART_BaudRate = 115200;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_Mode = USART_Mode_Tx;
   USART_Init(USART1, &USART_InitStructure);

   USART_Cmd(USART1, ENABLE);

    USART1_SendString("Hello World\r\n");

        while (1)
        {
        }
	
	
}
