/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ENA1_Pin GPIO_PIN_13
#define ENA1_GPIO_Port GPIOC
#define DIR1_Pin GPIO_PIN_14
#define DIR1_GPIO_Port GPIOC
#define LED2_R_Pin GPIO_PIN_15
#define LED2_R_GPIO_Port GPIOC
#define OPTO_IN1_Pin GPIO_PIN_0
#define OPTO_IN1_GPIO_Port GPIOA
#define OPTO_IN2_Pin GPIO_PIN_1
#define OPTO_IN2_GPIO_Port GPIOA
#define DIR2_Pin GPIO_PIN_4
#define DIR2_GPIO_Port GPIOA
#define ENA2_Pin GPIO_PIN_5
#define ENA2_GPIO_Port GPIOA
#define STEP2_Pin GPIO_PIN_6
#define STEP2_GPIO_Port GPIOA
#define ENA3_Pin GPIO_PIN_7
#define ENA3_GPIO_Port GPIOA
#define DIR3_Pin GPIO_PIN_0
#define DIR3_GPIO_Port GPIOB
// #define LED1_G_Pin GPIO_PIN_2
#define LED1_G_Pin          GPIO_PIN_13
#define LED1_G_GPIO_Port GPIOB
#define STEP3_Pin GPIO_PIN_10
#define STEP3_GPIO_Port GPIOB
#define I2C_SDA_Pin GPIO_PIN_12
#define I2C_SDA_GPIO_Port GPIOB
#define I2C_SCK_Pin GPIO_PIN_13
#define I2C_SCK_GPIO_Port GPIOB
#define ENA4_Pin GPIO_PIN_14
#define ENA4_GPIO_Port GPIOB
#define DIR4_Pin GPIO_PIN_15
#define DIR4_GPIO_Port GPIOB
#define STEP4_Pin GPIO_PIN_8
#define STEP4_GPIO_Port GPIOA
#define OPTO_IN3_Pin GPIO_PIN_11
#define OPTO_IN3_GPIO_Port GPIOA
#define OPTO_IN4_Pin GPIO_PIN_12
#define OPTO_IN4_GPIO_Port GPIOA
#define RS485_CTR_Pin GPIO_PIN_15
#define RS485_CTR_GPIO_Port GPIOA
#define DIPSW2_Pin GPIO_PIN_3
#define DIPSW2_GPIO_Port GPIOB
#define DIPSW4_Pin GPIO_PIN_4
#define DIPSW4_GPIO_Port GPIOB
#define DIPSW1_Pin GPIO_PIN_5
#define DIPSW1_GPIO_Port GPIOB
#define DIPSW8_Pin GPIO_PIN_6
#define DIPSW8_GPIO_Port GPIOB
#define STEP1_Pin GPIO_PIN_9
#define STEP1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
