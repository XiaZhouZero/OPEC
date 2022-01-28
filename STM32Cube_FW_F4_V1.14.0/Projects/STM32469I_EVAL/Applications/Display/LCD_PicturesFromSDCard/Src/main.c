/**
  ******************************************************************************
  * @file    Display/LCD_PicturesFromSDCard/Src/main.c
  * @author  MCD Application Team
  * @version V1.0.6
  * @date    04-November-2016
  * @brief   This file provides main program functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright � 2016 STMicroelectronics International N.V.
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
uint32_t ExeTime_1[32] = {0};
uint32_t ExeTime_2[32] = {0};
uint32_t ExeTime_3[32] = {0};
uint32_t ExeTime_4[32] = {0};
uint32_t loop_count = 0;
#endif
/** @addtogroup STM32F4xx_HAL_Applications
  * @{
  */

/** @addtogroup LCD_PicturesFromSDCard
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
FATFS SD_FatFs;  /* File system object for SD card logical drive */
char SD_Path[4]; /* SD card logical drive path */
char* pDirectoryFiles[MAX_BMP_FILES];
char pDirectoryFileNames[MAX_BMP_FILES][MAX_BMP_FILE_NAME];
uint8_t  ubNumberOfFiles = 0;
uint32_t uwBmplen = 0;

/* Internal Buffer defined in SDRAM memory */
uint8_t *uwInternalBuffer = (uint8_t *)INTERNAL_BUFFER_START_ADDRESS;
uint32_t counter = 0, transparency = 0;
uint8_t str[30];
// uwInternalBuffer = (uint8_t *)INTERNAL_BUFFER_START_ADDRESS;

/* Private function prototypes -----------------------------------------------*/
static void LCD_Config(void);
static void SystemClock_Config(void);
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/
void BSP_LED_Init_0()
{
  BSP_LED_Init(LED3);
}


void Link_Driver()
{
  if(FATFS_LinkDriver(&SD_Driver, SD_Path) != 0)
  {
    /* FatFs Initialization Error */
    Error_Handler();
  }
}



void Init_Directory_File_Pointers() {
  for (uint32_t i = 0; i < MAX_BMP_FILES; i++)
  {
    // pDirectoryFiles[i] = malloc(MAX_BMP_FILE_NAME);
    pDirectoryFiles[i] =  &pDirectoryFileNames[i][0];
    if(pDirectoryFiles[i] == NULL)
    {
      /* Set the Text Color */
      BSP_LCD_SetTextColor(LCD_COLOR_RED);

      BSP_LCD_DisplayStringAtLine(8, (uint8_t*)"  Cannot allocate memory ");
      while(1) { ; }
    }
  }
}


void Get_BMP_Filenames() {
  ubNumberOfFiles = Storage_GetDirectoryBitmapFiles("/Media", pDirectoryFiles);

  if (ubNumberOfFiles == 0)
  {
    for (uint32_t i = 0; i < MAX_BMP_FILES; i++)
    {
      // free(pDirectoryFiles[i]);
      pDirectoryFiles[i] = NULL;
    }
    BSP_LCD_DisplayStringAtLine(8, (uint8_t*)"  No Bitmap files...      ");
    while(1) { ; }
  }
}

#ifdef SOMETHING
void Main_Loop() {
  while(1)
  {
    counter = 0;

    while ((counter) < ubNumberOfFiles)
    {
      /* Step1 : Display on Foreground layer -------------------------------*/
      /* Format the string */
      sprintf ((char*)str, "Media/%-11.11s", pDirectoryFiles[counter]);

      if (Storage_CheckBitmapFile((const char*)str, &uwBmplen) == 0)
      {
        /* Format the string */
        sprintf ((char*)str, "Media/%-11.11s", pDirectoryFiles[counter]);

        /* Set LCD foreground Layer */
        BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER_FOREGROUND);

        /* Open a file and copy its content to an internal buffer */
        Storage_OpenReadFile(uwInternalBuffer, (const char*)str);

        /* Write bmp file on LCD frame buffer */
        BSP_LCD_DrawBitmap(0, 0, uwInternalBuffer);

        /* Configure the transparency for background layer : Increase the transparency */
        for (transparency = 0; transparency < 255; (transparency++))
        {
          BSP_LCD_SetTransparency(LTDC_ACTIVE_LAYER_FOREGROUND, transparency);

          /* Insert a delay of display */
          // HAL_Delay(10);
        }

        /* Configure the transparency for foreground layer : decrease the transparency */
        for (transparency = 255; transparency > 0; transparency--)
        {
          BSP_LCD_SetTransparency(LTDC_ACTIVE_LAYER_FOREGROUND, transparency);

          /* Insert a delay of display */
          // HAL_Delay(10);
        }

        // HAL_Delay(1000);

        /* Clear the Foreground Layer */
        BSP_LCD_Clear(LCD_COLOR_BLACK);

        /* Jump to the next image */
        counter++;

        /* Step2 : Display on Background layer -----------------------------*/
        /* Format the string */
        sprintf ((char*)str, "Media/%-11.11s", pDirectoryFiles[counter]);

        if ((Storage_CheckBitmapFile((const char*)str, &uwBmplen) == 0) || (counter < (ubNumberOfFiles)))
        {
          /* Connect the Output Buffer to LCD Background Layer  */
          BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER_BACKGROUND);

          /* Format the string */
          sprintf ((char*)str, "Media/%-11.11s", pDirectoryFiles[counter]);

          /* Open a file and copy its content to an internal buffer */
          Storage_OpenReadFile(uwInternalBuffer, (const char*)str);

          /* Write bmp file on LCD frame buffer */
          BSP_LCD_DrawBitmap(0, 0, uwInternalBuffer);

          /* Configure the transparency for background layer : decrease the transparency */
          for (transparency = 0; transparency < 255; (transparency++))
          {
            BSP_LCD_SetTransparency(LTDC_ACTIVE_LAYER_BACKGROUND, transparency);

            /* Insert a delay of display */
            // HAL_Delay(10);
          }

          // HAL_Delay(1000);

          /* Step3 : -------------------------------------------------------*/
          /* Configure the transparency for background layer : Increase the transparency */
          for (transparency = 255; transparency > 0; transparency--)
          {
            BSP_LCD_SetTransparency(LTDC_ACTIVE_LAYER_BACKGROUND, transparency);

            /* Insert a delay of display */
            // HAL_Delay(10);
          }

          // HAL_Delay(1000);

          /* Clear the Background Layer */
          BSP_LCD_Clear(LCD_COLOR_BLACK);

          counter++;
        }
        else if (Storage_CheckBitmapFile((const char*)str, &uwBmplen) == 0)
        {
          /* Set the Text Color */
          BSP_LCD_SetTextColor(LCD_COLOR_RED);

          BSP_LCD_DisplayStringAtLine(7, (uint8_t *) str);
          BSP_LCD_DisplayStringAtLine(8, (uint8_t*)"    File type not supported. ");
          while(1) { ; }
        }
      }
    }
  }
}
#else
void Display_Forground() {
  /* Step1 : Display on Foreground layer -------------------------------*/
  /* Format the string */
  sprintf ((char*)str, "Media/%-11.11s", pDirectoryFiles[counter]);

  if (Storage_CheckBitmapFile((const char*)str, &uwBmplen) == 0)
  {
    /* Format the string */
    sprintf ((char*)str, "Media/%-11.11s", pDirectoryFiles[counter]);

    /* Set LCD foreground Layer */
    BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER_FOREGROUND);

    /* Open a file and copy its content to an internal buffer */
    Storage_OpenReadFile(uwInternalBuffer, (const char*)str);

    /* Write bmp file on LCD frame buffer */
    BSP_LCD_DrawBitmap(0, 0, uwInternalBuffer);

    /* Configure the transparency for background layer : Increase the transparency */
    for (transparency = 0; transparency < 255; (transparency++))
    {
        BSP_LCD_SetTransparency(LTDC_ACTIVE_LAYER_FOREGROUND, transparency);

        /* Insert a delay of display */
        // HAL_Delay(10);
    }

    /* Configure the transparency for foreground layer : decrease the transparency */
    for (transparency = 255; transparency > 0; transparency--)
    {
        BSP_LCD_SetTransparency(LTDC_ACTIVE_LAYER_FOREGROUND, transparency);

        /* Insert a delay of display */
        // HAL_Delay(10);
    }

    // HAL_Delay(1000);

    /* Clear the Foreground Layer */
    BSP_LCD_Clear(LCD_COLOR_BLACK);

    /* Jump to the next image */
    counter++;
    counter = counter % ubNumberOfFiles;
  }
}


void Format_String() {
  /* Format the string */
  sprintf ((char*)str, "Media/%-11.11s", pDirectoryFiles[counter]);

  if ((Storage_CheckBitmapFile((const char*)str, &uwBmplen) == 0) || (counter < (ubNumberOfFiles))) {
      return;
  }
  else if (Storage_CheckBitmapFile((const char*)str, &uwBmplen) == 0)
  {
      /* Set the Text Color */
      BSP_LCD_SetTextColor(LCD_COLOR_RED);

      BSP_LCD_DisplayStringAtLine(7, (uint8_t *) str);
      BSP_LCD_DisplayStringAtLine(8, (uint8_t*)"    File type not supported. ");
      while(1) {;}
  }
}


void Display_Background() {
  /* Step2 : Display on Background layer -----------------------------*/

  /* Connect the Output Buffer to LCD Background Layer  */
  BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER_BACKGROUND);

  /* Format the string */
  sprintf ((char*)str, "Media/%-11.11s", pDirectoryFiles[counter]);

  /* Open a file and copy its content to an internal buffer */
  Storage_OpenReadFile(uwInternalBuffer, (const char*)str);

  /* Write bmp file on LCD frame buffer */
  BSP_LCD_DrawBitmap(0, 0, uwInternalBuffer);

  /* Configure the transparency for background layer : decrease the transparency */
  for (transparency = 0; transparency < 255; (transparency++))
  {
    BSP_LCD_SetTransparency(LTDC_ACTIVE_LAYER_BACKGROUND, transparency);

    /* Insert a delay of display */
    // HAL_Delay(10);
  }

  // HAL_Delay(1000);
}


void Configure_Transparency() {
  /* Step3 : -------------------------------------------------------*/
  /* Configure the transparency for background layer : Increase the transparency */
  for (transparency = 255; transparency > 0; transparency--)
  {
  BSP_LCD_SetTransparency(LTDC_ACTIVE_LAYER_BACKGROUND, transparency);

  /* Insert a delay of display */
  // HAL_Delay(10);
  }

  // HAL_Delay(1000);

  /* Clear the Background Layer */
  BSP_LCD_Clear(LCD_COLOR_BLACK);

  counter++;
  counter = counter % ubNumberOfFiles;
}

#endif


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
  */
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  HAL_Init();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[1] = get_cycles();
#endif

  /* Configure the system clock to 200 MHz */
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  SystemClock_Config();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[2] = get_cycles();
#endif

  /* Configure LED3 */
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  BSP_LED_Init_0();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[3] = get_cycles();
#endif

  /*##-1- Configure LCD ######################################################*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  LCD_Config();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[4] = get_cycles();
#endif

  /*##-2- Link the SD Card disk I/O driver ###################################*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Link_Driver();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[5] = get_cycles();
#endif

  /*##-3- Initialize the Directory Files pointers (heap) ###################*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Init_Directory_File_Pointers();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[6] = get_cycles();
#endif

  /* Get the BMP file names on root directory */
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Get_BMP_Filenames();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[7] = get_cycles();
#endif

  /* Main infinite loop */
  // Main_Loop();
  while(1)
  {
#ifdef TIMER_BASELINE
  loop_count++; // start to records from loop_count = 1
  reset_timer();
  start_timer();
#endif
    Display_Forground();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime_1[loop_count] = get_cycles();
#endif

#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
    Format_String();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime_2[loop_count] = get_cycles();
#endif

#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
    Display_Background();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime_3[loop_count] = get_cycles();
#endif

#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
    Configure_Transparency();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime_4[loop_count] = get_cycles();
#endif
  }
}

/**
  * @brief  LCD configuration
  * @param  None
  * @retval None
  */
static void LCD_Config(void)
{
  uint8_t lcd_status = LCD_OK;

  /* LCD DSI initialization in mode Video Burst */
  /* Initialize DSI LCD */
  BSP_LCD_Init();
  while(lcd_status != LCD_OK);

  BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER_BACKGROUND, LCD_FB_START_ADDRESS);

  /* Select the LCD Background Layer */
  BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER_BACKGROUND);

  /* Clear the Background Layer */
  BSP_LCD_Clear(LCD_COLOR_BLACK);

  BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER_FOREGROUND, LCD_BG_LAYER_ADDRESS);

  /* Select the LCD Foreground Layer */
  BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER_FOREGROUND);

  /* Clear the Foreground Layer */
  BSP_LCD_Clear(LCD_COLOR_BLACK);

  /* Configure the transparency for foreground and background :
  Increase the transparency */
  BSP_LCD_SetTransparency(LTDC_ACTIVE_LAYER_BACKGROUND, 0);
  BSP_LCD_SetTransparency(LTDC_ACTIVE_LAYER_FOREGROUND, 100);
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

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
