#include "stm32f10x.h"
#include "usart.h"
#include "wwdg.h"

/*
 * wwdg_ewi_triggered: 由 WWDG_IRQHandler 置 1
 * 当 EWI 中断触发时被设为 1，主循环检测到即知"中断发生了"。
 */
extern volatile uint8_t wwdg_ewi_triggered;

void Delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * 8000; i++)
    {
        __NOP();
    }
}

/*
 * ============================================================
 *  main() - WWDG 窗口看门狗演示
 * ============================================================
 *
 *  两个阶段自动切换:
 *
 *  阶段 1 (首次上电 POR):
 *    - 正常喂狗 29 次 (每次等 30ms，counter 在窗口内)
 *    - 第 30 次立刻喂狗 (counter 在窗口外) -> WWDG 复位!
 *
 *  阶段 2 (WWDG 复位后):
 *    - 完全不喂狗
 *    - counter 从 127 减到 64 -> EWI 中断触发
 *    - counter 减到 63 -> WWDG 复位!
 *
 *  想看阶段 1: 完全断电 (拔 USB) 再上电
 *  阶段 2 会一直循环: 每次 WWDG 复位后都进入阶段 2
 */
int main(void)
{
    /* ====== 第 1 步: 初始化串口 ====== */
    UART1_Configuration();

    /* ====== 第 2 步: 判断复位类型 ====== */
    /*
     * 在 CheckAndReportResetSource 清除标志之前先读取。
     * 如果上次是 WWDG 复位 -> 进入阶段 2
     * 如果上次是上电复位   -> 进入阶段 1
     */
    uint8_t was_wwdg_reset = 0;
    if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
        was_wwdg_reset = 1;

    UART_SendStr("\r\n");
    UART_SendStr("========================================\r\n");
    UART_SendStr("  WWDG Window Watchdog Demo\r\n");
    UART_SendStr("========================================\r\n");

    /* ====== 第 3 步: 报告复位来源 ====== */
    CheckAndReportResetSource();

    /* ====== 第 4 步: 初始化 WWDG ====== */
    WWDG_Config();

    /* ====== 第 5 步: 选择演示阶段 ====== */
    if (was_wwdg_reset)
    {
        /*
         * ============================================
         *  阶段 2 : "喂狗太晚" — 不喂狗
         * ============================================
         *
         *  时间线:
         *    0ms:  counter = 127
         *    ...
         *   57ms: counter = 0x40 -> EWI 中断!
         *   58ms: counter = 0x3F -> WWDG 复位!
         */
        UART_SendStr(">>> Phase 2: TOO LATE Demo <<<\r\n");
        UART_SendStr("I will NOT feed the watchdog.\r\n");
        UART_SendStr("Counter: 127 -> 64(EWI!) -> 63(RESET!)\r\n");
        UART_SendStr("\r\n");

        uint32_t tick = 0;
        while (1)
        {
            tick++;

            /* 检查 EWI 是否触发 */
            if (wwdg_ewi_triggered)
            {
                UART_SendStr("!!! EWI! System will reset now!\r\n");
                wwdg_ewi_triggered = 0;
            }

            if (tick % 50000 == 0)
            {
                UART_SendStr("[Phase 2] tick=");
                UART_SendDec(tick);
                UART_SendStr(", NOT feeding...\r\n");
            }
        }
    }
    else
    {
        /*
         * ============================================
         *  阶段 1 : "过早喂狗" — 先正常喂，再故意违例
         * ============================================
         *
         *  Loop 1~29: 等 30ms -> counter 减到 ~94 (在窗口内) -> 喂狗 OK
         *  Loop 30:   不等 -> counter 仍在 ~117 (大于 95) -> 违规! 复位!
         */
        UART_SendStr(">>> Phase 1: TOO EARLY Demo <<<\r\n");
        UART_SendStr("Loop 1~29: wait 30ms, feed in window -> OK\r\n");
        UART_SendStr("Loop 30:   feed IMMEDIATELY -> TOO EARLY! RESET!\r\n");
        UART_SendStr("\r\n");

        uint32_t loopCount = 0;
        while (1)
        {
            loopCount++;

            if (loopCount <= 29)
            {
                /*
                 * 正常喂狗:
                 * 等 30ms -> counter 从 127 减约 33 个 tick -> ~94
                 * 94 在窗口 [64, 95] 内 -> 喂狗成功!
                 */
                Delay_ms(30);
                WWDG_FeedDog();
                UART_SendStr("[Loop ");
                UART_SendDec(loopCount);
                UART_SendStr("] wait 30ms -> counter~94 -> Fed OK!\r\n");
            }
            else if (loopCount == 30)
            {
                /*
                 * 故意过早喂狗:
                 * 不等 -> counter 还在 ~117
                 * 117 > 窗口值 95 -> 窗口违规 -> 复位!
                 */
                UART_SendStr("[Loop ");
                UART_SendDec(loopCount);
                UART_SendStr("] NO delay! counter~117 > window=95\r\n");
                UART_SendStr("     -> Feeding TOO EARLY! RESET!\r\n");

                WWDG_FeedDog();  /* 这一行触发复位! */

                /* 以下代码不会执行 (复位太快) */
                while (1);
            }
        }
    }
}
