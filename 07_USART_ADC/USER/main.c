#include "stm32f10x.h"
#include "usart.h"
#include "adc.h"

/*
 * adc_value:          由 ADC1_2_IRQHandler 更新的 ADC 转换结果
 * adc_conversion_done:中断置 1，主循环检测后清零
 */
extern volatile uint16_t adc_value;
extern volatile uint8_t  adc_conversion_done;

/*
 * ============================================================
 *  main() - ADC 单通道中断转换实验
 * ============================================================
 *
 *  接线:
 *    PA1 ← 模拟信号 (接 3.3V / GND / 电位器中间脚)
 *    PA9 → USB-TTL RX (串口输出)
 *
 *  流程:
 *    软件触发 → ADC 采样+转换 → EOC 中断 → ISR 读值
 *    → 主循环打印 → 延时 → 重复
 */
int main(void)
{
    /* ====== 第 1 步: 初始化串口 ====== */
    UART1_Configuration();

    UART_SendStr("\r\n");
    UART_SendStr("========================================\r\n");
    UART_SendStr("  ADC Interrupt Conversion Demo\r\n");
    UART_SendStr("  Channel: ADC1_CH1 (PA1)\r\n");
    UART_SendStr("  Mode: Single + EOC Interrupt\r\n");
    UART_SendStr("========================================\r\n\r\n");

    /* ====== 第 2 步: 初始化 ADC ====== */
    ADC_GPIO_Config();
    ADC1_Mode_Config();

    UART_SendStr("ADC initialized. Starting conversion...\r\n\r\n");

    /* ====== 第 3 步: 主循环 ====== */
    uint32_t count = 0;

    while (1)
    {
        /* 软件触发一次单次转换 */
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);

        /* 等中断把 adc_conversion_done 置 1 */
        while (adc_conversion_done == 0);
        adc_conversion_done = 0;

        /* 打印结果 */
        count++;
        UART_SendStr("[");
        if (count < 10) UART_SendByte(' ');
        UART_SendDec(count);
        UART_SendStr("] ADC Value: ");
        UART_SendDec(adc_value);

        /* 换算电压 */
        {
            uint32_t voltage_mv = (uint32_t)adc_value * 3300 / 4096;
            UART_SendStr("  (");
            UART_SendDec(voltage_mv / 1000);
            UART_SendByte('.');
            UART_SendDec(voltage_mv % 1000 / 100);
            UART_SendDec(voltage_mv % 100  / 10);
            UART_SendStr(" V)");
        }

        UART_SendStr("\r\n");

        /* 延时 500ms */
        for (uint32_t i = 0; i < 500 * 8000; i++) __NOP();
    }
}
