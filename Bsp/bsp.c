/**
 * @file      bsp.c
 * @brief     
 * 
 * @version   1.0
 * @author    Drinkto
 * @date      Mar 6, 2026
 * 
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Mar 6, 2026 | 1.0 | Drinkto | xxx |
 */

#include "bsp_led.h"

void bsp_PlatformInit(void)
{
        /* 在这里添加其他外设的初始化代码 */
        bsp_LedInit(&led1, false, GPIOC, GPIO_PIN_13); /* 初始化LED1，LED灭时为低电平，使用GPIOC的13号引脚 */
}