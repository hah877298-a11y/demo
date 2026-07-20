#include "adc.h"
#define ADC_CHANNEL_COUNT 3
/*
 * ============================================================
 *  adc_buffer[ADC_CHANNEL_COUNT] - 多通道 ADC 结果数组
 *
 *  由 DMA 在后台自动从 ADC_DR 搬运到此处，
 *  主循环直接读取，无需中断参与。
 * ============================================================
 */
volatile uint16_t adc_buffer[ADC_CHANNEL_COUNT] = {0};

/*
 * ============================================================
 *  ADC_GPIO_Config - 配置 PA1/PA2/PA3 为模拟输入
 * ============================================================
 *
 *  ADC1 通道 1 对应的引脚是 PA1。
 *  模拟输入模式让引脚连接到 ADC 模块内部，
 *  而不是数字 GPIO 电路。
 *
 *  同时也使能了 ADC1 的外设时钟。
 */
void ADC_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能 GPIOA 和 ADC1 的时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);

    /*
     * ADC 时钟预分频: PCLK2=72MHz → 6分频 = 12MHz
     *
     * 必须设! SystemInit() 把 ADCPRE 清零了（默认2分频=36MHz），
     * 超过 STM32F103 ADC 最大允许的 14MHz，超频会导致校准失败、
     * 转换不触发、读数异常等随机问题。
     */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    /* PA1/PA2/PA3 = 模拟输入 */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*
 * ============================================================
 *  ADC1_Mode_Config - 配置 ADC1 多通道扫描 + DMA 自动搬运
 * ============================================================
 *
 *  数据流 (全程 CPU 不参与):
 *    ADC 扫描 CH1→CH2→CH3 → 每次 EOC 触发 DMA
 *    → DMA 搬运 ADC_DR → adc_buffer[i] → 循环往复
 *
 *  配置要点:
 *    1. 独立模式 (单 ADC 工作)
 *    2. 扫描模式 (依次转换所有规则组通道)
 *    3. 连续转换 + DMA 循环模式 (永不停歇)
 *    4. 软件触发首次启动
 *    5. 右对齐 (12位数据低12位有效)
 *    6. DMA1_Channel1 外设→内存，半字，循环
 *    7. ADC 校准
 */
void ADC1_Mode_Config(void)
{
    ADC_InitTypeDef   ADC_InitStructure;
    DMA_InitTypeDef   DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    /*
     * ====== 第 1 步: ADC 工作模式配置 ======
     */

    /* 先填充默认值，再修改需要的字段 */
    ADC_StructInit(&ADC_InitStructure);

    ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;       // 独立模式
    ADC_InitStructure.ADC_ScanConvMode       = ENABLE;                     // 多通道，扫描
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                     // 多次转换
    ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;  // 软件触发
    ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;        // 右对齐
    ADC_InitStructure.ADC_NbrOfChannel       = 3;                          // 转换 3 个通道
    ADC_Init(ADC1, &ADC_InitStructure);
    DMA_StructInit(&DMA_InitStructure);
   
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;  // ADC_DR 的地址
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)adc_buffer;  // 数组首地址
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;  // 外设→内存，还是内存→外设？
    DMA_InitStructure.DMA_BufferSize         = ADC_CHANNEL_COUNT;  // 搬几次？
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;  // 外设地址要不要递增？
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;  // 内存地址要不要递增？
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  // 16 位 = HalfWord
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;  // 跟上面一样
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;  // 循环还是单次？
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;  // 不是内存到内存
    
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    /*
     * ====== 第 2 步: 配置通道 1 的采样时间 ======
     *
     * 采样时间越长，信号越稳定，但转换越慢。
     * 55.5 个时钟周期 (约 4.6us @ 12MHz ADCCLK) 是常用折衷值。
     *
     * Rank=1 表示在其所在序列中第一个被转换。
     */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 3, ADC_SampleTime_55Cycles5);

    /*
     * ====== 第 3 步: 中断 ======
     *
     * 使用 DMA 模式，EOC 中断不需要。
     * ADC 每完成一个通道自动触发 DMA 搬运。
     */
    //ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);  // 已禁用

    /*
     * ====== 第 4 步: 使能 ADC 并校准 ======
     *
     * 校准是必须的！未校准的 ADC 读数不准。
     *
     * 注意: 校准前必须确保 ADC 时钟 ≤ 14MHz，
     * 否则校准可能永远不完成导致死循环。
     * ADC_GPIO_Config() 中已设置 ADCCLK=12MHz（PCLK2/6）。
     */
    ADC_Cmd(ADC1, ENABLE);
    
    /* 等待 ADC 上电稳定 (tSTAB ≈ 1μs，这里多等一会) */
    {
        volatile uint32_t t = 10000;
        while (t--);
    }
    
    /* 复位校准（带超时保护） */
    ADC_ResetCalibration(ADC1);
    {
        volatile uint32_t cal_timeout = 100000;
        while (ADC_GetResetCalibrationStatus(ADC1) && --cal_timeout);
    }

    /* 执行校准（带超时保护） */
    ADC_StartCalibration(ADC1);
    {
        volatile uint32_t cal_timeout = 100000;
        while (ADC_GetCalibrationStatus(ADC1) && --cal_timeout);
    }
    
    ADC_DMACmd(ADC1, ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
