#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

void UART1_Configuration(void);
void CheckAndReportResetSource(void);

/* 自实现的串口输出函数（不用 printf，不依赖 MicroLIB） */
void UART_SendByte(uint8_t byte);
void UART_SendStr(const char *str);
void UART_SendDec(uint32_t num);
void UART_SendHex(uint8_t num);

#endif
