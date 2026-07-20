#include "stm32f10x.h"
#include "usart.h"
#include "adc.h"

/*
 * adc_buffer: DMA 自动更新的三通道 ADC 结果数组
 *            adc_buffer[0]=CH1(PA1), [1]=CH2(PA2), [2]=CH3(PA3)
 */

/*
 * ============================================================
 *  main() - ADC 三通道扫描 + DMA 自动搬运实验
 * ============================================================
 *
 *  接线:
 *    PA1 ← 模拟信号 CH1 (如接 3.3V)
 *    PA2 ← 模拟信号 CH2 (如接 GND)
 *    PA3 ← 模拟信号 CH3 (如悬空或接电位器)
 *    PA9 → USB-TTL RX (串口输出)
 *
 *  流程:
 *    上电初始化 → ADC 校准 → DMA 启动 → ADC 软件触发首次转换
 *    → ADC 连续扫描 CH1→CH2→CH3 → DMA 每通道自动搬运到 adc_buffer
 *    → 主循环直接读数组 → 打印 → 延时 → 重复
 */
int main(void)
{
    /* ====== 第 1 步: 初始化串口 ====== */
    UART1_Configuration();

    UART_SendStr("\r\n");
    UART_SendStr("========================================\r\n");
    UART_SendStr("  ADC Multi-Channel Scan + DMA Demo\r\n");
    UART_SendStr("  CH1=PA1  CH2=PA2  CH3=PA3\r\n");
    UART_SendStr("  Mode: Scan + Continuous + DMA Circular\r\n");
    UART_SendStr("========================================\r\n\r\n");

    /* ====== 第 2 步: 初始化 ADC ====== */
    ADC_GPIO_Config();
    ADC1_Mode_Config();

    UART_SendStr("ADC initialized. Starting conversion...\r\n\r\n");

    /* ====== 第 3 步: 主循环 ====== */
    uint32_t count = 0;

    while (1)
    {

        /* 打印结果 */
        count++;
        UART_SendStr("[");
        if (count < 10) UART_SendByte(' ');
        UART_SendDec(count);
        UART_SendStr("] CH1=");
        UART_SendDec(adc_buffer[0]);
        UART_SendStr(" CH2=");
        UART_SendDec(adc_buffer[1]);
        UART_SendStr(" CH3=");
        UART_SendDec(adc_buffer[2]);
        /* 换算电压 */
        {
            uint32_t voltage_mv = (uint32_t) adc_buffer[0] * 3300 / 4096;
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
