# STM32F103 学习笔记

本项目用于记录 STM32F103（Blue Pill）各外设模块的开发实验。所有实验基于 Keil MDK + STM32 标准外设库 V3.5.0。

---

## 1. UART 字符串收发

**项目目录**：[01_UART_String](./01_UART_String)

**实验简介**：STM32 串口字符串收发实验，实现 USART1 与 PC 串口助手的双向通信。

**硬件环境**：

- 开发板：STM32F103C8T6 (Blue Pill)
- 烧录器：ST-Link V2
- 串口模块：CH340 USB 转 TTL

**运行结果**：按下 Reset 键后，串口助手成功接收 Hello World 数据。

![Hello World 运行截图](./hello_world.png)

---

## 2. Timer PWM 输出

**项目目录**：[02_Timer_PWM](./02_Timer_PWM)

**实验简介**：基于 TIM4 的 4 通道 PWM 输出仿真，通过 Keil 逻辑分析仪观察 50% 占空比方波。

**仿真运行步骤**：

1. 使用 Keil MDK 打开 `02_Timer_PWM/MDK-ARM/demo.uvprojx`
2. 点击编译（Build），然后点击红色 `d` 图标进入 Debug 模式
3. 提示：工程已内置 `debug.ini` 脚本，自动解锁了 `error 65: access violation` 内存限制
4. 打开 `Logic Analyzer` 窗口，按下 `F5` 全速运行
5. 在逻辑分析仪中通过**鼠标滚轮缩小时间轴**，即可观察到 4 路 50% 占空比 PWM 方波

**开发环境**：

- 开发工具：Keil uVision5 MDK
- 编译器：ARMCC (Compiler V5)
- 仿真芯片：STM32F103C8/RC 系列

---

## 3. 按键 EXTI 中断

**项目目录**：[03_Key_EXTI](./03_Key_EXTI)

**实验简介**：通过配置 EXTI（外部中断）实现按键检测，翻转板载 LED 状态。

**实现功能**：

- **EXTI 中断配置**：使用 PB4 引脚作为外部中断输入，配置为下降沿触发
- **LED 控制**：控制 PC13 引脚连接的 LED 进行状态翻转
- **避坑总结**：
  - 解决了 PB4 引脚作为 JTAG 接口的重映射问题 (`GPIO_Remap_SWJ_JTAGDisable`)
  - 解决了 RCC 时钟配置错误，确保外设正常工作
  - 实现了按键中断逻辑，并正确添加了中断标志位清除

**硬件连接**：

- MCU: STM32F103C8T6 (Blue Pill)
- 按键: 连接至 PB4 和 GND
- LED: 板载 PC13 (低电平点亮)

**开发环境**：

- IDE: VS Code + Keil MDK
- 语言: C 语言 (STM32 标准外设库)

**使用方法**：

1. 使用 Keil 或 VS Code 打开工程
2. 确保 BOOT0 跳线已正确设置 (Flash 启动)
3. 编译并下载代码到开发板
4. 按下按键，观察 LED 是否成功翻转

---

## 4. DMA 内存到内存传输

**项目目录**：[04__DMA_M2M](./04__DMA_M2M)

**实验简介**：演示 DMA 在内存到内存 (Memory-to-Memory) 模式下的数据传输，以及 EXTI 和 GPIO 的基本控制逻辑。

**实现功能**：

- **DMA 内存传输**：使用 DMA1 通道 1 实现从源缓冲区 (`SRC_Buffer`) 到目的缓冲区 (`DST_Buffer`) 的自动数据拷贝
- **数据验证**：实现 `Buffercmp` 函数用于校验传输数据的完整性
- **外部中断控制**：配置 EXTI 线路 4 (PB4 引脚) 作为触发源，通过中断实现 LED 的状态翻转
- **状态指示**：通过 PC13 引脚控制 LED，根据 DMA 传输是否成功直观反馈系统状态

**开发环境**：

- IDE: Keil MDK (uVision5)
- 硬件平台: STM32F103 系列微控制器
- 库版本: STM32F10x 标准外设库

---

## 5. IWDG 独立看门狗 + 心跳队列

**项目目录**：[05_USART_IWDG](./05_USART_IWDG)

**实验简介**：独立看门狗（IWDG）与 USART 串口调试的综合实验，实现"裁判模式"心跳队列——每个任务只汇报心跳，由统一裁判决定是否喂狗，防止看门狗被单个任务绑架。

**实现功能**：

- **IWDG 驱动**：预分频=64，重装载=625，溢出时间≈1s，配置后不可关闭
- **复位诊断**：启动时检测 RCC 复位标志位，通过串口报告是上电复位还是看门狗复位
- **任务心跳队列**：裁判模式——每个任务只汇报心跳，统一裁判决定是否喂狗
- **故障注入验证**：注释掉某个任务的汇报函数，亲眼观察看门狗复位 + 串口打印死因报告

**硬件连接**：

- MCU: STM32F103C8T6 (Blue Pill)
- 烧录器: ST-Link V2 (SWCLK/SWDIO/GND/3.3V)
- 串口模块: USB-to-TTL (PA9→RX, PA10→TX, GND→GND)，波特率 115200

**运行结果（正常）**：

```text
===== IWDG Demo (with Health-Check) =====
========== Reset Source ==========
[!] NRST Pin Reset Detected
==================================
Health-Queue initialized. 3 tasks registered.
IWDG started. Timeout = ~1 second.
[Loop 1] All tasks ALIVE -> Dog fed!
[Loop 2] All tasks ALIVE -> Dog fed!
...
```

**运行结果（故障注入后）**：

```text
[Loop 1] !! TASK DEAD! Refusing to feed dog...
--- Task Health Report ---
  [SensorTask] heartbeat=4  ALIVE
  [DataProc]   heartbeat=4  ALIVE
  [CommTask]   heartbeat=0  DEAD!
--------------------------
===== IWDG Demo (with Health-Check) =====
========== Reset Source ==========
[!] IWDG Reset Detected
==================================
```

---

## 6. WWDG 窗口看门狗

**项目目录**：[06_USART_WWDG](./06_USART_WWDG)

**实验简介**：窗口看门狗（WWDG）与 USART 串口调试的综合实验。与 IWDG 不同，WWDG 必须在**精确的时间窗口内**喂狗——喂早了不行，喂晚了也不行。

**WWDG vs IWDG 核心区别**：

| 特性 | IWDG | WWDG |
|------|------|------|
| 时钟源 | 独立 LSI (~40kHz) | APB1 (PCLK1=36MHz) |
| 喂狗规则 | 超时前喂就行 | 必须在 [64, 窗口值] 区间内喂 |
| 计数器 | 12 位递减 | 7 位递减 |
| 窗口检测 | 无 | 有（喂早了也复位） |
| 早期中断 | 无 | EWI（counter=0x40 时触发） |
| 能否关闭 | 启动后不可关 | 启动后不可关 |

**实验参数**：

- 分频系数 = 8
- 计数器初值 = 0x7F (127)
- 窗口值 = 0x5F (95)
- 计数器时钟 = 36MHz / 4096 / 8 = 1099 Hz
- 每 tick ≈ 910 µs

```text
127 ─── 95 ─── 64 ─── 63 = 复位!
|<-- 禁喂 -->|<-- 可喂窗口 -->|<-- 太晚 -->|
   ~29 ms          ~28 ms
```

**实现功能**：

- **两阶段自动演示**：
  - 阶段 1（首次上电 POR）：正常喂狗 29 次 → 第 30 次故意过早喂狗 → WWDG 复位
  - 阶段 2（WWDG 复位后）：完全不喂狗 → counter 自然减到 0x3F → WWDG 复位
- **复位源检测**：通过 RCC CSR 寄存器的 WWDGRSTF 标志位识别 WWDG 复位
- **自实现串口**：`UART_SendStr/SendDec/SendHex` 直接寄存器操作，不依赖 MicroLIB

**避坑总结**：

- **printf 顺序陷阱**：`WWDG_Enable()` 必须在 `printf` 之后调用，否则打印期间看门狗就超时了
- **EWI 中断不能 printf**：EWI 到复位只有 910µs，中断中只能做最轻量的操作（设置标志位）
- **"Connect under Reset"**：WWDG 复位循环会阻止调试器连接，需在 Debug 设置中勾选该选项
- **BOOT0 救砖**：如果"Connect under Reset"也不行，把 BOOT0 拉高后上电即可绕过用户程序

---

## 7. 单通道 ADC 中断转换

**项目目录**：[07_USART_ADC](./07_USART_ADC)

**实验简介**：配置 ADC1 通道 1（PA1）实现模拟信号采集，每次转换完成后触发 EOC 中断，在 ISR 中读取转换结果并通过串口打印。

**实现功能**：

- **ADC 中断采集**：配置 ADC1_CH1（PA1）为模拟输入，单次转换 + 软件触发模式
- **EOC 中断**：每次转换完成后硬件置 EOC 标志，触发 `ADC1_2_IRQHandler` 读取 `ADC_DR`
- **ADC 校准**：上电后执行复位校准 + ADC 校准，确保读数准确
- **电压换算**：`voltage_mv = adc_value * 3300 / 4096`，12 位 ADC 量程 0~4095 对应 0~3.3V
- **串口输出**：自实现寄存器直操作串口函数，不依赖 MicroLIB

**避坑总结**：

- **ADCCLK 超频陷阱**：`SystemInit()` 默认 ADCPRE=0（PCLK2/2=36MHz），远超 ADC 最大 14MHz，必须显式调用 `RCC_ADCCLKConfig(RCC_PCLK2_Div6)`
- **中断 vs 轮询**：EOC 中断方式比轮询 `ADC_GetFlagStatus` 更高效
- **校准顺序不能错**：`ADC_Cmd(ENABLE)` → 等待稳定 → `ADC_ResetCalibration` → `ADC_StartCalibration`

**硬件连接**：

- MCU: STM32F103C8T6 (Blue Pill)
- 模拟输入: PA1 ← 3.3V / GND / 电位器中间脚
- 串口模块: USB-to-TTL (PA9→RX, GND→GND)，波特率 115200

**运行结果**：

```text
ADC Interrupt Conversion Demo
Channel: ADC1_CH1 (PA1)
Mode: Single + EOC Interrupt
[ 1] ADC Value: 4095  (3.29 V)
[ 2] ADC Value: 4095  (3.29 V)
[ 3] ADC Value: 4094  (3.29 V)
```

PA1 接 3.3V → 读数 ~4095（满量程）；接 GND → 读数 ~0；接电位器 → 读数随旋钮线性变化。

---

## 8. 多通道 ADC + DMA 自动搬运

**项目目录**：[08_DMA_ADC](./08_DMA_ADC)

**实验简介**：在实验 7 的基础上，从"单通道中断"升级为"多通道扫描 + DMA 自动搬运"。ADC1 以扫描模式依次转换 CH1/CH2/CH3（PA1/PA2/PA3），DMA 以循环模式将每次转换结果自动搬运到内存数组，CPU 无需参与数据搬运。

**核心原理**：ADC_DR 数据寄存器只有 1 个，扫描模式下新数据会覆盖旧数据。DMA 在每个通道转换完成瞬间自动从 ADC_DR 搬到内存，全程硬件完成。

```text
PA1→CH1 ─┐
PA2→CH2 ─┤ ADC 扫描 → ADC_DR → DMA1_Ch1 → adc_buffer[0/1/2]
PA3→CH3 ─┘ (连续)     (固定)    (外设→内存)  (递增, 循环)
```

**实现功能**：

- **ADC 扫描模式**：`ScanConvMode=ENABLE`，`ContinuousConvMode=ENABLE`，`NbrOfChannel=3`
- **DMA 循环搬运**：`DMA_Mode_Circular`，源地址固定，目的地址递增，每轮搬 3 次
- **主循环零等待**：直接读取 `adc_buffer[0/1/2]`，无需等待中断标志位
- **易扩展**：使用 `#define ADC_CHANNEL_COUNT 3` 宏，增减通道只需改一个数字

**避坑总结**：

- **时钟必须先开**：`RCC_AHBPeriphClockCmd(DMA1)` 必须在 `DMA_StructInit` 之前调用
- **DMA 启动时机**：`DMA_Cmd` 必须放在 ADC 校准完成之后
- **DMA_Mode_Circular 不可少**：`DMA_Mode_Normal` 搬完一轮就停转
- **数组类型**：必须用 `uint16_t`，ADC 12 位结果范围 0~4095

**硬件连接**：

- MCU: STM32F103C8T6 (Blue Pill)
- 模拟输入: PA1/PA2/PA3 ← 分别接 3.3V / GND / 悬空或电位器
- 串口模块: USB-to-TTL (PA9→RX, GND→GND)，波特率 115200

**运行结果**：

```text
ADC Multi-Channel Scan + DMA Demo
CH1=PA1  CH2=PA2  CH3=PA3
Mode: Scan + Continuous + DMA Circular
[ 1] CH1=4095 CH2=0    CH3=1138  (3.29 V)
[ 2] CH1=4095 CH2=0    CH3=1137  (3.29 V)
[ 3] CH1=4095 CH2=0    CH3=1135  (3.29 V)
```

---

## 9. 软件 I2C 读写 MPU6050 + OLED 显示

**项目目录**：[09_I2C_MPU6050_SW](./09_I2C_MPU6050_SW)

**实验简介**：纯软件模拟 I2C 总线，驱动 MPU6050/MPU6500 六轴传感器 + SSD1306 OLED（128×64），在屏幕上实时显示加速度、陀螺仪和温度数据。全链路从 GPIO 开漏输出开始手写，不使用 STM32 硬件 I2C。

**硬件接线**：

| Blue Pill | MPU6050 | OLED (SSD1306) |
|:---------:|:-------:|:--------------:|
| 3.3V | VCC | VCC |
| GND | GND | GND |
| PB10 | SCL | SCL |
| PB11 | SDA | SDA |
| — | AD0（悬空） | — |

> 两个模块共用同一条 I2C 总线（PB10=SCL, PB11=SDA），地址不冲突（MPU6050=0x68/0x70, OLED=0x3C）。

**软件架构**（自底向上四层）：

```text
┌─────────────────────────────────────┐
│ main.c      ← 主循环: 读传感器→OLED显示 │
├─────────────────────────────────────┤
│ MPU6050.c   ← MPU6050_ReadRegs/WriteReg │
│ OLED.c      ← OLED_ShowString/ShowNum   │
├─────────────────────────────────────┤
│ i2c.c       ← 软件 I2C (Start/Stop/Send/Read) │
├─────────────────────────────────────┤
│ GPIO (PB10/PB11 开漏输出)              │
└─────────────────────────────────────┘
```

**各阶段对应文件**：

| 阶段 | 文件 | 核心内容 |
|:----:|------|---------|
| 1 | `i2c.c` / `i2c.h` | 软件模拟 I2C：起始/停止/发送/接收/应答，~100kHz |
| 2 | `MPU6050.c` / `MPU6050.h` | 寄存器读写、传感器初始化、原始数据读取、物理量换算 |
| 3 | `OLED.c` / `OLED.h` | SSD1306 页寻址模式、6×8 ASCII 字库、字符串/整数显示 |
| 4 | `main.c` | 综合调度：I2C→OLED→MPU6050 初始化 → 循环显示 |

**核心知识点**：

- **开漏输出 + 上拉 = 线与逻辑**：任意设备可拉低总线，无设备可强行拉高
- **I2C 起始/停止信号**：SCL=1 期间 SDA 变化（1→0 起始，0→1 停止）
- **应答机制**：每 8 bit 后跟 1 bit ACK/NAK，实现可靠传输
- **控制字节区分命令/数据**：SSD1306 用 0x00（命令）和 0x40（数据）区分
- **MPU6050 读时序**：先写寄存器地址（写模式）→ 重新起始 → 读数据（读模式）
- **避免浮点运算**：STM32F103 无硬件 FPU，用整数运算（×100÷2048）换算物理量

**I2C 读写时序对比**：

```text
写寄存器:  [S]→[ADDR+W]→[RegAddr]→[Data]→[P]
读寄存器:  [S]→[ADDR+W]→[RegAddr]→[S]→[ADDR+R]→[Data...]→[P]
                                           ↑ 重新起始，切换方向
```

**运行结果**：OLED 屏幕 8 行实时显示：

```text
=== MPU6050 ===
AX: +0.02g        ← 水平静止 ≈ 0
AY: -0.01g        ← 水平静止 ≈ 0
AZ: +1.01g        ← 受重力 ≈ +1g
GX: +0.7d/s       ← 静止 ≈ 0
GY: -2.1d/s       ← 静止 ≈ 0
GZ: +5.3d/s       ← 静止 ≈ 0
T : 31.2C          ← 芯片温度
```

**踩坑记录**：

- 芯片丝印标 MPU6050 但 WHO_AM_I 返回 0x70 → 实际是 MPU6500，寄存器兼容，改 WHO_AM_I 检查即可
- `stm32f10x_i2c.h` 中的 `I2C_Init()` 与自定义函数重名 → 改名 `SoftI2C_Init()`
- `stm32f10x_conf.h` 缺失 → 从兄弟项目复制标准模板
- OLED 初始化失败（全程黑屏）→ 上电延时从 ~4ms 增加到 ~300ms + I2C 降速到 25kHz
- 浮点 `OLED_ShowFloat` 在 STM32F103 上不可靠 → 改用纯整数运算显示

**开发环境**：

- IDE: Keil MDK (uVision5)，ARMCC V5
- 硬件: STM32F103C8T6 (Blue Pill)
- 传感器: MPU6050/MPU6500（AD0 悬空，地址 0x68/0x70）
- 显示: SSD1306 0.96 寸 OLED（128×64, I2C 地址 0x3C）
- 库: STM32 标准外设库 V3.5.0
