#ifndef __DMA_M2M_H
#define __DMA_M2M_H

//#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_dma.h"
#include "misc.h"


#define BUFFSIZE 32
extern uint32_t DST_Buffer[BUFFSIZE];
extern const uint32_t SRC_Buffer[BUFFSIZE];
extern uint32_t CurrDataCounter;
extern void dam_m2m_int(void);

//void LED_PC13_Init(void); 
//void LED_PC13_on(void);   
//void LED_PC13_off(void);  

#endif