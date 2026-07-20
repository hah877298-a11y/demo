#include "usart.h"

/*
 * ============================================================
 *  UART1_Configuration - 初始化 USART1 (直接寄存器操作)
 * ============================================================
 *
 *  为什么用寄存器而不是库函数？
 *  因为直接操作寄存器已验证可以工作，避免 printf/MicroLIB 的依赖问题。
 *
 *  配置: PA9 = TX, 115200-8-N-1
 *  系统时钟: PCLK2 = 72MHz (由 SystemInit 配置)
 */
void UART1_Configuration(void)
{
    /* 1. 使能 GPIOA + USART1 时钟 */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;

    /* 2. PA9 = 50MHz 复用推挽输出 */
    GPIOA->CRH &= ~(0xF << 4);      // 清除 PA9 配置位
    GPIOA->CRH |=  (0xB << 4);      // 0b1011 = 50MHz AF Push-Pull

    /* 3. 波特率 = 115200 */
    /*
     * USARTDIV = PCLK2 / (16 * BaudRate)
     *          = 72,000,000 / (16 * 115200)
     *          = 39.0625
     *   整数部分 = 39 (DIV_Mantissa)
     *   小数部分 = 0.0625 * 16 = 1 (DIV_Fraction)
     */
    USART1->BRR = (39 << 4) | 1;

    /* 4. 使能 USART (TX + RX) */
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE;
    USART1->CR1 |= USART_CR1_UE;
}

/*
 * ============================================================
 *  UART_SendByte - 发送一个字节（阻塞）
 * ============================================================
 */
void UART_SendByte(uint8_t byte)
{
    while (!(USART1->SR & USART_SR_TXE));   // 等 TXE: 发送寄存器空
    USART1->DR = byte;                       // 写入数据
}

/*
 * ============================================================
 *  UART_SendStr - 发送字符串
 * ============================================================
 */
void UART_SendStr(const char *str)
{
    while (*str)
    {
        UART_SendByte((uint8_t)(*str++));
    }
}

/*
 * ============================================================
 *  UART_SendDec - 发送十进制无符号整数
 * ============================================================
 */
void UART_SendDec(uint32_t num)
{
    char buf[12];          // 最大 4294967295 = 10 位 + 结尾
    uint8_t i = 0;

    if (num == 0)
    {
        UART_SendByte('0');
        return;
    }

    while (num > 0)
    {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }

    /* 反转输出（个位在前，需要倒过来） */
    while (i > 0)
    {
        UART_SendByte(buf[--i]);
    }
}

/*
 * ============================================================
 *  UART_SendHex - 发送十六进制（固定 2 位）
 * ============================================================
 */
void UART_SendHex(uint8_t num)
{
    static const char hex[] = "0123456789ABCDEF";
    UART_SendByte(hex[(num >> 4) & 0x0F]);
    UART_SendByte(hex[num & 0x0F]);
}

/*
 * ============================================================
 *  CheckAndReportResetSource - 检查并报告复位来源
 * ============================================================
 */
void CheckAndReportResetSource(void)
{
    UART_SendStr("\r\n========== Reset Source ==========\r\n");

    if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
        UART_SendStr("[!] WWDG Reset Detected\r\n");

    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
        UART_SendStr("[!] IWDG Reset Detected\r\n");

    if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        UART_SendStr("[!] NRST Pin Reset Detected\r\n");

    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        UART_SendStr("[!] Power-On Reset Detected\r\n");

    if (RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET)
        UART_SendStr("[!] Software Reset Detected\r\n");

    RCC_ClearFlag();

    UART_SendStr("==================================\r\n\r\n");
}
