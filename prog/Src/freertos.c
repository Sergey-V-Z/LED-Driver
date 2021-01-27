/* USER CODE BEGIN Header */
/**
******************************************************************************
* File Name          : freertos.c
* Description        : Code for freertos applications
******************************************************************************
* @attention
*
* <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
* All rights reserved.</center></h2>
*
* This software component is licensed by ST under Ultimate Liberty license
* SLA0044, the "License"; You may not use this file except in compliance with
* the License. You may obtain a copy of the License at:
*                             www.st.com/SLA0044
*
******************************************************************************
*/
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "mb.h"
#include "mbport.h"

using namespace std;

#define start_addr               10
#define addr_dim_chanels        sart_addr
#define addr_enable_chanels     addr_dim_chanels + 25
#define end_w_and_start_r       addr_enable_chanels + 25
#define addr_chanels_is_down    end_w_and_start_r
#define end_r                   addr_chanels_is_down + 25
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern SPI_HandleTypeDef hspi3;
extern SPI_HandleTypeDef hspi1;
extern settings_t settings;
int start = 1;

uint16_t dim_chanels[25] = {0,};
uint16_t enable_chanels[25] = {0,};
uint16_t chanels_is_down[25] = {0,};

uint32_t data_for_hc595 = 0;
/* USER CODE END Variables */
osThreadId MainTaskHandle;
osThreadId ModBus_TaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
//   extern "C"
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void ModBusTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
extern "C" void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
   *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
   *ppxIdleTaskStackBuffer = &xIdleStack[0];
   *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
   /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
   
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
   /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
   /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
   /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
   /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of MainTask */
  osThreadDef(MainTask, StartDefaultTask, osPriorityNormal, 0, 512);
  MainTaskHandle = osThreadCreate(osThread(MainTask), NULL);

  /* definition and creation of ModBus_Task */
  osThreadDef(ModBus_Task, ModBusTask, osPriorityNormal, 0, 512);
  ModBus_TaskHandle = osThreadCreate(osThread(ModBus_Task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
   /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief  Function implementing the defaultTask thread.
* @param  argument: Not used 
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
   uint8_t TxBuff[4] = {0xff,};
   //uint8_t RxBuff[50] = {0};
   uint16_t Size = 4;
   HAL_StatusTypeDef StatusSPI1;
   HAL_GPIO_WritePin(MR_GPIO_Port, MR_Pin, GPIO_PIN_SET);
   HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, GPIO_PIN_RESET);
   
   /* Infinite loop */
   for(;;)
   {
     
    TxBuff[0] = *(uint8_t*)&data_for_hc595;
    TxBuff[1] = *(uint8_t*)(&data_for_hc595)+1;
    TxBuff[2] = *(uint8_t*)(&data_for_hc595)+2;
    TxBuff[3] = *(uint8_t*)(&data_for_hc595)+3;
    
     HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
     StatusSPI1 = HAL_SPI_Transmit(&hspi1, TxBuff, Size, 100);
     StatusSPI1 = StatusSPI1;
     HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
     
      osDelay(1);
   }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_ModBusTask */
/**
* @brief Function implementing the ModBus_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ModBusTask */
void ModBusTask(void const * argument)
{
  /* USER CODE BEGIN ModBusTask */
   /* Infinite loop */
   eMBErrorCode eStatus = eMBInit( MB_RTU, settings.SlaveAddress, 3, settings.BaudRate, MB_PAR_NONE );
   eStatus = eMBEnable();
   //HAL_TIM_Base_Start_IT(&htim17);
   for(;;)
   {
      eMBPoll();
      //taskYIELD();
   }
  /* USER CODE END ModBusTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/*description https://www.freemodbus.org/api/group__modbus__registers.html*/
//0x04
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
   eMBErrorCode    eStatus = MB_ENOERR;
   
   return eStatus;
}
//0x03 0x06 0x10
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
   
   //uint8_t CMD[5] = {0};
   volatile HAL_StatusTypeDef status;
   
   if(usAddress == 0 ){}
   else{usAddress--;} 
   
   //   // dim_chanels range
   //   if((usAddress >= addr_dim_chanels) && (usAddress < addr_enable_chanels)){
   //      for(int i = addr_dim_chanels - start_addr; i < usNRegs; ++i)
   //      {
   //        dim_chanels[]
   //      }
   //
   //   }
   //   // addr_enable_chanels range
   //   else if((usAddress >= addr_enable_chanels) && (usAddress < addr_chanels_is_down)){
   //      
   //   }
   //   //addr_chanels_is_down range
   //   else if((usAddress >= addr_chanels_is_down) && (usAddress < end_r) && (eMode == MB_REG_READ)){
   //      
   //   }
   
   switch (eMode)
   {
     case MB_REG_READ:
      {	
         switch (usAddress)
         {
           case 0: //  Stop/Start
            {	
               
               break;
            }
           case 1: // Dir
            {	
               
               break;
            }
           case 2: //Status start/stop
            {	
               
               break;
            }
           case 3: //Status Dir
            {	
               
               break;
            }
           case 4: // RPM
            {	
               
               break;
            }
           case 5: // 
            {	
               break;
            }
           case 6: // 
            {	
               
               break;
            }
           case 7: // 
            {	
               break;
            }
           case 8: 
            {	
               
               break;
            }
           case 9: 
            {	
               
               break;
            }
           case 10: // start dim
            {	
               
               break;
            }
           case 11: 
            {	
               
               break;
            }
           case 12: 
            {	
               
               break;
            }
           case 13: 
            {	
               
               break;
            }
           case 14: 
            {	
               
               break;
            }
           case 15: 
            {	
               
               break;
            }
           case 16: 
            {	
               
               break;
            }
           case 17: 
            {	
               
               break;
            }
           case 18: 
            {	
               
               break;
            }
           case 19: 
            {	
               
               break;
            }
           case 20: 
            {	
               
               break;
            }
           case 21: 
            {	
               
               break;
            }
           case 22: 
            {	
               
               break;
            }
           case 23: 
            {	
               
               break;
            }
           case 24: 
            {	
               
               break;
            }
           case 25: 
            {	
               
               break;
            }
           case 26: 
            {	
               
               break;
            }
           case 27: 
            {	
               
               break;
            }
           case 28: 
            {	
               
               break;
            } 
           case 29: 
            {	
               
               break;
            }
           case 30: 
            {	
               
               break;
            }
           case 31: 
            {	
               
               break;
            }
           case 32: 
            {	
               
               break;
            }
           case 33: 
            {	
               
               break;
            }
           case 34: // end dim
            {	
               
               break;
            }
           case 35: // start enable
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 0);
               }else{
                  data_for_hc595 &= ~(1 << 0);
               }
               break;
            }
           case 36: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 1);
               }else{
                  data_for_hc595 &= ~(1 << 1);
               }               
               break;
            }  
           case 37: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 2);
               }else{
                  data_for_hc595 &= ~(1 << 2);
               }               
               break;
            }
           case 38: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 3);
               }else{
                  data_for_hc595 &= ~(1 << 3);
               }               
               break;
            }
           case 49: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 4);
               }else{
                  data_for_hc595 &= ~(1 << 4);
               }               
               break;
            }
           case 40: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 5);
               }else{
                  data_for_hc595 &= ~(1 << 5);
               }               
               break;
            }
           case 41: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 6);
               }else{
                  data_for_hc595 &= ~(1 << 6);
               }               
               break;
            }
           case 42: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 7);
               }else{
                  data_for_hc595 &= ~(1 << 7);
               }               
               break;
            }
           case 43: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 8);
               }else{
                  data_for_hc595 &= ~(1 << 8);
               }               
               break;
            }
           case 44: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 9);
               }else{
                  data_for_hc595 &= ~(1 << 9);
               }               
               break;
            }
           case 45: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 10);
               }else{
                  data_for_hc595 &= ~(1 << 10);
               }               
               break;
            }
           case 46: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 11);
               }else{
                  data_for_hc595 &= ~(1 << 11);
               }              
               break;
            }  
           case 47: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 12);
               }else{
                  data_for_hc595 &= ~(1 << 12);
               }               
               break;
            }
           case 48: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 13);
               }else{
                  data_for_hc595 &= ~(1 << 13);
               }               
               break;
            }
           case 49: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 14);
               }else{
                  data_for_hc595 &= ~(1 << 14);
               }               
               break;
            }
           case 50: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 15);
               }else{
                  data_for_hc595 &= ~(1 << 15);
               }               
               break;
            }
           case 51: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 16);
               }else{
                  data_for_hc595 &= ~(1 << 16);
               }               
               break;
            }
           case 52: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 17);
               }else{
                  data_for_hc595 &= ~(1 << 17);
               }               
               break;
            }
           case 53: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 18);
               }else{
                  data_for_hc595 &= ~(1 << 18);
               }               
               break;
            }
           case 54: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 19);
               }else{
                  data_for_hc595 &= ~(1 << 19);
               }               
               break;
            }
           case 55: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 20);
               }else{
                  data_for_hc595 &= ~(1 << 20);
               }               
               break;
            }
           case 56: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 21);
               }else{
                  data_for_hc595 &= ~(1 << 21);
               }               
               break;
            }  
           case 57: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 22);
               }else{
                  data_for_hc595 &= ~(1 << 22);
               }               
               break;
            }
           case 58: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 23);
               }else{
                  data_for_hc595 &= ~(1 << 23);
               }               
               break;
            }
           case 59: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (1 << 24);
               }else{
                  data_for_hc595 &= ~(1 << 24);
               }               
               break;
            }
           default:
            {	
               eStatus = MB_ENOREG;
               break;
            }
         }
         
         break;
      }
     case MB_REG_WRITE:
      {	
         
         switch (usAddress)
         {
           case 0: //  Stop/Start
            {	
               
               break;
            }
           case 1: // Dir
            {	
               
               break;
            }
           case 2: //Status start/stop
            {	
               
               break;
            }
           case 3: //Status Dir
            {	
               
               break;
            }
           case 4: // RPM
            {	
               
               break;
            }
           case 5: // 
            {	
               break;
            }
           case 6: // 
            {	
               
               break;
            }
           case 7: // 
            {	
               break;
            }
           case 8: 
            {	
               
               break;
            }
           case 9: 
            {	
               
               break;
            }
           case 10: 
            {	
               
               break;
            }
           case 11: 
            {	
               
               break;
            }
           case 12: 
            {	
               
               break;
            }
           case 13: 
            {	
               
               break;
            }
           case 14: 
            {	
               
               break;
            }
           case 15: 
            {	
               
               break;
            }
           case 16: 
            {	
               
               break;
            }
           case 17: 
            {	
               
               break;
            }
           case 18: 
            {	
               
               break;
            }
           case 19: 
            {	
               
               break;
            }
           case 20: 
            {	
               
               break;
            }
           case 21: 
            {	
               
               break;
            }
           case 22: 
            {	
               
               break;
            }
           case 23: 
            {	
               
               break;
            }
           case 24: 
            {	
               
               break;
            }
           case 25: 
            {	
               
               break;
            }
           case 26: 
            {	
               
               break;
            }
           case 27: 
            {	
               
               break;
            }
           case 28: 
            {	
               
               break;
            } 
           case 29: 
            {	
               
               break;
            }
           case 30: 
            {	
               
               break;
            }
           case 31: 
            {	
               
               break;
            }
           case 32: 
            {	
               
               break;
            }
           case 33: 
            {	
               
               break;
            }
           case 34: 
            {	
               
               break;
            }
           case 35: 
            {	
               
               break;
            }
           case 36: 
            {	
               
               break;
            }
           case 37: 
            {	
               
               break;
            }  
           case 38: 
            {	
               
               break;
            }
           case 39: 
            {	
               
               break;
            }
           case 40: 
            {	
               
               break;
            }
           case 41: 
            {	
               
               break;
            }
           case 42: 
            {	
               
               break;
            }
           case 43: 
            {	
               
               break;
            }
           case 44: 
            {	
               
               break;
            }
           case 45: 
            {	
               
               break;
            }
           case 46: 
            {	
               
               break;
            }
           case 47: 
            {	
               
               break;
            }  
           case 48: 
            {	
               
               break;
            }
           case 49: 
            {	
               
               break;
            }
           case 50: 
            {	
               
               break;
            }
           case 51: 
            {	
               
               break;
            }
           case 52: 
            {	
               
               break;
            }
           case 53: 
            {	
               
               break;
            }
           case 54: 
            {	
               
               break;
            }
           case 55: 
            {	
               
               break;
            }
           case 56: 
            {	
               
               break;
            }
           case 57: 
            {	
               
               break;
            }  
           case 58: 
            {	
               
               break;
            }
           case 59: 
            {	
               
               break;
            }
           case 60: // chanels is down
            {	
               
               break;
            }
           case 61: 
            {	
               
               break;
            }
           case 62: 
            {	
               
               break;
            }
           case 63: 
            {	
               
               break;
            }
           case 64: 
            {	
               
               break;
            }
           case 65: 
            {	
               
               break;
            }
           case 66: 
            {	
               
               break;
            }
           case 67: 
            {	
               
               break;
            }  
           case 68: 
            {	
               
               break;
            }
           case 69: 
            {	
               
               break;
            }
           case 70: 
            {	
               
               break;
            }
           case 71: 
            {	
               
               break;
            }
           case 72: 
            {	
               
               break;
            }
           case 73: 
            {	
               
               break;
            }
           case 74: 
            {	
               
               break;
            }
           case 75: 
            {	
               
               break;
            }
           case 76: 
            {	
               
               break;
            }
           case 77: 
            {	
               
               break;
            }  
           case 78: 
            {	
               
               break;
            }
           case 79: 
            {	
               
               break;
            }
           case 80: 
            {	
               
               break;
            }
           case 81: 
            {	
               
               break;
            }
           case 82: 
            {	
               
               break;
            }
           case 83: 
            {	
               
               break;
            }
           case 84: 
            {	
               
               break;
            }
           default:
            {	
               eStatus = MB_ENOREG;
               break;
            }
         }
         break;
      }
     default:
      {	
         eStatus = MB_EINVAL;
         break;
      }
   }
   
   
   eMBErrorCode    eStatus = MB_ENOERR;
   
   
   return eStatus;
}

// 0x01 0x0f 0x05
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
   return MB_ENOREG;
}
//0x02
eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
   return MB_ENOREG;
}       
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
