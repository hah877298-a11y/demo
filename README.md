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
