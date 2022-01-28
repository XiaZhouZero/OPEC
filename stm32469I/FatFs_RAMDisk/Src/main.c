/**
  ******************************************************************************
  * @file    FatFs/FatFs_RAMDisk/Src/main.c 
  * @author  MCD Application Team
  * @brief   Main program body
  *          This sample code shows how to use FatFs with RAM disk drive.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#ifdef TIMER_BASELINE
unsigned int TS_0,TS_1;//timestamp
volatile unsigned int *DWT_CYCCNT  ;
volatile unsigned int *DWT_CONTROL ;
volatile unsigned int *SCB_DEMCR   ;

void reset_timer(){
    DWT_CYCCNT   = (volatile unsigned int *)0xE0001004; //address of the register
    DWT_CONTROL  = (volatile unsigned int *)0xE0001000; //address of the register
    SCB_DEMCR    = (volatile unsigned int *)0xE000EDFC; //address of the register
    *SCB_DEMCR   = *SCB_DEMCR | 0x01000000;
    *DWT_CYCCNT  = 0; // reset the counter
    *DWT_CONTROL = 0; 
}

void start_timer(){
    *DWT_CONTROL = *DWT_CONTROL | 1 ; // enable the counter
    TS_0 = *DWT_CYCCNT;
}

void stop_timer(){
    TS_1 = *DWT_CYCCNT;
    *DWT_CONTROL = *DWT_CONTROL | 0 ; // disable the counter    
}

unsigned int get_cycles(){
    return TS_1 - TS_0;
}

void info_brkpt(){ 
    printf("\n");
}

uint32_t ExeTime[32] = {0};
#endif
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
FATFS RAMDISKFatFs;  /* File system object for RAM disk logical drive */
FIL MyFile;          /* File object */
char RAMDISKPath[4]; /* RAM disk logical drive path */
static uint8_t buffer[_MAX_SS]; /* a work buffer for the f_mkfs() */
FRESULT res;                                          /* FatFs function common result code */
uint32_t byteswritten, bytesread;                     /* File write/read counts */
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/

void System_Init()
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();
  
  /* Configure the system clock to 180 MHz */
  SystemClock_Config();
  
  /* Configure LED1 and LED3 */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED3);
}


void Link_Driver()
{
  if(FATFS_LinkDriver(&SDRAMDISK_Driver, RAMDISKPath) != 0)
  {
    Error_Handler();
  }
}


void Register_FS()
{
  if(f_mount(&RAMDISKFatFs, (TCHAR const*)RAMDISKPath, 0) != FR_OK)
  {
    /* FatFs Initialization Error */
    Error_Handler();
  }
}


void Create_FAT_FS()
{
  if(f_mkfs((TCHAR const*)RAMDISKPath, FM_ANY, 0, buffer, sizeof(buffer)) != FR_OK)
  {     
    Error_Handler();
  }
}


void Open_File()
{
  if(f_open(&MyFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) 
  {
    /* 'STM32.TXT' file Open for write Error */
    Error_Handler();
  }
}


void Write_to_File()
{
  uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */

  res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);
  
  if((byteswritten == 0) || (res != FR_OK))
  {
    /* 'STM32.TXT' file Write or EOF Error */
    Error_Handler();
  }
  else {
    f_close(&MyFile);
  }
}


void Open_File_Read()
{
  if(f_open(&MyFile, "STM32.TXT", FA_READ) != FR_OK)
  {
    /* 'STM32.TXT' file Open for read Error */
    Error_Handler();
  }
}


void Read_File()
{
  uint8_t rtext[100];                                   /* File read buffer */
  res = f_read(&MyFile, rtext, sizeof(rtext), (UINT*)&bytesread);
  
  if((bytesread == 0) || (res != FR_OK)) /* EOF or Error */
  {
    /* 'STM32.TXT' file Read or EOF Error */
    Error_Handler();
  }
  else {
    f_close(&MyFile);
  }
}


void Compare_Data()
{
  if ((bytesread != byteswritten))
  {                
    /* Read data is different from the expected data */
    Error_Handler(); 
  }
  else
  {
    /* Success of the demo: no error occurrence */
    BSP_LED_On(LED1);
  }
}

void Unlink_Driver()
{
  FATFS_UnLinkDriver(RAMDISKPath);
}


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{  
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  System_Init();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[1] = get_cycles();
#endif
  /*##-1- Link the RAM disk I/O driver #######################################*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Link_Driver();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[2] = get_cycles();
#endif

  /*##-2- Register the file system object to the FatFs module ##############*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Register_FS();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[3] = get_cycles();
#endif
  /*##-3- Create a FAT file system (format) on the logical drive #########*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Create_FAT_FS();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[4] = get_cycles();
#endif

  /*##-4- Create and Open a new text file object with write access #####*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Open_File();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[5] = get_cycles();
#endif

  /*##-5- Write data to the text file ################################*/
  /*##-6- Close the open text file #################################*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Write_to_File();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[6] = get_cycles();
#endif

  /*##-7- Open the text file object with read access ###############*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Open_File_Read();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[7] = get_cycles();
#endif

  /*##-8- Read data from the text file ###########################*/
  /*##-9- Close the open text file #############################*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Read_File();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[8] = get_cycles();
#endif

  /*##-10- Compare read data with the expected data ############*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Compare_Data();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[9] = get_cycles();
#endif

  /*##-11- Unlink the RAM disk I/O driver ####################################*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Unlink_Driver();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[10] = get_cycles();
#endif

  /* Infinite loop */
  while (1)
  {;}
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            PLL_R                          = 6
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 6;
  
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* Activate the OverDrive to reach the 180 MHz Frequency */  
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Turn LED3 on */
  BSP_LED_On(LED3);
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
