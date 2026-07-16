#include "usart.h"

/**
  * @brief  初始化 USART1: PA9(TX) + PA10(RX), 波特率 115200
  * @param  无
  * @retval 无
  * @note   使用 PCLK2=72MHz 作为时钟源
  */
void UART1_Configuration(void)
{
    /* ---------- GPIO 配置 ---------- */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA9 = TX (复用推挽输出)
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;   // 复用推挽
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PA10 = RX (浮空输入)
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ---------- USART1 配置 ---------- */
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate            = 115200;
    USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits            = USART_StopBits_1;
    USART_InitStruct.USART_Parity              = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStruct);

    USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  重定向 fputc → USART1，使 printf 可用
  * @note   勾选 Keil → Target → Use MicroLIB（或实现 __use_no_semihosting）
  */
int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint8_t)ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return ch;
}

/**
  * @brief  检查复位来源并通过串口报告
  * @param  无
  * @retval 无
  * @note   检查完毕后必须清除标志位，否则下次复位后无法区分
  *
  *    ┌──────────────────────────────────┐
  *    │ 检测流程：                        │
  *    │  a) 读 RCC_GetFlagStatus 查标志   │
  *    │  b) 打印对应的复位原因字符串      │
  *    │  c) 清除所有已读标志位            │
  *    └──────────────────────────────────┘
  */
void CheckAndReportResetSource(void)
{
    printf("\r\n========== Reset Source ==========\r\n");

    /*
     * 关键改进：用独立 if 代替 if...else if，确保不遗漏任何标志位。
     * 多个复位标志可能同时为 1（如：POR + IWDG 先后发生），
     * 全部检查完后再统一清除，实现完整的"二阶诊断"。
     */

    /* ① 先读：独立检查所有标志 */
    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
        printf("[!] IWDG Reset Detected\r\n");

    if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        printf("[!] NRST Pin Reset Detected\r\n");

    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        printf("[!] Power-On Reset Detected\r\n");

    if (RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET)
        printf("[!] Software Reset Detected\r\n");

    /* ② 后清：全部检查完后统一清除 */
    RCC_ClearFlag();

    printf("==================================\r\n\r\n");
}
