#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include <stdio.h>
#include "led.h"

extern int led_flags;
int led_flags = 0;
// 系统必备的中断函数
void NMI_Handler(void) {}
void HardFault_Handler(void) { while (1); }
void MemManage_Handler(void) { while (1); }
void BusFault_Handler(void) { while (1); }
void UsageFault_Handler(void) { while (1); }
void SVC_Handler(void) {}
void DebugMon_Handler(void) {}
void PendSV_Handler(void) {}
void SysTick_Handler(void) {}

// --- 按键中断逻辑 ---
void EXTI4_IRQHandler(void)
{
    //  EXTI Line4 触发的中断
    if(EXTI_GetITStatus(EXTI_Line4) != RESET) 
    {
        // [ LED 翻转逻辑]
        if(led_flags == 0){
              LED_PC13_on();
              led_flags = 1;            
        }else{
              LED_PC13_off();
              led_flags = 0;
        }
        // 清中断标志位！
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}