#include "led.h"
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>

void LED_PC13_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    GPIO_SetBits(GPIOC, GPIO_Pin_13); // 默认灭
}

void LED_PC13_on(void)
{
    GPIO_ResetBits(GPIOC, GPIO_Pin_13); // 点亮
}

void LED_PC13_off(void)
{
    GPIO_SetBits(GPIOC, GPIO_Pin_13);   // 熄灭
}