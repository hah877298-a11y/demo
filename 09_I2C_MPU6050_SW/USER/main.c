#include "stm32f10x.h"
#include "i2c.h"
#include "MPU6050.h"
#include "OLED.h"

/* ============================================================
 *       LED 闪 烁 诊 断 版 — 用 PC13 板 载 LED 排 查
 * ============================================================
 *
 *  Blue Pill 板载 LED 接在 PC13, 低电平点亮.
 *  不同闪烁次数代表不同的执行阶段, 用肉眼即可判断:
 *
 *  闪烁次数 │ 含义
 *  ─────────┼─────────────────────────────
 *   闪 1 次  │ 程序启动, 开始运行
 *   闪 2 次  │ I2C GPIO 初始化完成 (PB10/PB11 OK)
 *   闪 3 次  │ OLED 初始化完成
 *   闪 4 次  │ OLED_Fill 完成 (你应该看到全屏亮!)
 *   闪 5 次  │ MPU6050 ID 读取成功 (0x68)
 *   闪 6 次  │ 进入主循环
 *
 *  如果 LED 停在某个闪烁次数不再继续:
 *   → 说明卡在那一阶段, 查看下方对应解释
 *
 *  如果 LED 完全不亮也不闪:
 *   → 程序根本没运行, 检查烧录和电源
 * ============================================================
 */

/* ---- PC13 LED 控制 ---- */
#define LED_ON()    GPIO_ResetBits(GPIOC, GPIO_Pin_13)   /* PC13=0 → LED 亮 */
#define LED_OFF()   GPIO_SetBits(GPIOC, GPIO_Pin_13)     /* PC13=1 → LED 灭 */

static void delay_ms(uint32_t ms)
{
    uint32_t i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 8000; j++)
            __NOP();
}

/* ---- 闪灯: 闪烁 count 次 (每闪 200ms 亮 + 200ms 灭) ---- */
static void blink(uint8_t count)
{
    uint8_t i;
    for (i = 0; i < count; i++)
    {
        LED_ON();   delay_ms(200);
        LED_OFF();  delay_ms(200);
    }
    delay_ms(800);  /* 每组闪烁之间停顿, 方便数数 */
}

int main(void)
{
    MPU6050_RawData raw;
    MPU6050_PhyData phy;
    uint8_t whoami;

    /* ---- 初始化 PC13 LED ---- */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    {
        GPIO_InitTypeDef g;
        g.GPIO_Pin   = GPIO_Pin_13;
        g.GPIO_Mode  = GPIO_Mode_Out_PP;
        g.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOC, &g);
    }
    LED_OFF();  /* PC13 输出高电平 → LED 灭 */

    delay_ms(500);  /* 等待电源稳定 */

    blink(1);       /* ★ 闪 1 次 = 程序启动成功 */

    /* ===== 阶段 1: I2C 初始化 ===== */
    SoftI2C_Init();
    blink(2);       /* ★ 闪 2 次 = I2C GPIO 初始化完成 */

    /* ===== 阶段 2: OLED 初始化 ===== */
    OLED_Init();
    blink(3);       /* ★ 闪 3 次 = OLED 初始化完成 */

    /* ===== 阶段 3: 全屏点亮测试 ===== */
    OLED_Fill();
    blink(4);       /* ★ 闪 4 次 = OLED_Fill 完成 → 此时屏幕应该全亮! */

    delay_ms(1500); /* 让全亮状态停留 1.5 秒, 方便观察 */
    OLED_Clear();

    /* ===== 阶段 4: MPU6050 测试 ===== */
    MPU6050_Init();
    whoami = MPU6050_ReadID();

    OLED_ShowString(0, 0, "OLED: OK");
    /*
     * MPU6050 的 WHO_AM_I = 0x68 (104)
     * MPU6500 的 WHO_AM_I = 0x70 (112)
     * 两种芯片寄存器兼容, 都视为正常.
     */
    if (whoami == 0x68 || whoami == 0x70)
    {
        OLED_ShowString(0, 2, "MPU: OK");
        blink(5);   /* ★ 闪 5 次 = 传感器通信成功 */
    }
    else
    {
        OLED_ShowString(0, 2, "MPU: FAIL");
        OLED_ShowString(0, 3, "ID=");
        OLED_ShowNum(24, 3, whoami, 4);
        while (1)
        {
            LED_ON();  delay_ms(500);
            LED_OFF(); delay_ms(500);
        }
    }

    delay_ms(1000);
    blink(6);       /* ★ 闪 6 次 = 进入主循环 */

    /* ===== 阶段 5: 主循环 (整数运算显示物理量, 不用浮点) =====
     *
     *  换算公式 (全用整数, 避免浮点运算):
     *
     *    加速度 (±16g):  g值 = 原始值 × 100 ÷ 2048
     *      例: raw=2104 → 2104*100/2048 = 102 → 显示 +1.02g
     *
     *    陀螺仪 (±2000°/s):  °/s = 原始值 × 100 ÷ 164
     *      例: raw=100 → 100*100/164 = 60 → 显示 +6.0d/s
     *
     *    温度:  °C = 原始值 × 10 ÷ 340 + 365
     *      例: raw=500 → 500*10/340+365 = 14+365 = 379 → 显示 37.9C
     */
    while (1)
    {
        int32_t val;
        uint8_t sign;
        int32_t ipart, dpart;

        MPU6050_ReadRawData(&raw);

        OLED_Clear();
        OLED_ShowString(0, 0, "=== MPU6050 ===");

        /* ---- 加速度 X (2 位小数) ---- */
        val  = (int32_t)raw.Accel_X * 100 / 2048;   /* ×100 → 2 位小数 */
        sign = (val >= 0) ? '+' : '-';
        if (val < 0) val = -val;
        ipart = val / 100;  dpart = val % 100;

        OLED_ShowString(0, 1, "AX:");
        OLED_ShowChar(18, 1, sign);
        OLED_ShowNum(24, 1, ipart, 2);
        OLED_ShowChar(36, 1, '.');
        OLED_ShowChar(42, 1, '0' + dpart / 10);     /* 十分位 */
        OLED_ShowChar(48, 1, '0' + dpart % 10);     /* 百分位 */
        OLED_ShowString(60, 1, "g");

        /* ---- 加速度 Y ---- */
        val  = (int32_t)raw.Accel_Y * 100 / 2048;
        sign = (val >= 0) ? '+' : '-';
        if (val < 0) val = -val;
        ipart = val / 100;  dpart = val % 100;

        OLED_ShowString(0, 2, "AY:");
        OLED_ShowChar(18, 2, sign);
        OLED_ShowNum(24, 2, ipart, 2);
        OLED_ShowChar(36, 2, '.');
        OLED_ShowChar(42, 2, '0' + dpart / 10);
        OLED_ShowChar(48, 2, '0' + dpart % 10);
        OLED_ShowString(60, 2, "g");

        /* ---- 加速度 Z ---- */
        val  = (int32_t)raw.Accel_Z * 100 / 2048;
        sign = (val >= 0) ? '+' : '-';
        if (val < 0) val = -val;
        ipart = val / 100;  dpart = val % 100;

        OLED_ShowString(0, 3, "AZ:");
        OLED_ShowChar(18, 3, sign);
        OLED_ShowNum(24, 3, ipart, 2);
        OLED_ShowChar(36, 3, '.');
        OLED_ShowChar(42, 3, '0' + dpart / 10);
        OLED_ShowChar(48, 3, '0' + dpart % 10);
        OLED_ShowString(60, 3, "g");

        /* ---- 陀螺仪 X (1 位小数) ---- */
        val  = (int32_t)raw.Gyro_X * 100 / 164;       /* ×10 → 1 位小数 */
        sign = (val >= 0) ? '+' : '-';
        if (val < 0) val = -val;
        ipart = val / 10;  dpart = val % 10;

        OLED_ShowString(0, 4, "GX:");
        OLED_ShowChar(18, 4, sign);
        OLED_ShowNum(24, 4, ipart, 3);
        OLED_ShowChar(42, 4, '.');
        OLED_ShowChar(48, 4, '0' + dpart);
        OLED_ShowString(60, 4, "d/s");

        /* ---- 陀螺仪 Y ---- */
        val  = (int32_t)raw.Gyro_Y * 100 / 164;
        sign = (val >= 0) ? '+' : '-';
        if (val < 0) val = -val;
        ipart = val / 10;  dpart = val % 10;

        OLED_ShowString(0, 5, "GY:");
        OLED_ShowChar(18, 5, sign);
        OLED_ShowNum(24, 5, ipart, 3);
        OLED_ShowChar(42, 5, '.');
        OLED_ShowChar(48, 5, '0' + dpart);
        OLED_ShowString(60, 5, "d/s");

        /* ---- 陀螺仪 Z ---- */
        val  = (int32_t)raw.Gyro_Z * 100 / 164;
        sign = (val >= 0) ? '+' : '-';
        if (val < 0) val = -val;
        ipart = val / 10;  dpart = val % 10;

        OLED_ShowString(0, 6, "GZ:");
        OLED_ShowChar(18, 6, sign);
        OLED_ShowNum(24, 6, ipart, 3);
        OLED_ShowChar(42, 6, '.');
        OLED_ShowChar(48, 6, '0' + dpart);
        OLED_ShowString(60, 6, "d/s");

        /* ---- 温度 (1 位小数) ----
         *
         *  注意: 这里测的是芯片内部温度, 不是室温!
         *  芯片自发热 + 传感器精度有限 (误差 ±3°C),
         *  正常显示会比室温高 10~20°C, 这是正常的.
         *
         *  MPU6500 公式: °C = 原始值 ÷ 321 + 21
         *  (你的芯片是 MPU6500, WHO_AM_I=0x70,
         *   偏移量 21°C 而非 MPU6050 的 36.53°C) */
        val  = (int32_t)raw.Temp * 10 / 321 + 210;
        ipart = val / 10;  dpart = val % 10;

        OLED_ShowString(0, 7, "T :");
        OLED_ShowNum(24, 7, ipart, 2);
        OLED_ShowChar(36, 7, '.');
        OLED_ShowChar(42, 7, '0' + dpart);
        OLED_ShowString(54, 7, "C");

        /* 主循环中 LED 快速闪一下, 证明程序还在跑 */
        LED_ON();  delay_ms(20);
        LED_OFF();
        delay_ms(180);
    }
}
