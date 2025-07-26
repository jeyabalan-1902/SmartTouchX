/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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
#define TOUCH_LED3_Pin GPIO_PIN_1
#define TOUCH_LED3_GPIO_Port GPIOH
#define TOUCH_LED2_Pin GPIO_PIN_2
#define TOUCH_LED2_GPIO_Port GPIOC
#define TOUCH_LED1_Pin GPIO_PIN_3
#define TOUCH_LED1_GPIO_Port GPIOC
#define DISP_BACKLIT_Pin GPIO_PIN_0
#define DISP_BACKLIT_GPIO_Port GPIOA
#define TOUCH_LED4_Pin GPIO_PIN_14
#define TOUCH_LED4_GPIO_Port GPIOB
#define L_RELAY_4_Pin GPIO_PIN_6
#define L_RELAY_4_GPIO_Port GPIOC
#define L_RELAY_2_Pin GPIO_PIN_8
#define L_RELAY_2_GPIO_Port GPIOC
#define L_RELAY_1_Pin GPIO_PIN_9
#define L_RELAY_1_GPIO_Port GPIOC
#define L_RELAY_3_Pin GPIO_PIN_8
#define L_RELAY_3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
