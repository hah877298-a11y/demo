#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

/*
 * ADC_CHANNEL_COUNT - 规则组通道数（方便修改）
 * adc_buffer       - DMA 自动搬运的多通道结果数组
 *
 * DMA1_Channel1 在后台持续将 ADC_DR 搬运到此数组，
 * 主循环直接读取，无需中断。
 */
#define ADC_CHANNEL_COUNT  3
extern volatile uint16_t adc_buffer[ADC_CHANNEL_COUNT];

void ADC_GPIO_Config(void);
void ADC1_Mode_Config(void);

#endif /* __ADC_H */
