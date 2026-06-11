/**
 * @file      bsp_led.h
 * @brief     led驱动
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

#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "gpio.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal_gpio.h"

#include <stdbool.h>

typedef struct LED_TAG {
        GPIO_TypeDef *port; /* 所使用的端口 */
        uint16_t pin;       /* 所使用的引脚号 */
        bool status;        /* led状态 */
        GPIO_PinState led_off_level; /* led灭时IO口电平状态 */
        bool init;          /* 初始化标志 */
} Led_t;

int8_t bsp_LedInit(Led_t *_led, bool _led_off_level, GPIO_TypeDef *_port, uint16_t _pin);
extern Led_t led1;

#endif
