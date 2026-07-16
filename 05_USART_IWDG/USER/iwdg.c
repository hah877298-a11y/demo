#include "iwdg.h"

/**
  * @brief  初始化独立看门狗 (IWDG)
  * @param  无
  * @retval 无
  * @note   预分频=64, 重装载=625, LSI~40kHz
  *         溢出时间 ≈ 64/40000 × 625 = 1.0 秒
  *         启动后不可关闭，必须每 <1 秒喂狗一次
  */
void IWDG_Configuration(void)
{
    /* ① 解锁 IWDG 写保护：向 KR 写入 0x5555 */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* ② 设置预分频器：决定计数器递减的快慢 */
    IWDG_SetPrescaler(IWDG_Prescaler_64);

    /* ③ 设置重装载值：计数器从这里开始递减 */
    IWDG_SetReload(625);

    /* ④ 重载计数器：将 625 装入递减计数器 */
    IWDG_ReloadCounter();

    /* ⑤ 启动看门狗：向 KR 写入 0xCCCC，此后无法关闭 */
    IWDG_Enable();
}

/**
  * @brief  喂狗函数：在溢出前调用以阻止复位
  * @param  无
  * @retval 无
  * @note   调用间隔必须 < 溢出时间（本配置中约 1 秒）
  */
void IWDG_FeedDog(void)
{
    IWDG_ReloadCounter();
}
