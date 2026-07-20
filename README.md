# STM32F103 软件仿真与学习笔记

本项目用于记录 STM32F103 的各模块开发与纯软件仿真实验。无需实体开发板，全流程基于 Keil 仿真器实现。

## 📁 实验目录说明

### 1. [01_UART_String](./01_UART_String)
 **简介**：STM32 串口字符串收发实验。
## 🛠️ 硬件环境
 开发板：STM32F103C8T6 (Blue Pill)
 烧录器：ST-Link V2
 串口模块：CH340 USB 转 TTL

## 📈 运行结果
按下板子上的 Reset 键后，电脑串口助手成功接收到数据：

![Hello World 运行截图](./hello_world.png)

### 2. [02_Timer_PWM](./02_Timer_PWM)
 **简介**：基于 TIM4 的 4 通道 PWM 输出仿真。
 **仿真运行步骤**：
  1. 使用 Keil MDK 打开 `02_Timer_PWM/MDK-ARM/demo.uvprojx`。
  2. 点击编译（Build），然后点击红色 `d` 图标进入 Debug 模式。
  3. 提示：工程已内置 `debug.ini` 脚本，自动解锁了 `error 65: access violation` 内存限制。
  4. 打开 `Logic Analyzer` 窗口，按下 `F5` 全速运行。
  5. 在逻辑分析仪中通过**鼠标滚轮缩小时间轴**，即可观察到 4 路极其完美的 50% 占空比 PWM 方波。

## 🛠️ 环境配置
 开发工具：Keil uVision5 MDK
 编译器：ARMCC (Compiler V5)
 仿真芯片：STM32F103C8/RC 系列

# 3. [03_Key_EXTI](./03_Key_EXTI)

## 实验描述
这是一个基于 STM32F103 的按键中断实验项目。通过配置 EXTI (外部中断) 来实现按键检测，从而翻转板载 LED 的状态。

## 实现功能
 **EXTI 中断配置**：使用 PB4 引脚作为外部中断输入，配置为下降沿触发。
 **LED 控制**：控制 PC13 引脚连接的 LED 进行状态翻转。
 **避坑总结**：
    - 解决了 PB4 引脚作为 JTAG 接口的重映射问题 (`GPIO_Remap_SWJ_JTAGDisable`)。
    - 解决了 RCC 时钟配置错误，确保外设正常工作。
    - 实现了按键中断逻辑，并正确添加了中断标志位清除。

## 硬件连接
 **MCU**: STM32F103C8T6 (Blue Pill)
 **按键**: 连接至 PB4 和 GND
 **LED**: 板载 PC13 (低电平点亮)

## 开发环境
 **IDE**: VS Code + Keil MDK
 **语言**: C语言 (STM32 标准外设库)

## 如何使用
1. 使用 Keil 或 VS Code 打开工程。
2. 确保 BOOT0 跳线已正确设置 (Flash 启动)。
3. 编译并下载代码到开发板。
4. 按下按键，观察 LED 是否成功翻转。

# 4. [04__DMA_M2M](./04__DMA_M2M)

## 实验描述
这是一个基于 **STM32F103** 的学习实验，演示了 DMA (直接存储器存取) 在内存到内存 (Memory-to-Memory) 模式下的数据传输，以及外部中断 (EXTI) 和 GPIO 的基本控制逻辑。

## 实现功能

   **DMA 内存传输**: 使用 DMA1 通道 1 实现从源缓冲区 (`SRC_Buffer`) 到目的缓冲区 (`DST_Buffer`) 的自动数据拷贝。
   **数据验证**: 实现 `Buffercmp` 函数用于校验传输数据的完整性。
   **外部中断控制**: 配置 EXTI 线路 4 (PB4 引脚) 作为触发源，通过中断实现 LED 的状态翻转。
   **状态指示**: 通过 PC13 引脚控制 LED 灯，根据 DMA 传输是否成功直观地反馈系统状态。

## 开发环境

   **IDE**: Keil MDK (uVision5)
   **硬件平台**: STM32F103 系列微控制器
   **库版本**: STM32F10x 标准外设库 (Standard Peripheral Library)

# 5. [05_USART_IWDG](./05_USART_IWDG)

## 实验描述
这是一个基于 STM32F103 的独立看门狗（IWDG）与 USART 串口调试的综合实验。
 **IWDG 驱动**：预分频=64，重装载=625，溢出时间≈1s，配置后不可关闭。
 **复位诊断**：启动时检测 RCC 复位标志位，通过串口报告是上电复位还是看门狗复位。
 **任务心跳队列**：裁判模式 — 每个任务只汇报心跳，由统一裁判决定是否喂狗，防止"看门狗被绑架"。
 **故障注入验证**：注释掉某个任务的汇报函数，亲眼观察看门狗复位 + 串口打印死因报告。

## 硬件连接
 **MCU**: STM32F103C8T6 (Blue Pill)
 **烧录器**: ST-Link V2 (SWCLK/SWDIO/GND/3.3V)
 **串口模块**: USB-to-TTL (PA9→RX, PA10→TX, GND→GND)，波特率 115200

## 开发环境
 **IDE**: Keil MDK (uVision5)，需勾选 Use MicroLIB
 **语言**: C 语言 (STM32 标准外设库 V3.5.0)

## 结果
[17:27:20.541]收←◆
===== IWDG Demo (with Health-Check) =====

========== Reset Source ==========
[!] NRST Pin Reset Detected
==================================

Health-Queue initialized. 3 tasks registered.

IWDG started. Timeout = ~1 second.
Dog will be fed ONLY if ALL tasks report alive.

[Loop 1] All tasks ALIVE -> Dog fed!

[17:27:20.836]收←◆[Loop 2] All tasks ALIVE -> Dog fed!

[17:27:21.105]收←◆[Loop 3] All tasks ALIVE -> Dog fed!

...

CommTask_Sim() 里的 Queue_ReportAlive(2) 注释掉
[17:31:50.200]收←◆
===== IWDG Demo (with Health-Check) =====

========== Reset Source ==========
[!] IWDG Reset Detected
[!] NRST Pin Reset Detected
==================================

Health-Queue initialized. 3 tasks registered.

IWDG started. Timeout = ~1 second.
Dog will be fed ONLY if ALL tasks report alive.

[Loop 1] !! TASK DEAD! Refusing to feed dog...
--- Task Health Report ---
  [SensorTask] heartbeat=4  ALIVE
  [DataProc] heartbeat=4  ALIVE
  [CommTask] heartbeat=0  DEAD!
--------------------------

# 6. [06_USART_WWDG](./06_USART_WWDG)

## 实验描述
这是一个基于 STM32F103 的窗口看门狗（WWDG）与 USART 串口调试的综合实验。与 IWDG（独立看门狗）不同，WWDG 必须在**精确的时间窗口内**喂狗——喂早了不行，喂晚了也不行。

## WWDG vs IWDG 核心区别

| 特性     |      IWDG         |            WWDG           |
| 时钟源   | 独立 LSI (~40kHz) |     APB1 (PCLK1=36MHz)     |
| 喂狗规则 | 超时前喂就行       | 必须在 [64, 窗口值] 区间内喂 |
| 计数器   | 12 位递减         |          7 位递减          |
| 窗口检测 |       无          |         有（喂早了也复位）  |
| 早期中断 |       无          | EWI（counter=0x40 时触发） |
| 能否关闭 |   启动后不可关     |        启动后不可关        |

## 实验参数
分频系数 = 8
计数器初值 = 0x7F (127)
窗口值 = 0x5F (95)
计数器时钟 = 36MHz / 4096 / 8 = 1099 Hz
每 tick ≈ 910 us

127 ---> 95 ---> 64 ---> 63 = 复位!
|<-- 禁止喂狗 -->|<-- 允许喂狗窗口 -->|<-- 太晚 -->|
     ~29 ms           ~28 ms


## 实现功能

- **WWDG 驱动**：预分频=8，窗口值=0x5F，计数器=0x7F，总超时≈58ms
- **EWI 早期唤醒中断**：计数器减到 0x40 时触发，距离复位仅剩 910us
- **两阶段自动演示**：
  - 阶段 1（首次上电 POR）：正常喂狗 29 次 → 第 30 次故意过早喂狗 → WWDG 复位
  - 阶段 2（WWDG 复位后）：完全不喂狗 → counter 自然减到 0x3F → WWDG 复位
- **复位源检测**：通过 RCC CSR 寄存器的 WWDGRSTF 标志位识别 WWDG 复位
- **自实现串口函数**：`UART_SendStr/SendDec/SendHex` 直接寄存器操作，不依赖 MicroLIB/printf

## 避坑总结

- **printf 顺序陷阱**：`WWDG_Enable()` 必须在 `printf` 之后调用，否则打印期间看门狗就超时了
- **EWI 中断不能 printf**：EWI 到复位只有 910us，printf 一个字符就要 87us，根本来不及。中断中只能做最轻量的操作（设置标志位）
- **"Connect under Reset"**：WWDG 复位循环会阻止调试器连接，需在 Debug 设置中勾选该选项
- **BOOT0 救砖**：如果"Connect under Reset"也不行，把 BOOT0 拉高后上电即可绕过用户程序

## 硬件连接

- **MCU**: STM32F103C8T6 (Blue Pill)
- **烧录器**: ST-Link V2 (SWCLK/SWDIO/GND/3.3V)
- **串口模块**: USB-to-TTL (PA9→RX, GND→GND)，波特率 115200
- **其他**: 不需要 LED、按键等外设

## 开发环境

- **IDE**: Keil MDK (uVision5)
- **语言**: C 语言 (STM32 标准外设库 V3.5.0)
- **不需要** MicroLIB（使用自实现的寄存器直操作串口函数）

## 结果

**阶段 1（首次上电）：**

========================================
  WWDG Window Watchdog Demo
========================================

========== Reset Source ==========
[!] Power-On Reset Detected
==================================

WWDG Config OK:
  Prescaler = 8 (counter clock = 1099 Hz)
  Window    = 0x5F (95)
  Counter   = 0x7F (127)
  Timeout   = ~58 ms, window = 64~95 (~28 ms)

>>> Phase 1: TOO EARLY Demo <<<
Loop 1~29: wait 30ms, feed in window -> OK
Loop 30:   feed IMMEDIATELY -> TOO EARLY! RESET!

[Loop 1] wait 30ms -> counter~94 -> Fed OK!
[Loop 2] wait 30ms -> counter~94 -> Fed OK!
...
[Loop 29] wait 30ms -> counter~94 -> Fed OK!
[Loop 30] NO delay! counter~117 > window=95
     -> Feeding TOO EARLY! RESET!
```

**阶段 2（WWDG 复位后自动进入）：**


========================================
  WWDG Window Watchdog Demo
========================================

========== Reset Source ==========
[!] WWDG Reset Detected
[!] NRST Pin Reset Detected
==================================

>>> Phase 2: TOO LATE Demo <<<
I will NOT feed the watchdog.
Counter: 127 -> 64(EWI!) -> 63(RESET!)

[Phase 2] tick=50000, NOT feeding...
!!! EWI! System will reset now!

# 7. [07_USART_ADC](./07_USART_ADC)

## 实验描述
这是一个基于 STM32F103 的单通道 ADC 中断转换实验。通过配置 ADC1 通道 1（PA1）实现模拟信号采集，每次转换完成后触发 EOC 中断，在 ISR 中读取转换结果并通过串口打印。

## 实现功能

 **ADC 中断采集**：配置 ADC1_CH1（PA1）为模拟输入，单次转换 + 软件触发模式
 **EOC 中断**：每次转换完成后硬件置 EOC 标志，触发 `ADC1_2_IRQHandler` 读取 `ADC_DR` 并保存到全局变量
 **ADC 校准**：上电后执行复位校准 + ADC 校准，确保读数准确（注意：ADC 时钟必须 ≤ 14MHz，设置 PCLK2/6=12MHz）
 **电压换算**：`voltage_mv = adc_value * 3300 / 4096`，12 位 ADC 量程 0~4095 对应 0~3.3V
 **串口输出**：自实现寄存器直操作串口函数，不依赖 MicroLIB/printf

## 避坑总结

 **ADCCLK 超频陷阱**：`SystemInit()` 默认 ADCPRE=0（PCLK2/2=36MHz），远超 ADC 最大 14MHz，会导致**校准卡死**或转换无响应，必须显式调用 `RCC_ADCCLKConfig(RCC_PCLK2_Div6)`
 **中断 vs 轮询**：EOC 中断方式比轮询 `ADC_GetFlagStatus` 更高效，CPU 只在转换完成时介入一次
 **校准顺序不能错**：`ADC_Cmd(ENABLE)` → 等待稳定 → `ADC_ResetCalibration` → `ADC_StartCalibration`，上电后必须先校准

## 硬件连接

 **MCU**: STM32F103C8T6 (Blue Pill)
 **模拟输入**: PA1 ← 3.3V / GND / 电位器中间脚- **串口模块**: USB-to-TTL (PA9→RX, GND→GND)，波特率 115200

## 开发环境

 **IDE**: Keil MDK (uVision5)
 **语言**: C 语言 (STM32 标准外设库 V3.5.0)
 **不需要** MicroLIB（使用自实现的寄存器直操作串口函数）

## 结果


```
# ========================================
ADC Interrupt Conversion Demo
Channel: ADC1_CH1 (PA1)
Mode: Single + EOC Interrupt
ADC initialized. Starting conversion...
[ 1] ADC Value: 4095  (3.29 V)
[ 2] ADC Value: 4095  (3.29 V)
[ 3] ADC Value: 4094  (3.29 V)
```

PA1 接 3.3V → 读数 ~4095（满量程）；接 GND → 读数 ~0；接电位器 → 读数随旋钮线性变化。


# 8. [08_DMA_ADC](./08_DMA_ADC)

## 实验描述
在实验 7 的基础上，从"单通道中断"升级为"多通道扫描 + DMA 自动搬运"。ADC1 以扫描模式依次转换 CH1/CH2/CH3（PA1/PA2/PA3），DMA1_Channel1 以循环模式将每次转换结果自动搬运到内存数组 `adc_buffer[3]`，CPU 无需参与数据搬运。

## 核心原理

### 为什么要用 DMA？
ADC_DR 数据寄存器只有 **1 个**。扫描模式下 3 个通道依次转换，新数据会覆盖旧数据。如果 CPU 不及时来读，前一个通道的转换结果就丢失了。DMA 在每个通道转换完成瞬间（EOC 信号）自动从 ADC_DR 搬到内存，全程硬件完成，不打断 CPU。

### 数据流

```
PA1→CH1 ─┐
PA2→CH2 ─┤  ADC 扫描  ──→  ADC_DR  ──→  DMA1_Ch1  ──→  adc_buffer[0/1/2]
PA3→CH3 ─┘  (连续)         (固定地址)     (外设→内存)     (递增地址, 循环)
```

## 实现功能

 **GPIO 配置**：PA1/PA2/PA3 全部设为模拟输入 (`GPIO_Mode_AIN`)
 **ADC 扫描模式**：`ScanConvMode=ENABLE`，`ContinuousConvMode=ENABLE`，`NbrOfChannel=3`
 **DMA 循环搬运**：`DMA_Mode_Circular`，源地址 `&ADC1->DR`（固定），目的地址 `adc_buffer`（递增），每轮搬 3 次自动循环
 **上电校准**：正确的初始化顺序 — `RCC_AHB 开时钟` → `DMA_StructInit` → 配置参数 → `DMA_Init` → `ADC_Cmd` → 等稳定 → `校准` → `ADC_DMACmd` → `DMA_Cmd` → `ADC_SoftwareStartConvCmd`
 **主循环零等待**：直接读取 `adc_buffer[0/1/2]` 即可，无需等待中断标志位
 **易扩展**：使用 `#define ADC_CHANNEL_COUNT 3` 宏，增减通道只需改一个数字

## 避坑总结

 **时钟必须先开**：`RCC_AHBPeriphClockCmd(DMA1)` 必须在 `DMA_StructInit` 之前调用，否则写 DMA 寄存器无效
 **DMA 启动时机**：`DMA_Cmd` 和 `ADC_SoftwareStartConvCmd` 必须放在 ADC 校准完成之后，不能在 ADC 校准过程中启动搬运
 **DMA_Mode_Circular 不可少**：用 `DMA_Mode_Normal` 会在搬完一轮后停转，ADC 还在转换但数据不再更新
 **数组类型**：必须用 `uint16_t` 而非 `uint8_t`，ADC 12 位结果范围 0~4095，`uint8_t` 存不下

## 硬件连接

 **MCU**: STM32F103C8T6 (Blue Pill)
 **模拟输入**: PA1/PA2/PA3 ← 分别接 3.3V / GND / 悬空或电位器
 **串口模块**: USB-to-TTL (PA9→RX, GND→GND)，波特率 115200

## 开发环境

 **IDE**: Keil MDK (uVision5)
 **语言**: C 语言 (STM32 标准外设库 V3.5.0)
 **不需要** MicroLIB（使用自实现的寄存器直操作串口函数）

## 结果


```
# ========================================
ADC Multi-Channel Scan + DMA Demo
CH1=PA1  CH2=PA2  CH3=PA3
Mode: Scan + Continuous + DMA Circular
ADC initialized. Starting conversion...
[ 1] CH1=4095 CH2=0 CH3=1138  (3.29 V)
[ 2] CH1=4095 CH2=0 CH3=1137  (3.29 V)
[ 3] CH1=4095 CH2=0 CH3=1135  (3.29 V)
```

PA1 接 3.3V → ~4095，PA2 接 GND → ~0，PA3 悬空 → 随机浮空值。三通道数据每 500ms 自动刷新一轮。
