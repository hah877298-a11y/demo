#include"stm32f10x_gpio.h"
#include"stm32f10x_rcc.h"
#include"stm32f10x_exti.h"
#include"misc.h"
#include"led.h"
#include"dma_m2m.h"

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

TestStatus Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
{
    while(BufferLength--)
    {
        if(*pBuffer != *pBuffer1)
        {
            return FAILED;
        }

        pBuffer++;
        pBuffer1++;
    }

    return PASSED;
}

void EXTI4_PB4_Init(void)
{
    // ===== 第1步：开启时钟 =====
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    // ===== 第2步：禁用 JTAG，释放 PB4 =====
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    // ===== 第3步：配置 PB4 为上拉输入 =====
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // ===== 第4步：连接 PB4 → EXTI Line4 =====
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);

    // ===== 第5步：配置 EXTI（下降沿触发）=====
    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_Line    = EXTI_Line4;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    // ===== 第6步：配置 NVIC 中断优先级 =====
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel                   = EXTI4_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 0;
    NVIC_Init(&NVIC_InitStruct);
}

int main (void)
{
	TestStatus ret;
    //EXTI4_PB4_Init();
    LED_PC13_Init();
	dma_m2m_int();
	
	CurrDataCounter=DMA_GetCurrDataCounter(DMA1_Channel1);
	
	while(DMA_GetCurrDataCounter(DMA1_Channel1) != 0);
	
	ret=Buffercmp(SRC_Buffer,DST_Buffer,BUFFSIZE);
	
	if(ret==PASSED){
	LED_PC13_on();
	}else{
	LED_PC13_off();
	}
    for(;;)
    {
        
    }
    
   //retrun 0;
}