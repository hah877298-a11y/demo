#ifndef __USART_H
#define __USART_H

#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include <stdio.h>

void UART1_Configuration(void);
void CheckAndReportResetSource(void);

#endif
