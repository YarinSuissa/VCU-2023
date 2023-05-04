/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "api_common.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
void Error_Handler(void);
extern CAN_HandleTypeDef  hcan1;
extern TIM_HandleTypeDef  htim2;
extern TIM_HandleTypeDef  htim6;
extern TIM_HandleTypeDef  htim7;
extern UART_HandleTypeDef huart2;

#define MAIN_CAN		  &hcan1
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern char buffer[200];
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
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define LED_ERROR_Pin GPIO_PIN_0
#define LED_ERROR_GPIO_Port GPIOC
#define ASMS_STATE_Pin GPIO_PIN_0
#define ASMS_STATE_GPIO_Port GPIOB
#define LED_STAGE_1_Pin GPIO_PIN_1
#define LED_STAGE_1_GPIO_Port GPIOB
#define LED_STAGE_2_Pin GPIO_PIN_2
#define LED_STAGE_2_GPIO_Port GPIOB
#define LED_STAGE_3_Pin GPIO_PIN_10
#define LED_STAGE_3_GPIO_Port GPIOB
#define LED_TX_Pin GPIO_PIN_14
#define LED_TX_GPIO_Port GPIOB
#define LED_RX_Pin GPIO_PIN_15
#define LED_RX_GPIO_Port GPIOB
#define PRECHARGED_Pin GPIO_PIN_6
#define PRECHARGED_GPIO_Port GPIOC
#define BUZZER_EN_Pin GPIO_PIN_7
#define BUZZER_EN_GPIO_Port GPIOC
#define CAP_ABOVE_TRESH_Pin GPIO_PIN_8
#define CAP_ABOVE_TRESH_GPIO_Port GPIOC
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define FINAL_SD_Pin GPIO_PIN_2
#define FINAL_SD_GPIO_Port GPIOD
#define BRAKE_LIGHT_Pin GPIO_PIN_3
#define BRAKE_LIGHT_GPIO_Port GPIOB
#define SD_RELAY_Pin GPIO_PIN_4
#define SD_RELAY_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
