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
#include "stm32h7xx_hal.h"

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
#define TP_CS_Pin GPIO_PIN_8
#define TP_CS_GPIO_Port GPIOI
#define KEY2_Pin GPIO_PIN_13
#define KEY2_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_2
#define KEY1_GPIO_Port GPIOH
#define KEY0_Pin GPIO_PIN_3
#define KEY0_GPIO_Port GPIOH
#define PCF_SCL_Pin GPIO_PIN_4
#define PCF_SCL_GPIO_Port GPIOH
#define PCF_SDA_Pin GPIO_PIN_5
#define PCF_SDA_GPIO_Port GPIOH
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOB
#define LED0_Pin GPIO_PIN_1
#define LED0_GPIO_Port GPIOB
#define TP_SCK_Pin GPIO_PIN_6
#define TP_SCK_GPIO_Port GPIOH
#define TP_PEN_Pin GPIO_PIN_7
#define TP_PEN_GPIO_Port GPIOH
#define TP_SDA_Pin GPIO_PIN_3
#define TP_SDA_GPIO_Port GPIOI
#define DMCI_RESET_Pin GPIO_PIN_15
#define DMCI_RESET_GPIO_Port GPIOA
#define SCCB_SDA_Pin GPIO_PIN_3
#define SCCB_SDA_GPIO_Port GPIOB
#define SCCB_SCL_Pin GPIO_PIN_4
#define SCCB_SCL_GPIO_Port GPIOB
#define LCD_BL_Pin GPIO_PIN_5
#define LCD_BL_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
