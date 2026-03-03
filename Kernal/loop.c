/**
 * @file      : loop.c
 * @brief     :
 *
 * @version   : 1.0
 * @author    : Drinkto
 * @date      : Jan 27, 2026
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Jan 27, 2026 | 1.0 | Drinkto | xxx |
 */

#include "loop.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include "tim.h"

void loop(void)
{
        for (;;) {
                // HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
                HAL_GPIO_WritePin(DIR1_GPIO_Port, DIR1_Pin, GPIO_PIN_SET);

                HAL_GPIO_TogglePin(LED2_R_GPIO_Port, LED2_R_Pin);
                HAL_Delay(500);

                HAL_GPIO_WritePin(DIR1_GPIO_Port, DIR1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_TogglePin(LED1_G_GPIO_Port, LED1_G_Pin);
                HAL_Delay(500);

                // HAL_GPIO_WritePin(ENA1_GPIO_Port, ENA1_Pin, GPIO_PIN_RESET);
        }
}