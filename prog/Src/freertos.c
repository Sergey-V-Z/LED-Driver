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
#include "flash_spi.h"

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
#define CH1 TIM2->CCR1 //CH1
#define CH2 TIM2->CCR2 //CH2
#define CH3 TIM4->CCR1 //CH3
#define CH4 TIM4->CCR4 //CH4
#define CH5 TIM4->CCR3 //CH5

#define CH6 TIM3->CCR1 //CH6
#define CH7 TIM3->CCR2 //CH7
#define CH8 TIM3->CCR3 //CH8
#define CH9 TIM3->CCR4 //CH9
#define CH10 TIM15->CCR1 //CH10

#define CH11 TIM15->CCR2 //CH11
#define CH12 TIM16->CCR1 //CH12
#define CH13 TIM17->CCR1 //CH13
#define CH14 TIM1->CCR1 //CH14
#define CH15 TIM1->CCR2 //CH15

#define CH16 TIM1->CCR3 //CH16
#define CH17 TIM2->CCR3 //CH17
#define CH18 TIM2->CCR4 //CH18
#define CH19 TIM4->CCR4 //CH19
#define CH20 TIM8->CCR1 //CH20

#define CH21 TIM8->CCR2 //CH21
#define CH22 TIM8->CCR3 //CH22
#define CH23 TIM8->CCR4 //CH23
#define CH24 TIM1->CCR4 //CH24

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc4;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi3;
extern settings_t settings;
extern flash *FlashP;
int start = 1;

uint16_t dim_chanels[25] = {0,};
uint16_t enable_chanels[25] = {0,};
uint16_t chanels_is_down[25] = {0,};
//bufers for adc
uint16_t CH1_CH10[10] = {0,};
uint16_t CH11_CH12[2] = {0,};
uint16_t CH13_CH25[13] = {0,};

uint32_t data_for_hc595 = 0xffffffff;
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
   
   // for hc595
   HAL_StatusTypeDef StatusSPI1;
   HAL_GPIO_WritePin(MR_GPIO_Port, MR_Pin, GPIO_PIN_SET);
   HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, GPIO_PIN_RESET);
   
   CH1 = settings.Channels[0].DIMChannel;
   CH2 = settings.Channels[1].DIMChannel;
   CH3 = settings.Channels[2].DIMChannel;
   CH4 = settings.Channels[3].DIMChannel;
   CH5 = settings.Channels[4].DIMChannel;
   CH6 = settings.Channels[5].DIMChannel;
   CH7 = settings.Channels[6].DIMChannel;
   CH8 = settings.Channels[7].DIMChannel;
   CH9 = settings.Channels[8].DIMChannel;
   CH10 = settings.Channels[9].DIMChannel;
   CH11 = settings.Channels[10].DIMChannel;
   CH12 = settings.Channels[11].DIMChannel;
   CH13 = settings.Channels[12].DIMChannel;
   CH14 = settings.Channels[13].DIMChannel;
   CH15 = settings.Channels[14].DIMChannel;
   CH16 = settings.Channels[15].DIMChannel;
   CH17 = settings.Channels[16].DIMChannel;
   CH18 = settings.Channels[17].DIMChannel;
   CH19 = settings.Channels[18].DIMChannel;
   CH20 = settings.Channels[19].DIMChannel;
   CH21 = settings.Channels[20].DIMChannel;
   CH22 = settings.Channels[21].DIMChannel;
   CH23 = settings.Channels[22].DIMChannel;
   CH24 = settings.Channels[23].DIMChannel;

   
   // for adc
   HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&CH1_CH10,10);
   HAL_ADC_Start_DMA(&hadc2,(uint32_t*)&CH11_CH12,2);
   HAL_ADC_Start_DMA(&hadc4,(uint32_t*)&CH13_CH25,1);
   
   /* Infinite loop */
   for(;;)
   {
      
      HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
      StatusSPI1 = HAL_SPI_Transmit(&hspi1, (uint8_t*)&data_for_hc595, 4, 100);
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

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc1)
{
   
}

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
   eMBErrorCode    eStatus = MB_ENOERR;
   //uint8_t CMD[5] = {0};
   volatile HAL_StatusTypeDef status;
   
   if(usAddress == 0 ){}
   else{usAddress--;} 
   
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
              uint16_t temp = CH1;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 11: 
            {	
              uint16_t temp = CH2;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 12: 
            {	
              uint16_t temp = CH3;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 13: 
            {	
              uint16_t temp = CH4;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 14: 
            {	
              uint16_t temp = CH5;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 15: 
            {	
              uint16_t temp = CH6;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 16: 
            {	
              uint16_t temp = CH7;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 17: 
            {	
              uint16_t temp = CH8;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 18: 
            {	
              uint16_t temp = CH9;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 19: 
            {	
              uint16_t temp = CH10;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 20: 
            {	
              uint16_t temp = CH11;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 21: 
            {	
              uint16_t temp = CH12;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 22: 
            {	
              uint16_t temp = CH13;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 23: 
            {	
              uint16_t temp = CH14;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 24: 
            {	
              uint16_t temp = CH15;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 25: 
            {	
              uint16_t temp = CH16;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 26: 
            {	
              uint16_t temp = CH17;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 27: 
            {	
              uint16_t temp = CH18;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 28: 
            {	
              uint16_t temp = CH19;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            } 
           case 29: 
            {	
              uint16_t temp = CH20;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 30: 
            {	
              uint16_t temp = CH21;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 31: 
            {	
              uint16_t temp = CH22;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 32: 
            {	
              uint16_t temp = CH23;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 33: 
            {	
              uint16_t temp = CH24;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 34: // end dim
            {
              uint16_t temp = CH24;
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;
               break;
            }
           case 35: // start enable
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 31);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;
              
               break;
            }
           case 36: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 30);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }  
           case 37: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 29);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;              
               break;
            }
           case 38: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 28);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;             
               break;
            }
           case 39: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 27);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 40: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 26);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;              
               break;
            }
           case 41: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 25);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 42: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 24);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;              
               break;
            }
           case 43: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 23);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 44: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 22);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 45: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 21);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;            
               break;
            }
           case 46: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 20);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;             
               break;
            }  
           case 47: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 19);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;              
               break;
            }
           case 48: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 18);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;              
               break;
            }
           case 49: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 17);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 50: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 16);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;                
               break;
            }
           case 51: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 15);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 52: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 14);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 53: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 13);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 54: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 12);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;               
               break;
            }
           case 55: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 11);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;              
               break;
            }
           case 56: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 10);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;             
               break;
            }  
           case 57: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 9);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;              
               break;
            }
           case 58: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 8);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;              
               break;
            }
           case 59: 
            {	
               uint16_t temp = data_for_hc595 & (uint32_t)(1 << 7);
              *(pucRegBuffer) = (temp & 0xff00)>>8;
              *(pucRegBuffer+1) = temp & 0x00ff;              
               break;
            }
           case 60: // chanels is down
            {	//V=ADCval*VALmax/ADCmax. 4095
               
               //*pucRegBuffer = ((UCHAR*) &CH1_CH10[0])+1;
               //*pucRegBuffer +1 = ((UCHAR*) &CH1_CH10[0]);
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
           case 8: // save
            {	
               FlashP->Write(settings);
               break;
            }
           case 9: 
            {	
               
               break;
            }
           case 10: // start PWM
            {
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH1 = temp;
                settings.Channels[0].DIMChannel = temp;
               }
               break;
            }
           case 11: 
            {	
                uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH2 = temp;
                settings.Channels[1].DIMChannel = temp;
               }              
               break;
            }
           case 12: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH3 = temp;
                settings.Channels[2].DIMChannel = temp;
               }               
               break;
            }
           case 13: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH4 = temp;
                settings.Channels[3].DIMChannel = temp;
               }               
               break;
            }
           case 14: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH5 = temp;
                settings.Channels[4].DIMChannel = temp;
               }               
               break;
            }
           case 15: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH6 = temp;
                settings.Channels[5].DIMChannel = temp;
               }               
               break;
            }
           case 16: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH7 = temp;
                settings.Channels[6].DIMChannel = temp;
               }               
               break;
            }
           case 17: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH8 = temp;
                settings.Channels[7].DIMChannel = temp;
               }               
               break;
            }
           case 18: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH9 = temp;
                settings.Channels[8].DIMChannel = temp;
               }               
               break;
            }
           case 19: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH10 = temp;
                settings.Channels[9].DIMChannel = temp;
               }               
               break;
            }
           case 20: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH11 = temp;
                settings.Channels[10].DIMChannel = temp;
               }               
               break;
            }
           case 21: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH12 = temp;
                settings.Channels[11].DIMChannel = temp;
               }               
               break;
            }
           case 22: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH13 = temp;
                settings.Channels[12].DIMChannel = temp;
               }               
               break;
            }
           case 23: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH14 = temp;
                settings.Channels[13].DIMChannel = temp;
               }               
               break;
            }
           case 24: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH15 = temp;
                settings.Channels[14].DIMChannel = temp;
               }               
               break;
            }
           case 25: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH16 = temp;
                settings.Channels[15].DIMChannel = temp;
               }               
               break;
            }
           case 26: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH17 = temp;
                settings.Channels[16].DIMChannel = temp;
               }               
               break;
            }
           case 27: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH18 = temp;
                settings.Channels[17].DIMChannel = temp;
               }               
               break;
            }
           case 28: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH19 = temp;
                settings.Channels[18].DIMChannel = temp;
               }               
               break;
            } 
           case 29: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH20 = temp;
                settings.Channels[19].DIMChannel = temp;
               }               
               break;
            }
           case 30: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH21 = temp;
                settings.Channels[20].DIMChannel = temp;
               }               
               break;
            }
           case 31: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH22 = temp;
                settings.Channels[21].DIMChannel = temp;
               }               
               break;
            }
           case 32: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH23 = temp;
                settings.Channels[22].DIMChannel = temp;
               }               
               break;
            }
           case 33: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH24 = temp;
                settings.Channels[23].DIMChannel = temp;
                settings.Channels[24].DIMChannel = temp;
               }               
               break;
            }
           case 34: 
            {	
               uint16_t temp = 0;
               temp = temp | (*(pucRegBuffer) << 8);
               temp = temp | *(pucRegBuffer+1);
               if((temp > 0) && (temp <= 1000)){
                CH24 = temp;
                settings.Channels[23].DIMChannel = temp;
                settings.Channels[24].DIMChannel = temp;  
               }
               break;
            }        
           case 35: // start enable
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 31);
               }else{
                  data_for_hc595 &= ~(1 << 31);
               }
               break;
            }
           case 36: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 30);
               }else{
                  data_for_hc595 &= ~(1 << 30);
               }               
               break;
            }  
           case 37: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 29);
               }else{
                  data_for_hc595 &= ~(1 << 29);
               }               
               break;
            }
           case 38: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 28);
               }else{
                  data_for_hc595 &= ~(1 << 28);
               }               
               break;
            }
           case 39: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 27);
               }else{
                  data_for_hc595 &= ~(1 << 27);
               }               
               break;
            }
           case 40: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 26);
               }else{
                  data_for_hc595 &= ~(1 << 26);
               }               
               break;
            }
           case 41: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 25);
               }else{
                  data_for_hc595 &= ~(1 << 25);
               }               
               break;
            }
           case 42: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 24);
               }else{
                  data_for_hc595 &= ~(1 << 24);
               }               
               break;
            }
           case 43: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 23);
               }else{
                  data_for_hc595 &= ~(1 << 23);
               }               
               break;
            }
           case 44: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 22);
               }else{
                  data_for_hc595 &= ~(1 << 22);
               }               
               break;
            }
           case 45: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 21);
               }else{
                  data_for_hc595 &= ~(1 << 21);
               }               
               break;
            }
           case 46: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 20);
               }else{
                  data_for_hc595 &= ~(1 << 20);
               }              
               break;
            }  
           case 47: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 19);
               }else{
                  data_for_hc595 &= ~(1 << 19);
               }               
               break;
            }
           case 48: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 18);
               }else{
                  data_for_hc595 &= ~(1 << 18);
               }               
               break;
            }
           case 49: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 17);
               }else{
                  data_for_hc595 &= ~(1 << 17);
               }               
               break;
            }
           case 50: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 16);
               }else{
                  data_for_hc595 &= ~(1 << 16);
               }               
               break;
            }
           case 51: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 15);
               }else{
                  data_for_hc595 &= ~(1 << 15);
               }               
               break;
            }
           case 52: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 14);
               }else{
                  data_for_hc595 &= ~(1 << 14);
               }               
               break;
            }
           case 53: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 13);
               }else{
                  data_for_hc595 &= ~(1 << 13);
               }               
               break;
            }
           case 54: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 12);
               }else{
                  data_for_hc595 &= ~(1 << 12);
               }               
               break;
            }
           case 55: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 11);
               }else{
                  data_for_hc595 &= ~(1 << 11);
               }               
               break;
            }
           case 56: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 10);
               }else{
                  data_for_hc595 &= ~(1 << 10);
               }               
               break;
            }  
           case 57: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 9);
               }else{
                  data_for_hc595 &= ~(1 << 9);
               }               
               break;
            }
           case 58: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 8);
               }else{
                  data_for_hc595 &= ~(1 << 8);
               }               
               break;
            }
           case 59: 
            {	
               if(!*(pucRegBuffer+1)){
                  data_for_hc595 |= (uint32_t)(1 << 7);
               }else{
                  data_for_hc595 &= ~(1 << 7);
               }               
               break;
            }
           case 60: // chanels is down
            {	//V=ADCval*VALmax/ADCmax. 4095
               
               //*pucRegBuffer = ((UCHAR*) &CH1_CH10[0])+1;
               //*pucRegBuffer +1 = ((UCHAR*) &CH1_CH10[0]);
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

