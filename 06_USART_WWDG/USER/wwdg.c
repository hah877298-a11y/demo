#include "wwdg.h"
#include "usart.h"

/*
 * ============================================================
 *  WWDG_Config - 窗口看门狗初始化
 * ============================================================
 *
 *  【配置参数】
 *  PCLK1 = 36MHz (APB1 总线时钟)
 *  WWDG 计数器时钟 = 36MHz / 4096 / 8 = 1099 Hz
 *  每个 tick = 1/1099 = 约 910 微秒
 *
 *  计数器初值 = 0x7F (127)
 *  窗口值     = 0x5F (95)
 *  总超时     = (127-63) * 910us = 约 58 ms
 *  窗口区间   = (95-64)  * 910us = 约 28 ms
 *
 *  【窗口看门狗规则】
 *
 *  127 -----> 95 -----> 64 -----> 63 = 复位!
 *   |<- 禁止喂狗 ->|<- 允许喂狗窗口 ->|<- 太晚 ->|
 *
 *  喂早了(counter > 95): 违规! 复位!
 *  喂晚了(counter < 64): 违规! 复位!
 *  正确(64 <= counter <= 95): 喂狗成功
 */
void WWDG_Config(void)
{
    /* 1. 打开 WWDG 时钟 (WWDG 挂在 APB1 总线上) */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

    /* 2. 分频系数 = 8 (决定计数器递减速度) */
    WWDG_SetPrescaler(WWDG_Prescaler_8);

    /* 3. 窗口值 = 0x5F (95): counter <= 95 才能喂狗 */
    WWDG_SetWindowValue(0x5F);

    /* 4. 使能 EWI (早期唤醒中断) */
    WWDG_ClearFlag();
    WWDG_EnableIT();

    /* 5. NVIC 配置: 收到 WWDG 中断 -> 调用 WWDG_IRQHandler */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel                   = WWDG_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /*
     * 6. 打印配置 (必须在 WWDG_Enable 之前! 否则看门狗在打印期间就超时了)
     */
    UART_SendStr("WWDG Config OK:\r\n");
    UART_SendStr("  Prescaler = 8 (counter clock = 1099 Hz)\r\n");
    UART_SendStr("  Window    = 0x5F (95)\r\n");
    UART_SendStr("  Counter   = 0x7F (127)\r\n");
    UART_SendStr("  Timeout   = ~58 ms, window = 64~95 (~28 ms)\r\n");
    UART_SendStr("\r\n");

    /*
     * 7. 启动 WWDG!
     * 一旦 WDGA 位置 1，计数器开始递减，无法停止，只有复位能关。
     */
    WWDG_Enable(0x7F);
}

/*
 * ============================================================
 *  WWDG_FeedDog - 喂狗
 * ============================================================
 *  把计数器重新设为 127。
 *  调用时机必须在 [64, 95] 窗口内，否则触发复位。
 */
void WWDG_FeedDog(void)
{
    WWDG_SetCounter(0x7F);
}
