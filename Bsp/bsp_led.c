/**
 * @file      bsp_led.c
 * @brief
 *
 * @version   1.0
 * @author    Drinkto
 * @date      Mar 5, 2026
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Mar 5, 2026 | 1.0 | Drinkto | xxx |
 */

#include "bsp_led.h"
#include <stddef.h>
#include "bsp.h"
#include "stm32f103xb.h"

/**
 * @brief     初始化LED
 * @details
 * BSP层封装的LED初始化函数，使用时传入LED结构体指针、LED灭时IO口电平状态、所使用的端口和引脚号
 * @param     _led LED结构体指针
 * @param     _led_off_level
 * LED灭时IO口电平状态，true表示高电平灭，false表示低电平灭
 * @param     _port 所使用的端口，如GPIOA、GPIOB等
 * @param     _pin 所使用的引脚号，如GPIO_PIN_0、GPIO_PIN_1等
 * @return    int8_t 返回值，0表示成功，-1表示失败
 */
int8_t bsp_LedInit(Led_t *_led, bool _led_off_level, GPIO_TypeDef *_port,
                   uint16_t _pin)
{
        if (_led == NULL) {
                return -1;
        }

        _led->port = _port;
        _led->pin = _pin;
        _led->led_off_level = _led_off_level; /* 设置LED关闭时IO口的电平 */
        _led->status = false;                 /* 初始状态为关闭 */
        _led->init = true;                    /* 该结构体已被初始化 */

        // BSP_CLK_ENABLE(_led->port); /* 使能对应GPIO端口的时钟 */
        BSP_GPIO_CONFIG_OUTPUT_PP(_led->port, _led->pin, _led->led_off_level);

        return 0;
}

/**
 * @brief     点亮LED
 * @details   BSP层封装的LED点亮函数，使用时传入LED结构体指针
 * @param     _led LED结构体指针
 * @return    int8_t 返回值，0表示成功，-1表示失败
 */
int8_t bsp_LedOn(Led_t *_led)
{
        if (_led == NULL || _led->init == false) {
                return -1;
        }

        HAL_GPIO_WritePin(_led->port, _led->pin,
                          !_led->led_off_level); /* 点亮LED */
        _led->status = true;

        return 0;
}

/**
 * @brief     熄灭LED
 * @details   BSP层封装的LED熄灭函数，使用时传入LED结构体指针
 * @param     _led LED结构体指针
 * @return    int8_t 返回值，0表示成功，-1表示失败
 */
int8_t bsp_LedOff(Led_t *_led)
{
        if (_led == NULL || _led->init == false) {
                return -1;
        }

        HAL_GPIO_WritePin(_led->port, _led->pin,
                          _led->led_off_level); /* 熄灭LED */
        _led->status = false;

        return 0;
}