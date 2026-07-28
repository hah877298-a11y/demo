#include "stm32f10x.h"
#include "spi.h"
#include "w25q64.h"
#include "OLED.h"
#include "i2c.h"

int main(void)
{
    uint16_t id;
    uint8_t mf, dev;

    SoftI2C_Init();
    OLED_Init();
    OLED_Clear();

    W25Q64_Init();

    /* 读取 JEDEC ID */
    id  = W25Q64_ReadID();
    mf  = (id >> 8) & 0xFF;
    dev = (id >> 0) & 0xFF;

    OLED_ShowString(0, 0, "W25Q64 OK");
    OLED_ShowString(0, 2, "MF:0x");
    OLED_ShowNum(36, 2, mf, 3);
    OLED_ShowString(0, 4, "DEV:0x");
    OLED_ShowNum(36, 4, dev, 2);

    while (1) {}
}
