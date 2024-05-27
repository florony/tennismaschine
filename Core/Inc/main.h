/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SPEED_Pin GPIO_PIN_0
#define SPEED_GPIO_Port GPIOC
#define SPIN_Pin GPIO_PIN_1
#define SPIN_GPIO_Port GPIOC
#define ANGLE_Pin GPIO_PIN_2
#define ANGLE_GPIO_Port GPIOC
#define POS_PULSE_Pin GPIO_PIN_1
#define POS_PULSE_GPIO_Port GPIOA
#define SW_1_Pin GPIO_PIN_4
#define SW_1_GPIO_Port GPIOA
#define LED_RED_Pin GPIO_PIN_7
#define LED_RED_GPIO_Port GPIOA
#define Input_1_Pin GPIO_PIN_5
#define Input_1_GPIO_Port GPIOC
#define SW_2_Pin GPIO_PIN_0
#define SW_2_GPIO_Port GPIOB
#define BDRV_FAIL_Pin GPIO_PIN_6
#define BDRV_FAIL_GPIO_Port GPIOC
#define TDRV_FAIL_Pin GPIO_PIN_8
#define TDRV_FAIL_GPIO_Port GPIOC
#define BDRV_DIR_Pin GPIO_PIN_9
#define BDRV_DIR_GPIO_Port GPIOC
#define TDRV_PULSE_Pin GPIO_PIN_8
#define TDRV_PULSE_GPIO_Port GPIOA
#define BDRV_PULSE_Pin GPIO_PIN_9
#define BDRV_PULSE_GPIO_Port GPIOA
#define LED_YELLOW_Pin GPIO_PIN_11
#define LED_YELLOW_GPIO_Port GPIOA
#define LED_GREEN_Pin GPIO_PIN_12
#define LED_GREEN_GPIO_Port GPIOA
#define PGM_1_Pin GPIO_PIN_10
#define PGM_1_GPIO_Port GPIOC
#define PGM_3_Pin GPIO_PIN_11
#define PGM_3_GPIO_Port GPIOC
#define PGM_2_Pin GPIO_PIN_12
#define PGM_2_GPIO_Port GPIOC
#define POS_DIR_Pin GPIO_PIN_7
#define POS_DIR_GPIO_Port GPIOB
#define TDRV_DIR_Pin GPIO_PIN_8
#define TDRV_DIR_GPIO_Port GPIOB
#define Input_2_Pin GPIO_PIN_9
#define Input_2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
