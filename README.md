# STM32F103 软件仿真与学习笔记

本项目用于记录 STM32F103 的各模块开发与纯软件仿真实验。无需实体开发板，全流程基于 Keil 仿真器实现。

## 📁 项目目录说明

### 1. [01_UART_String](./01_UART_String)
- **简介**：STM32 串口字符串收发实验。
## 🛠️ 硬件环境
- 开发板：STM32F103C8T6 (Blue Pill)
- 烧录器：ST-Link V2
- 串口模块：CH340 USB 转 TTL

## 📈 运行结果
按下板子上的 Reset 键后，电脑串口助手成功接收到数据：

![Hello World 运行截图](./hello_world.png)

### 2. [02_Timer_PWM](./02_Timer_PWM)
- **简介**：基于 TIM4 的 4 通道 PWM 输出仿真。
- **仿真运行步骤**：
  1. 使用 Keil MDK 打开 `02_Timer_PWM/MDK-ARM/demo.uvprojx`。
  2. 点击编译（Build），然后点击红色 `d` 图标进入 Debug 模式。
  3. 提示：工程已内置 `debug.ini` 脚本，自动解锁了 `error 65: access violation` 内存限制。
  4. 打开 `Logic Analyzer` 窗口，按下 `F5` 全速运行。
  5. 在逻辑分析仪中通过**鼠标滚轮缩小时间轴**，即可观察到 4 路极其完美的 50% 占空比 PWM 方波。

## 🛠️ 环境配置
- 开发工具：Keil uVision5 MDK
- 编译器：ARMCC (Compiler V5)
- 仿真芯片：STM32F103C8/RC 系列

# 3. [03_Key_EXTI](./03_Key_EXTI)

## 实验描述
这是一个基于 STM32F103 的按键中断实验项目。通过配置 EXTI (外部中断) 来实现按键检测，从而翻转板载 LED 的状态。

## 实现功能
- **EXTI 中断配置**：使用 PB4 引脚作为外部中断输入，配置为下降沿触发。
- **LED 控制**：控制 PC13 引脚连接的 LED 进行状态翻转。
- **避坑总结**：
    - 解决了 PB4 引脚作为 JTAG 接口的重映射问题 (`GPIO_Remap_SWJ_JTAGDisable`)。
    - 解决了 RCC 时钟配置错误，确保外设正常工作。
    - 实现了按键中断逻辑，并正确添加了中断标志位清除。

## 硬件连接
- **MCU**: STM32F103C8T6 (Blue Pill)
- **按键**: 连接至 PB4 和 GND
- **LED**: 板载 PC13 (低电平点亮)

## 开发环境
- **IDE**: VS Code + Keil MDK
- **语言**: C语言 (STM32 标准外设库)

## 如何使用
1. 使用 Keil 或 VS Code 打开工程。
2. 确保 BOOT0 跳线已正确设置 (Flash 启动)。
3. 编译并下载代码到开发板。
4. 按下按键，观察 LED 是否成功翻转。

# 4. [04__DMA_M2M](./04__DMA_M2M)

## 实验描述
这是一个基于 **STM32F103** 的学习实验，演示了 DMA (直接存储器存取) 在内存到内存 (Memory-to-Memory) 模式下的数据传输，以及外部中断 (EXTI) 和 GPIO 的基本控制逻辑。

## 实现功能

*   **DMA 内存传输**: 使用 DMA1 通道 1 实现从源缓冲区 (`SRC_Buffer`) 到目的缓冲区 (`DST_Buffer`) 的自动数据拷贝。
*   **数据验证**: 实现 `Buffercmp` 函数用于校验传输数据的完整性。
*   **外部中断控制**: 配置 EXTI 线路 4 (PB4 引脚) 作为触发源，通过中断实现 LED 的状态翻转。
*   **状态指示**: 通过 PC13 引脚控制 LED 灯，根据 DMA 传输是否成功直观地反馈系统状态。

## 开发环境

*   **IDE**: Keil MDK (uVision5)
*   **硬件平台**: STM32F103 系列微控制器
*   **库版本**: STM32F10x 标准外设库 (Standard Peripheral Library)



