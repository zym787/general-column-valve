/**
 * @file      bsp.h
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

#ifndef __BSP_H__
#define __BSP_H__

/**
 * @brief     使能外设时钟
 * @details   BSP层封装的外设时钟使能宏，使用时传入外设的基地址即可，如GPIOA、USART1等
 */
#define BSP_CLK_ENABLE(__PERIPHERAL_)                    \
        do {                                             \
                if ((__PERIPHERAL_) == DMA1) {           \
                        __HAL_RCC_DMA1_CLK_ENABLE();     \
                } else if ((__PERIPHERAL_) == SRAM) {    \
                        __HAL_RCC_SRAM_CLK_ENABLE();     \
                } else if ((__PERIPHERAL_) == FLITF) {   \
                        __HAL_RCC_FLITF_CLK_ENABLE();    \
                } else if ((__PERIPHERAL_) == CRC) {     \
                        __HAL_RCC_CRC_CLK_ENABLE();      \
                } else if ((__PERIPHERAL_) == TIM2) {    \
                        __HAL_RCC_TIM2_CLK_ENABLE();     \
                } else if ((__PERIPHERAL_) == TIM3) {    \
                        __HAL_RCC_TIM3_CLK_ENABLE();     \
                } else if ((__PERIPHERAL_) == WWDG) {    \
                        __HAL_RCC_WWDG_CLK_ENABLE();     \
                } else if ((__PERIPHERAL_) == USART2) {  \
                        __HAL_RCC_USART2_CLK_ENABLE();   \
                } else if ((__PERIPHERAL_) == I2C1) {    \
                        __HAL_RCC_I2C1_CLK_ENABLE();     \
                } else if ((__PERIPHERAL_) == BKP) {     \
                        __HAL_RCC_BKP_CLK_ENABLE();      \
                } else if ((__PERIPHERAL_) == PWR) {     \
                        __HAL_RCC_PWR_CLK_ENABLE();      \
                } else if ((__PERIPHERAL_) == AFIO) {    \
                        __HAL_RCC_AFIO_CLK_ENABLE();     \
                } else if ((__PERIPHERAL_) == GPIOA) {   \
                        __HAL_RCC_GPIOA_CLK_ENABLE();    \
                } else if ((__PERIPHERAL_) == GPIOB) {   \
                        __HAL_RCC_GPIOB_CLK_ENABLE();    \
                } else if ((__PERIPHERAL_) == GPIOC) {   \
                        __HAL_RCC_GPIOC_CLK_ENABLE();    \
                } else if ((__PERIPHERAL_) == GPIOD) {   \
                        __HAL_RCC_GPIOD_CLK_ENABLE();    \
                } else if ((__PERIPHERAL_) == ADC1) {    \
                        __HAL_RCC_ADC1_CLK_ENABLE();     \
                } else if ((__PERIPHERAL_) == TIM1) {    \
                        __HAL_RCC_TIM1_CLK_ENABLE();     \
                } else if ((__PERIPHERAL_) == SPI1) {    \
                        __HAL_RCC_SPI1_CLK_ENABLE();     \
                } else if ((__PERIPHERAL_) == USART1) {  \
                        __HAL_RCC_USART1_CLK_ENABLE();   \
                } else if ((__PERIPHERAL_) == USART2) {  \
                        __HAL_RCC_USART2_CLK_ENABLE();   \
                } else if ((__PERIPHERAL_) == I2C1) {    \
                        __HAL_RCC_I2C1_CLK_ENABLE();     \
                } else { /* 其他外设 */                  \
                        /* 可以添加其他外设的时钟使能 */ \
                }                                        \
        } while (0U)
/**
 * @brief     关闭外设时钟
 * @details   BSP层封装的外设时钟关闭宏，使用时传入外设的基地址即可，如GPIOA、USART1等
 */
#define BSP_CLK_DISABLE(__PERIPHERAL_)                   \
        do {                                             \
                if ((__PERIPHERAL_) == DMA1) {           \
                        __HAL_RCC_DMA1_CLK_DISABLE();    \
                } else if ((__PERIPHERAL_) == SRAM) {    \
                        __HAL_RCC_SRAM_CLK_DISABLE();    \
                } else if ((__PERIPHERAL_) == FLITF) {   \
                        __HAL_RCC_FLITF_CLK_DISABLE();   \
                } else if ((__PERIPHERAL_) == CRC) {     \
                        __HAL_RCC_CRC_CLK_DISABLE();     \
                } else if ((__PERIPHERAL_) == TIM2) {    \
                        __HAL_RCC_TIM2_CLK_DISABLE();    \
                } else if ((__PERIPHERAL_) == TIM3) {    \
                        __HAL_RCC_TIM3_CLK_DISABLE();    \
                } else if ((__PERIPHERAL_) == WWDG) {    \
                        __HAL_RCC_WWDG_CLK_DISABLE();    \
                } else if ((__PERIPHERAL_) == USART2) {  \
                        __HAL_RCC_USART2_CLK_DISABLE();  \
                } else if ((__PERIPHERAL_) == I2C1) {    \
                        __HAL_RCC_I2C1_CLK_DISABLE();    \
                } else if ((__PERIPHERAL_) == BKP) {     \
                        __HAL_RCC_BKP_CLK_DISABLE();     \
                } else if ((__PERIPHERAL_) == PWR) {     \
                        __HAL_RCC_PWR_CLK_DISABLE();     \
                } else if ((__PERIPHERAL_) == AFIO) {    \
                        __HAL_RCC_AFIO_CLK_DISABLE();    \
                } else if ((__PERIPHERAL_) == GPIOA) {   \
                        __HAL_RCC_GPIOA_CLK_DISABLE();   \
                } else if ((__PERIPHERAL_) == GPIOB) {   \
                        __HAL_RCC_GPIOB_CLK_DISABLE();   \
                } else if ((__PERIPHERAL_) == GPIOC) {   \
                        __HAL_RCC_GPIOC_CLK_DISABLE();   \
                } else if ((__PERIPHERAL_) == GPIOD) {   \
                        __HAL_RCC_GPIOD_CLK_DISABLE();   \
                } else if ((__PERIPHERAL_) == ADC1) {    \
                        __HAL_RCC_ADC1_CLK_DISABLE();    \
                } else if ((__PERIPHERAL_) == TIM1) {    \
                        __HAL_RCC_TIM1_CLK_DISABLE();    \
                } else if ((__PERIPHERAL_) == SPI1) {    \
                        __HAL_RCC_SPI1_CLK_DISABLE();    \
                } else if ((__PERIPHERAL_) == USART1) {  \
                        __HAL_RCC_USART1_CLK_DISABLE();  \
                } else if ((__PERIPHERAL_) == USART2) {  \
                        __HAL_RCC_USART2_CLK_DISABLE();  \
                } else if ((__PERIPHERAL_) == I2C1) {    \
                        __HAL_RCC_I2C1_CLK_DISABLE();    \
                } else { /* 其他外设 */                  \
                        /* 可以添加其他外设的时钟使能 */ \
                }                                        \
        } while (0U)

#define BSP_GPIO_CONFIG_OUTPUT_PP(__PORT__, __PIN__, __LEVEL__)        \
        do {                                                           \
                GPIO_InitTypeDef GPIO_InitStruct = {0};                \
                GPIO_InitStruct.Pin = (__PIN__);                       \
                GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;            \
                GPIO_InitStruct.Pull = GPIO_NOPULL;                    \
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;           \
                HAL_GPIO_Init((__PORT__), &GPIO_InitStruct);           \
                HAL_GPIO_WritePin((__PORT__), (__PIN__), (__LEVEL__)); \
        } while (0U)

#endif /* __BSP_H__ */