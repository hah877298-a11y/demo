#include "stm32f10x.h"
#include "adc.h"

/*
 * ============================================================
 *  stm32f10x_it.c - 中断服务函数
 * ============================================================
 */

/* ====== 系统异常处理 ====== */
void NMI_Handler(void)           {}
void HardFault_Handler(void)     { while (1); }
void MemManage_Handler(void)     { while (1); }
void BusFault_Handler(void)      { while (1); }
void UsageFault_Handler(void)    { while (1); }
void SVC_Handler(void)           {}
void DebugMon_Handler(void)      {}
void PendSV_Handler(void)        {}
void SysTick_Handler(void)       {}

void WWDG_IRQHandler(void)
{
    /* 当前实验不使用 WWDG，做空处理防止死机 */
}

/*
 * ============================================================
 *  ADC1_2_IRQHandler - (保留空壳，DMA 模式下不使用)
 * ============================================================
 *
 *  当前使用 DMA 自动搬运模式，EOC 中断已禁用。
 *  保留此空函数防止意外触发时程序跑飞。
 */
void ADC1_2_IRQHandler(void)
{
    
}
