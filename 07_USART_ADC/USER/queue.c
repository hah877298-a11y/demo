#include "queue.h"
#include <stdio.h>

/*
 * ┌─────────────────────────────────────────────────────┐
 * │          健康汇报队列的设计原理                     │
 * ├─────────────────────────────────────────────────────┤
 * │                                                     │
 * │  不会喂狗的任务 ←─ 只汇报                           │
 * │  会喂狗的函数   ←─ 只检查                           │
 * │                                                     │
 * │  裁判模式：                                         │
 * │  "我不信任何一个任务说自己健康，我要亲眼检查"       │
 * │                                                     │
 * │  流程：                                             │
 * │  ① 每个任务周期性调用 Queue_ReportAlive() 刷新心跳  │
 * │  ② 主循环周期性调用 Queue_CheckAllAlive() 递减心跳  │
 * │  ③ 若某任务心跳归零 → 该任务已死 → 不喂狗 → 复位    │
 * │                                                     │
 * └─────────────────────────────────────────────────────┘
 */

/* 全局任务健康表 */
static TaskHealth tasks[MAX_TASKS];

/**
  * @brief  初始化健康队列
  */
void Queue_Init(void)
{
    for (uint8_t i = 0; i < MAX_TASKS; i++)
    {
        tasks[i].heartbeat = 0;
        tasks[i].name      = "Unused";
        tasks[i].alive     = 0;
    }

    /*
     * 注册三个演示任务 —— 在真实项目中这些来自不同模块
     */
    tasks[0].name = "SensorTask";    // 传感器采集
    tasks[1].name = "DataProc";      // 数据处理
    tasks[2].name = "CommTask";      // 通信发送
}

/**
  * @brief  某个任务报告"我还活着"
  * @param  task_id: 任务编号 (0 ~ MAX_TASKS-1)
  * @note   任务在自己的循环中调用此函数，刷新心跳计数
  */
void Queue_ReportAlive(uint8_t task_id)
{
    if (task_id < MAX_TASKS && tasks[task_id].name[0] != 'U')
    {
        tasks[task_id].heartbeat = HEARTBEAT_TIMEOUT;
        tasks[task_id].alive     = 1;
    }
}

/**
  * @brief  裁判检查：所有注册任务是否都存活？
  * @retval 1 = 全部存活，可以喂狗
  *         0 = 至少有一个任务挂了，拒绝喂狗
  * @note   每调用一次，所有任务的心跳计数 -1
  *         心跳降到 0 则判定为死亡
  */
uint8_t Queue_CheckAllAlive(void)
{
    uint8_t allAlive = 1;

    for (uint8_t i = 0; i < MAX_TASKS; i++)
    {
        /* 只检查已注册的任务 */
        if (tasks[i].name[0] == 'U')
            continue;

        if (tasks[i].heartbeat > 0)
        {
            tasks[i].heartbeat--;  // 心跳衰减
        }

        if (tasks[i].heartbeat == 0)
        {
            tasks[i].alive = 0;    // 标记死亡
            allAlive = 0;          // 裁判：不合格！
        }
    }

    return allAlive;
}

/**
  * @brief  调试打印：输出所有任务的心跳状态
  */
void Queue_DebugPrint(void)
{
    printf("--- Task Health Report ---\r\n");
    for (uint8_t i = 0; i < MAX_TASKS; i++)
    {
        if (tasks[i].name[0] == 'U')
            continue;

        printf("  [%s] heartbeat=%lu  %s\r\n",
               tasks[i].name,
               tasks[i].heartbeat,
               tasks[i].alive ? "ALIVE" : "DEAD!");
    }
    printf("--------------------------\r\n");
}
