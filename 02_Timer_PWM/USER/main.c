/*TIM4 CH1-CH4 PWM OUTPUT*/

#include<stm32f10x_gpio.h>
#include<stm32f10x_rcc.h>
#include<stm32f10x_tim.h>

void tim4_pwm_init(void)
{   
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_Period=999;
    TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Down;
    TIM_TimeBaseInitStruct.TIM_Prescaler=71;

    TIM_OCInitTypeDef TIM_OCInitStruct;
    TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_Low;
    TIM_OCInitStruct.TIM_OutputState=ENABLE;
    TIM_OCInitStruct.TIM_Pulse=500;

    //使能TIM4的时钟->APB1
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    //使能GPIO的时钟->GPIOB->APB2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    //初始化GPIOB 6 7 8 9 引脚
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    //初始化TIM4定时器
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
    //初始化输出通道
    TIM_OC1Init( TIM4,  &TIM_OCInitStruct);
    TIM_OC2Init( TIM4,  &TIM_OCInitStruct);
    TIM_OC3Init( TIM4,  &TIM_OCInitStruct);
    TIM_OC4Init( TIM4,  &TIM_OCInitStruct);
    //使能比较寄存器预装载功能
    TIM_OC1PreloadConfig( TIM4,  TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig( TIM4,  TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig( TIM4,  TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig( TIM4,  TIM_OCPreload_Enable);
    //使能重装载功能
    TIM_ARRPreloadConfig( TIM4, ENABLE);
    //使能定时器的计数功能
    TIM_Cmd( TIM4, ENABLE);
}



int main(void)
{
    tim4_pwm_init();

    while(1){

    }
    //return 0;    
}
