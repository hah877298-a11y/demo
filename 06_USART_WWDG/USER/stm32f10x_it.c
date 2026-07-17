#include "stm32f10x.h"

/*
 * ============================================================
 *  stm32f10x_it.c - 中断服务函数
 * ============================================================
 */

/*
 * wwdg_ewi_triggered - EWI 中断触发标志
 *
 * 当 WWDG 计数器减到 0x40 时，WWDG_IRQHandler 被调用，
 * 这个变量被设为 1。主循环检测到它就知道中断发生了。
 *
 * 为什么不在中断里直接打印？
 * 因为从 0x40 到 0x3F (复位) 只有约 910 微秒，
 * 串口发一个字符就要约 87 微秒，根本来不及。
 * 设置一个变量只需要几十纳秒。
 */
volatile uint8_t wwdg_ewi_triggered = 0;

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

/*
 * ============================================================
 *  WWDG_IRQHandler - WWDG 早期唤醒中断
 * ============================================================
 *
 *  触发条件: WWDG 计数器 == 0x40 (十进制 64)
 *
 *  必须做的:
 *    1. 清除中断标志 (否则会反复触发)
 *    2. 设置通知变量 (告诉主循环)
 *
 *  触发后还剩约 910us 就复位了。
 */
void WWDG_IRQHandler(void)
{
    if (WWDG_GetFlagStatus() != RESET)
    {
        WWDG_ClearFlag();
        wwdg_ewi_triggered = 1;
    }
}
