#ifndef __QUEUE_H
#define __QUEUE_H

#include "stm32f10x.h"

/* 最大注册的任务数 */
#define MAX_TASKS  4

/* 任务心跳超时阈值（主循环轮询次数） */
#define HEARTBEAT_TIMEOUT  5

/*
 * 每个任务注册一个心跳槽位：
 *   - heartbeat : 该任务的"心跳计数器"，由任务自己周期性置位
 *   - name      : 调试用的任务名
 *   - alive     : 是否存活（由裁判判定）
 */
typedef struct
{
    uint32_t heartbeat;
    const char *name;
    uint8_t  alive;
} TaskHealth;

void  Queue_Init(void);
void  Queue_ReportAlive(uint8_t task_id);
uint8_t Queue_CheckAllAlive(void);
void  Queue_DebugPrint(void);

#endif
