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
 *  ADC1_2_IRQHandler - ADC 转换完成中断
 * ============================================================
 *
 *  触发条件: ADC1 完成一次模数转换（EOC 标志置位）
 *
 *  必须做的:
 *    1. 确认 EOC 标志位已置位
 *    2. 读取转换结果，保存到全局变量 adc_value
 *    3. 清除 EOC 中断标志位
 *
 *  "敲门"比喻:
 *    主程序在运行 (你在看书)，ADC 转换完成 (有人敲门)，
 *    硬件跳到这里来读数据 (你放下书去开门)，
 *    读完回去继续主循环 (开完门回来继续看书)。
 */
void ADC1_2_IRQHandler(void)
{
    if (ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET)
    {
        adc_value = ADC_GetConversionValue(ADC1);
        adc_conversion_done = 1;
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    }
}
