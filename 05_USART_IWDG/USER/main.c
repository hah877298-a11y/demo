#include "stm32f10x.h"
#include "usart.h"
#include "iwdg.h"
#include "queue.h"

/* 简单的软件延时（近似值） */
void Delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * 8000; i++)
    {
        __NOP();
    }
}

/*
 * 模拟三个独立的"任务"（裸机下用函数模拟，RTOS 中它们就是独立线程）
 */
void SensorTask_Sim(void)
{
    /* 传感器采集... */
    Queue_ReportAlive(0);  // 报告：我还活着！
}

void DataProc_Sim(void)
{
    /* 数据处理... */
    Queue_ReportAlive(1);
}

void CommTask_Sim(void)
{
    /* 通信发送... */
   // Queue_ReportAlive(2);
}

int main(void)
{
    /* ────────── 第1步：初始化串口 ────────── */
    UART1_Configuration();
    printf("\r\n===== IWDG Demo (with Health-Check) =====\r\n");

    /* ────────── 第2步：检查复位来源 ────────── */
    CheckAndReportResetSource();

    /* ────────── 第3步：初始化健康队列 ────────── */
    Queue_Init();
    printf("Health-Queue initialized. %d tasks registered.\r\n\r\n", 3);

    /* ────────── 第4步：启动看门狗 ────────── */
    IWDG_Configuration();
    printf("IWDG started. Timeout = ~1 second.\r\n");
    printf("Dog will be fed ONLY if ALL tasks report alive.\r\n\r\n");

    uint32_t loopCount = 0;

    /* ────────── 第5步：主循环 ────────── */
    while (1)
    {
        loopCount++;

        /*
         * 模拟 3 个任务各自运行并汇报健康状况
         */
        SensorTask_Sim();
        DataProc_Sim();
        CommTask_Sim();

        /*
         * 裁判模式：检查所有任务心跳
         *  ┌──────────────┐
         *  │ 全部存活？    │──Yes──→ IWDG_FeedDog()
         *  │              │
         *  │ 有人挂了？    │──No───→ 不喂狗 → 系统复位！
         *  └──────────────┘
         */
        if (Queue_CheckAllAlive())
        {
            IWDG_FeedDog();
            printf("[Loop %lu] All tasks ALIVE -> Dog fed!\r\n", loopCount);
        }
        else
        {
            /*
             * 有人挂了！拒绝喂狗 —— 看门狗将在 ~1 秒后复位系统。
             * 这是一个有意识的决定：宁可系统重启，也不能带着死掉的任务继续跑。
             */
            printf("[Loop %lu] !! TASK DEAD! Refusing to feed dog...\r\n", loopCount);
            Queue_DebugPrint();

            /* 死循环阻塞 —— 演示看门狗复位的效果 */
            while (1);
        }

        Delay_ms(200);  // 200ms 循环周期 < 1s 溢出时间
    }
}
