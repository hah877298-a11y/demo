#ifndef __STM32F10x_IT_H
#define __STM32F10x_IT_H

#include "stm32f10x.h"

// 必须包含的基本系统中断（即使你现在不用，写上也是好习惯）
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

// 你将来会用到的按键中断函数声明
void EXTI0_IRQHandler(void);

#endif /* __STM32F10x_IT_H */
