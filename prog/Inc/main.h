/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct 
{

  uint32_t OffStatusChannel;
  uint32_t EnableChannel;
  uint32_t DIMChannel;
  uint32_t CurrentChannel;
  
   
}Channel_t;  
//******************
//
// DESCRIPTION:
//  Общие настройки для устройства
//
// CREATED: 13.09.2020, by Ierixon-HP
//
// FILE: main.h
//
typedef struct 
{
  uint32_t BaudRate;
  uint32_t  SlaveAddress;
  Channel_t Channels[25];  
   
}settings_t;
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
#define DIM_CH6_Pin GPIO_PIN_2
#define DIM_CH6_GPIO_Port GPIOE
#define DIM_CH7_Pin GPIO_PIN_3
#define DIM_CH7_GPIO_Port GPIOE
#define DIM_CH8_Pin GPIO_PIN_4
#define DIM_CH8_GPIO_Port GPIOE
#define DIM_CH9_Pin GPIO_PIN_5
#define DIM_CH9_GPIO_Port GPIOE
#define DIM_CH10_Pin GPIO_PIN_9
#define DIM_CH10_GPIO_Port GPIOF
#define DIM_CH11_Pin GPIO_PIN_10
#define DIM_CH11_GPIO_Port GPIOF
#define Isens_CH1_Pin GPIO_PIN_0
#define Isens_CH1_GPIO_Port GPIOC
#define Isens_CH2_Pin GPIO_PIN_1
#define Isens_CH2_GPIO_Port GPIOC
#define Isens_CH3_Pin GPIO_PIN_2
#define Isens_CH3_GPIO_Port GPIOC
#define Isens_CH4_Pin GPIO_PIN_3
#define Isens_CH4_GPIO_Port GPIOC
#define Isens_CH5_Pin GPIO_PIN_2
#define Isens_CH5_GPIO_Port GPIOF
#define Isens_CH6_Pin GPIO_PIN_0
#define Isens_CH6_GPIO_Port GPIOA
#define Isens_CH7_Pin GPIO_PIN_1
#define Isens_CH7_GPIO_Port GPIOA
#define Isens_CH8_Pin GPIO_PIN_2
#define Isens_CH8_GPIO_Port GPIOA
#define Isens_CH9_Pin GPIO_PIN_3
#define Isens_CH9_GPIO_Port GPIOA
#define Isens_CH10_Pin GPIO_PIN_4
#define Isens_CH10_GPIO_Port GPIOF
#define Isens_CH11_Pin GPIO_PIN_4
#define Isens_CH11_GPIO_Port GPIOA
#define Isens_CH12_Pin GPIO_PIN_5
#define Isens_CH12_GPIO_Port GPIOA
#define DIM_CH12_Pin GPIO_PIN_6
#define DIM_CH12_GPIO_Port GPIOA
#define DIM_CH13_Pin GPIO_PIN_7
#define DIM_CH13_GPIO_Port GPIOA
#define Isens_CH13_Pin GPIO_PIN_8
#define Isens_CH13_GPIO_Port GPIOE
#define DIM_CH14_Pin GPIO_PIN_9
#define DIM_CH14_GPIO_Port GPIOE
#define DIM_CH15_Pin GPIO_PIN_11
#define DIM_CH15_GPIO_Port GPIOE
#define DIM_CH16_Pin GPIO_PIN_13
#define DIM_CH16_GPIO_Port GPIOE
#define Isens_CH14_Pin GPIO_PIN_14
#define Isens_CH14_GPIO_Port GPIOE
#define Isens_CH15_Pin GPIO_PIN_15
#define Isens_CH15_GPIO_Port GPIOE
#define DIM_CH17_Pin GPIO_PIN_10
#define DIM_CH17_GPIO_Port GPIOB
#define DIM_CH18_Pin GPIO_PIN_11
#define DIM_CH18_GPIO_Port GPIOB
#define Isens_CH16_Pin GPIO_PIN_12
#define Isens_CH16_GPIO_Port GPIOB
#define Isens_CH17_Pin GPIO_PIN_14
#define Isens_CH17_GPIO_Port GPIOB
#define Isens_CH18_Pin GPIO_PIN_15
#define Isens_CH18_GPIO_Port GPIOB
#define Isens_CH19_Pin GPIO_PIN_8
#define Isens_CH19_GPIO_Port GPIOD
#define Isens_CH20_Pin GPIO_PIN_9
#define Isens_CH20_GPIO_Port GPIOD
#define Isens_CH21_Pin GPIO_PIN_10
#define Isens_CH21_GPIO_Port GPIOD
#define Isens_CH22_Pin GPIO_PIN_11
#define Isens_CH22_GPIO_Port GPIOD
#define Isens_CH23_Pin GPIO_PIN_12
#define Isens_CH23_GPIO_Port GPIOD
#define Isens_CH24_Pin GPIO_PIN_13
#define Isens_CH24_GPIO_Port GPIOD
#define Isens_CH25_Pin GPIO_PIN_14
#define Isens_CH25_GPIO_Port GPIOD
#define DIM_CH19_Pin GPIO_PIN_15
#define DIM_CH19_GPIO_Port GPIOD
#define DIM_CH20_Pin GPIO_PIN_6
#define DIM_CH20_GPIO_Port GPIOC
#define DIM_CH21_Pin GPIO_PIN_7
#define DIM_CH21_GPIO_Port GPIOC
#define DIM_CH22_Pin GPIO_PIN_8
#define DIM_CH22_GPIO_Port GPIOC
#define DIM_CH23_Pin GPIO_PIN_9
#define DIM_CH23_GPIO_Port GPIOC
#define DIM_CH24_Pin GPIO_PIN_11
#define DIM_CH24_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_15
#define SPI1_CS_GPIO_Port GPIOA
#define SPI3_CS_Pin GPIO_PIN_2
#define SPI3_CS_GPIO_Port GPIOD
#define DIM_CH1_Pin GPIO_PIN_3
#define DIM_CH1_GPIO_Port GPIOD
#define DIM_CH2_Pin GPIO_PIN_4
#define DIM_CH2_GPIO_Port GPIOD
#define OE_Pin GPIO_PIN_7
#define OE_GPIO_Port GPIOD
#define MR_Pin GPIO_PIN_4
#define MR_GPIO_Port GPIOB
#define DIM_CH3_Pin GPIO_PIN_6
#define DIM_CH3_GPIO_Port GPIOB
#define DIM_CH4_Pin GPIO_PIN_7
#define DIM_CH4_GPIO_Port GPIOB
#define DIM_CH5_Pin GPIO_PIN_8
#define DIM_CH5_GPIO_Port GPIOB
#define SPI3_HOLD_Pin GPIO_PIN_9
#define SPI3_HOLD_GPIO_Port GPIOB
#define SPI3_WP_Pin GPIO_PIN_0
#define SPI3_WP_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
