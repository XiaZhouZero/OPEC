/**
  ******************************************************************************
  * @file    Display/LCD_AnimatedPictureFromSDCard/Src/main.c
  * @author  MCD Application Team
  * @brief   This file provides main program functions
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
/** @addtogroup STM32F4xx_HAL_Applications
  * @{
  */

/** @addtogroup LCD_AnimatedPictureFromSDCard
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
FATFS SD_FatFs;  /* File system object for SD card logical drive */
char SD_Path[4]; /* SD card logical drive path */
DIR directory;
char* pDirectoryFiles[MAX_BMP_FILES];
char pDirectoryFileNames[MAX_BMP_FILES][MAX_BMP_FILE_NAME];
uint8_t ubNumberOfFiles = 0;
uint32_t uwBmplen = 0;
extern LTDC_HandleTypeDef  hltdc_eval;
extern DSI_HandleTypeDef hdsi_eval;
DSI_VidCfgTypeDef hdsivideo_handle_0;

/* Internal Buffer defined in SDRAM memory */
uint8_t *uwInternalBuffer = (uint8_t *)INTERNAL_BUFFER_START_ADDRESS;

uint32_t counter = 0;


/* Private function prototypes -----------------------------------------------*/
static void System_Init(void);
static void LCD_Config(void);
static void Link_Driver(void);
static void Init_Directory_File_Pointers(void);
static void Display_Background(void);
static void Display_Foreground(void);
static void Turn_On_Display(void);
static void Infinite_Loop(void);

static void SystemClock_Config(void);
static void Error_Handler(void);
static uint8_t LCD_Init(void);


/* Private functions ---------------------------------------------------------*/

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

  /*##-1- Configure LCD ######################################################*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  LCD_Config();  
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[2] = get_cycles();
#endif

  /*##-2- Link the SD Card disk I/O driver ###################################*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Link_Driver();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[3] = get_cycles();
#endif

  /*##-3- Initialize the Directory Files pointers (heap) ###################*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Init_Directory_File_Pointers();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[4] = get_cycles();
#endif

  /*##-4- Display Background picture #######################################*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Display_Background();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[5] = get_cycles();
#endif

  /*##-5- Display Foreground picture #######################################*/
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Display_Foreground();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[6] = get_cycles();
#endif

#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Turn_On_Display();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[7] = get_cycles();
#endif

  /* Infinite loop */
#ifdef TIMER_BASELINE
	reset_timer();
	start_timer();
#endif
  Infinite_Loop();
#ifdef TIMER_BASELINE
	stop_timer();
	ExeTime[8] = get_cycles();
#endif

  while (1)
  {
  }
  
}


static void System_Init(void) {
  /* STM32F4xx HAL library initialization:
      - Configure the Flash prefetch, instruction and Data caches
      - Configure the Systick to generate an interrupt each 1 msec
      - Set NVIC Group Priority to 4
      - Global MSP (MCU Support Package) initialization
    */
  HAL_Init();
  
  /* Configure the system clock to 180 MHz */
  SystemClock_Config();
  
  /* Configure LED3 */
  BSP_LED_Init(LED3);
}


/**
  * @brief  LCD configuration.
  * @param  None
  * @retval None
  */
static void LCD_Config(void)
{
  /* LCD DSI initialization in mode Video Burst  */
  LCD_Init();
  
  BSP_LCD_DisplayOff(); /*turn display Off during the initialization */
  
  /* Activate background layers */
  BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER_BACKGROUND, LCD_BG_LAYER_ADDRESS); 
  
  /* Set LCD Background Layer  */
  BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER_BACKGROUND); 
  /* Clear the Background Layer */
  BSP_LCD_Clear(LCD_COLOR_WHITE);
 
  /* Activate foreground and background layers */  
  BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER_FOREGROUND, LCD_FG_LAYER_ADDRESS);   
  /* Set LCD Foreground Layer  */
  BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER_FOREGROUND);
  /* Clear the Foreground Layer */ 
  BSP_LCD_Clear(LCD_COLOR_BLACK); 

  /* Configure and enable the Color Keying feature */
  BSP_LCD_SetColorKeying(1, 0); 

  /* Configure the transparency for foreground: Increase the transparency */
  BSP_LCD_SetTransparency(LTDC_ACTIVE_LAYER_FOREGROUND, 100);
}


static void Link_Driver(void) {
  if(FATFS_LinkDriver(&SD_Driver, SD_Path) != 0)
  {
    Error_Handler();
  }
}


static void Init_Directory_File_Pointers(void) {
  for (counter = 0; counter < MAX_BMP_FILES; counter++)
    {
      // pDirectoryFiles[counter] = malloc(MAX_BMP_FILE_NAME);
      pDirectoryFiles[counter] = &pDirectoryFileNames[counter][0];
      if(pDirectoryFiles[counter] == NULL)
      {
        /* Set the Text Color */
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
        
        BSP_LCD_DisplayStringAtLine(8, (uint8_t*)"  Cannot allocate memory ");
        Error_Handler();
        
      }
    }
}


static void Display_Background(void) {
  // BSP_LCD_DisplayStringAtLine(8, (uint8_t*)"    Hello Display_Background...      ");
  /* Select Background Layer  */
  BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER_BACKGROUND);

  /* Register the file system object to the FatFs module */
  if(f_mount(&SD_FatFs, (TCHAR const*)SD_Path, 0) != FR_OK)
  {
    /* FatFs Initialization Error */
    /* Set the Text Color */
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    
    BSP_LCD_DisplayStringAtLine(8, (uint8_t*)"  FatFs Initialization Error ");
  }
  else
  {    
    /* Open directory */
    if (f_opendir(&directory, (TCHAR const*)"/BACK") != FR_OK)
    {
      /* Set the Text Color */
      BSP_LCD_SetTextColor(LCD_COLOR_RED);
      
      BSP_LCD_DisplayStringAtLine(8, (uint8_t*)"    Open directory.. fails");
      Error_Handler();
    }
  }

  if (Storage_CheckBitmapFile("BACK/image.bmp", &uwBmplen) == 0)
  {
    /* Format the string */
    Storage_OpenReadFile(uwInternalBuffer, "BACK/image.bmp");
    /* Write bmp file on LCD frame buffer */
    BSP_LCD_DrawBitmap(0, 0, uwInternalBuffer);
  }
  else
  {
    /* Set the Text Color */
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    
    BSP_LCD_DisplayStringAtLine(8, (uint8_t*)"    File type not supported. "); 
    Error_Handler();
  }
}


static void Display_Foreground(void) {
  // BSP_LCD_DisplayStringAtLine(8, (uint8_t*)"    Hello Display_Foreground...      ");
  /* Select Foreground Layer  */
  BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER_FOREGROUND);
  
  /* Decrease the foreground transparency */
  BSP_LCD_SetTransparency(LTDC_ACTIVE_LAYER_FOREGROUND, 200); 
  
  /* Get the BMP file names on root directory */
  ubNumberOfFiles = Storage_GetDirectoryBitmapFiles("/TOP", pDirectoryFiles);
  
  if (ubNumberOfFiles == 0)
  {
    for (counter = 0; counter < MAX_BMP_FILES; counter++)
    {
      // free(pDirectoryFiles[counter]);
      pDirectoryFiles[counter] = NULL;

    }
    /* Set the Text Color */
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    
    BSP_LCD_DisplayStringAtLine(8, (uint8_t*)"    No Bitmap files...      ");
    Error_Handler();
  }
}


static void Turn_On_Display(void) {
  HAL_Delay(100);
  BSP_LCD_DisplayOn(); /* turn display On after background initialization */
}


static void Infinite_Loop(void) {
  uint8_t str[30];

  // while(1)
  // {
    counter = 0;
    
    while (counter < ubNumberOfFiles)
    {
      /* Format the string */
      sprintf ((char*)str, "TOP/%-11.11s", pDirectoryFiles[counter]);
      
      if (Storage_CheckBitmapFile((const char*)str, &uwBmplen) == 0)
      {
        /* Format the string */
        sprintf ((char*)str, "TOP/%-11.11s", pDirectoryFiles[counter]);
        
        /* Open a file and copy its content to a buffer */
        Storage_OpenReadFile(uwInternalBuffer, (const char*)str);
        
        HAL_Delay(100);
        
        /* Write bmp file on LCD frame buffer */
        BSP_LCD_DrawBitmap(0, 0, uwInternalBuffer);
        
        /* Jump to next image */
        counter++;   
      }
      else
      {
        /* Set the Text Color */
        BSP_LCD_SetTextColor(LCD_COLOR_RED); 
        
        BSP_LCD_DisplayStringAtLine(7, (uint8_t *) str);        
        BSP_LCD_DisplayStringAtLine(8, (uint8_t*)"    File type not supported. "); 
        Error_Handler();
      }
    }
  // }
}


static uint8_t LCD_Init(void)
{
  DSI_PLLInitTypeDef dsiPllInit;
  DSI_PHY_TimerTypeDef  PhyTimings;
  static RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
  uint32_t LcdClock  = 19200; /*!< LcdClk = 19200 kHz */

  uint32_t laneByteClk_kHz = 0;
  uint32_t                   VSA; /*!< Vertical start active time in units of lines */
  uint32_t                   VBP; /*!< Vertical Back Porch time in units of lines */
  uint32_t                   VFP; /*!< Vertical Front Porch time in units of lines */
  uint32_t                   VACT; /*!< Vertical Active time in units of lines = imageSize Y in pixels to display */
  uint32_t                   HSA; /*!< Horizontal start active time in units of lcdClk */
  uint32_t                   HBP; /*!< Horizontal Back Porch time in units of lcdClk */
  uint32_t                   HFP; /*!< Horizontal Front Porch time in units of lcdClk */
  uint32_t                   HACT; /*!< Horizontal Active time in units of lcdClk = imageSize X in pixels to display */
  
  
  /* Toggle Hardware Reset of the DSI LCD using
  * its XRES signal (active low) */
  BSP_LCD_Reset();
  
  /* Call first MSP Initialize only in case of first initialization
  * This will set IP blocks LTDC, DSI and DMA2D
  * - out of reset
  * - clocked
  * - NVIC IRQ related to IP blocks enabled
  */
  BSP_LCD_MspInit();
  
/*************************DSI Initialization***********************************/  
  
  /* Base address of DSI Host/Wrapper registers to be set before calling De-Init */
  hdsi_eval.Instance = DSI;
  
  HAL_DSI_DeInit(&(hdsi_eval));
  
  dsiPllInit.PLLNDIV  = 100;
  dsiPllInit.PLLIDF   = DSI_PLL_IN_DIV5;
  dsiPllInit.PLLODF  = DSI_PLL_OUT_DIV1;
  laneByteClk_kHz = 62500; /* 500 MHz / 8 = 62.5 MHz = 62500 kHz */
  
  /* Set number of Lanes */
  hdsi_eval.Init.NumberOfLanes = DSI_TWO_DATA_LANES;
  
  /* TXEscapeCkdiv = f(LaneByteClk)/15.62 = 4 */
  hdsi_eval.Init.TXEscapeCkdiv = laneByteClk_kHz/15620; 
  
  HAL_DSI_Init(&(hdsi_eval), &(dsiPllInit));
  
  /* The following values are same for portrait and landscape orientations */
  VSA  = OTM8009A_480X800_VSYNC;        /* 10 */
  VBP  = OTM8009A_480X800_VBP;          /* 15 */
  VFP  = OTM8009A_480X800_VFP;          /* 16 */
  HSA  = OTM8009A_480X800_HSYNC;        /* 2 */
  HBP  = OTM8009A_480X800_HBP;          /* 20 */
  HFP  = OTM8009A_480X800_HFP;          /* 20 */ 
  HACT = OTM8009A_800X480_WIDTH;        /* 800 */
  VACT = OTM8009A_800X480_HEIGHT;       /* 480 */   

  hdsivideo_handle_0.VirtualChannelID = LCD_OTM8009A_ID;
  hdsivideo_handle_0.ColorCoding = LCD_DSI_PIXEL_DATA_FMT_RBG888;
  hdsivideo_handle_0.VSPolarity = DSI_VSYNC_ACTIVE_HIGH;
  hdsivideo_handle_0.HSPolarity = DSI_HSYNC_ACTIVE_HIGH;
  hdsivideo_handle_0.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;  
  hdsivideo_handle_0.Mode = DSI_VID_MODE_BURST; /* Mode Video burst ie : one LgP per line */
  hdsivideo_handle_0.NullPacketSize = 0xFFF;
  hdsivideo_handle_0.NumberOfChunks = 0;
  hdsivideo_handle_0.PacketSize                = HACT; /* Value depending on display orientation choice portrait/landscape */ 
  hdsivideo_handle_0.HorizontalSyncActive      = (HSA * laneByteClk_kHz) / LcdClock;
  hdsivideo_handle_0.HorizontalBackPorch       = (HBP * laneByteClk_kHz) / LcdClock;
  hdsivideo_handle_0.HorizontalLine            = ((HACT + HSA + HBP + HFP) * laneByteClk_kHz) / LcdClock; /* Value depending on display orientation choice portrait/landscape */
  hdsivideo_handle_0.VerticalSyncActive        = VSA;
  hdsivideo_handle_0.VerticalBackPorch         = VBP;
  hdsivideo_handle_0.VerticalFrontPorch        = VFP;
  hdsivideo_handle_0.VerticalActive            = VACT; /* Value depending on display orientation choice portrait/landscape */
  
  /* Enable or disable sending LP command while streaming is active in video mode */
  hdsivideo_handle_0.LPCommandEnable = DSI_LP_COMMAND_ENABLE; /* Enable sending commands in mode LP (Low Power) */
  
  /* Largest packet size possible to transmit in LP mode in VSA, VBP, VFP regions */
  /* Only useful when sending LP packets is allowed while streaming is active in video mode */
  hdsivideo_handle_0.LPLargestPacketSize = 16;
  
  /* Largest packet size possible to transmit in LP mode in HFP region during VACT period */
  /* Only useful when sending LP packets is allowed while streaming is active in video mode */
  hdsivideo_handle_0.LPVACTLargestPacketSize = 0;
  
  
  /* Specify for each region of the video frame, if the transmission of command in LP mode is allowed in this region */
  /* while streaming is active in video mode                                                                         */
  hdsivideo_handle_0.LPHorizontalFrontPorchEnable = DSI_LP_HFP_ENABLE;   /* Allow sending LP commands during HFP period */
  hdsivideo_handle_0.LPHorizontalBackPorchEnable  = DSI_LP_HBP_ENABLE;   /* Allow sending LP commands during HBP period */
  hdsivideo_handle_0.LPVerticalActiveEnable = DSI_LP_VACT_ENABLE;  /* Allow sending LP commands during VACT period */
  hdsivideo_handle_0.LPVerticalFrontPorchEnable = DSI_LP_VFP_ENABLE;   /* Allow sending LP commands during VFP period */
  hdsivideo_handle_0.LPVerticalBackPorchEnable = DSI_LP_VBP_ENABLE;   /* Allow sending LP commands during VBP period */
  hdsivideo_handle_0.LPVerticalSyncActiveEnable = DSI_LP_VSYNC_ENABLE; /* Allow sending LP commands during VSync = VSA period */
  
  /* Configure DSI Video mode timings with settings set above */
  HAL_DSI_ConfigVideoMode(&(hdsi_eval), &(hdsivideo_handle_0));

  /* Configure DSI PHY HS2LP and LP2HS timings */
  PhyTimings.ClockLaneHS2LPTime = 35;
  PhyTimings.ClockLaneLP2HSTime = 35;
  PhyTimings.DataLaneHS2LPTime = 35;
  PhyTimings.DataLaneLP2HSTime = 35;
  PhyTimings.DataLaneMaxReadTime = 0;
  PhyTimings.StopWaitTime = 10;
  HAL_DSI_ConfigPhyTimer(&hdsi_eval, &PhyTimings);
  

/*************************End DSI Initialization*******************************/ 
  
  
/************************LTDC Initialization***********************************/  
  
  /* Timing Configuration */    
  hltdc_eval.Init.HorizontalSync = (HSA - 1);
  hltdc_eval.Init.AccumulatedHBP = (HSA + HBP - 1);
  hltdc_eval.Init.AccumulatedActiveW = (HACT + HSA + HBP - 1);
  hltdc_eval.Init.TotalWidth = (HACT + HSA + HBP + HFP - 1);
  
  /* Initialize the LCD pixel width and pixel height */
  hltdc_eval.LayerCfg->ImageWidth  = HACT;
  hltdc_eval.LayerCfg->ImageHeight = VACT;   
  
  
  /* LCD clock configuration */
  /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
  /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 186 Mhz */
  /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 186 MHz / 5 = 37.2 MHz */
  /* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_2 = 37.2 MHz / 2 = 18.6 MHz */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 186;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct); 
  
  /* Background value */
  hltdc_eval.Init.Backcolor.Blue = 0;
  hltdc_eval.Init.Backcolor.Green = 0;
  hltdc_eval.Init.Backcolor.Red = 0;
  hltdc_eval.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc_eval.Instance = LTDC;
  
  /* Get LTDC Configuration from DSI Configuration */
  HAL_LTDC_StructInitFromVideoConfig(&(hltdc_eval), &(hdsivideo_handle_0));
  
  /* Initialize the LTDC */  
  HAL_LTDC_Init(&hltdc_eval);

  /* Enable the DSI host and wrapper after the LTDC initialization
     To avoid any synchronization issue, the DSI shall be started after enabling the LTDC */

  HAL_DSI_Start(&(hdsi_eval));
  
#if !defined(DATA_IN_ExtSDRAM)
  /* Initialize the SDRAM */
  BSP_SDRAM_Init();
#endif /* DATA_IN_ExtSDRAM */
  
  /* Initialize the font */
  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
  
/************************End LTDC Initialization*******************************/
  
  
/***********************OTM8009A Initialization********************************/  
  
  /* Initialize the OTM8009A LCD Display IC Driver (KoD LCD IC Driver)
  *  depending on configuration set in 'hdsivideo_handle_0'.
  */
  OTM8009A_Init(OTM8009A_FORMAT_RGB888, OTM8009A_ORIENTATION_LANDSCAPE);
  
/***********************End OTM8009A Initialization****************************/ 
  
  return LCD_OK; 
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  BSP_LCD_DisplayOn();  /*turn display On in case of error to display the error message */
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
    Error_Handler();
  }

  /* Activate the OverDrive to reach the 180 MHz Frequency */
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
     Error_Handler();
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
    Error_Handler();
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
