#include "dma_m2m.h"

const uint32_t SRC_Buffer[BUFFSIZE]={
0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,

0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,

0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,

0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,

0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,

0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,

0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,

0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80
};

uint32_t DST_Buffer[BUFFSIZE];

uint32_t CurrDataCounter;
void dma_m2m_int(void)
{
DMA_InitTypeDef DMA_InitStruct;
DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)SRC_Buffer;
DMA_InitStruct.DMA_MemoryBaseAddr=(uint32_t)DST_Buffer;
DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralSRC; 
DMA_InitStruct.DMA_BufferSize=BUFFSIZE;
DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Enable;	
DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;
DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Word;
DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Word;
DMA_InitStruct.DMA_Mode=DMA_Mode_Normal;
DMA_InitStruct.DMA_Priority=DMA_Priority_High;
DMA_InitStruct.DMA_M2M=DMA_M2M_Enable;

NVIC_InitTypeDef NVIC_InitStruct;
NVIC_InitStruct.NVIC_IRQChannel=DMA1_Channel1_IRQn;
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x1;
NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x1;
NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;

RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
DMA_Init( DMA1_Channel1, &DMA_InitStruct);

//使能中断
DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE );

//NVIC
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

NVIC_Init(&NVIC_InitStruct);

//使能DMA通道
DMA_Cmd(DMA1_Channel1, ENABLE );

}