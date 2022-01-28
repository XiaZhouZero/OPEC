/**
  ******************************************************************************
  * @file    FatFs/FatFs_USBDisk/Src/main.c
  * @author  MCD Application Team
  * @version V1.3.4
  * @date    06-May-2016
**/

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
#if 0
struct test_struct {
  char a[20];
  uint32_t a1;
  uint32_t a2;
  char b[40];
  uint32_t c;
};
#endif

/* Private define ------------------------------------------------------------*/

// #define OI_TRACE
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


/* Private variables ---------------------------------------------------------*/
unsigned char PinRxBuffer[PINRXBUFFSIZE];
unsigned char LockRxBuffer[LOCKRXBUFFSIZE];
unsigned char key[32];
unsigned char key_in[32];
unsigned char pin[] = "1995\n";
unsigned char str[STRSIZE];   //will be used for print
#ifdef TODO_SANITIZATION
uint32_t Critical = 0;    // Test Data Sanitization
#endif
// static int test_array[8] = {0,1,2,3,4,5,6,7};

/* UART handler declaration */
UART_HandleTypeDef UartHandle;
__IO ITStatus UartReady = RESET;

/* Private function prototypes -----------------------------------------------*/
void System_Init(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t f);
void Uart_Init();
void Key_Init();
void Unlock_Task(unsigned int one, unsigned int exp, unsigned int ms, unsigned int *unlock_count, unsigned int unused);
void Lock_Task();
static void print(unsigned char*, int len);
static void SystemClock_Config(void);
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/
#ifdef OI_TRACE
char* traced_funcnames[256] = {0};
called_func_count = 0;

// void record_func(char *ptr) {
//   traced_funcnames[called_func_count++] = ptr;
// }


// int _write(int file, char *ptr, int len)
// {
//     // UartReady = RESET;
//     HAL_UART_Transmit_IT(&UartHandle, (uint8_t *)ptr, len);
//     // while (UartReady != SET) {}
//     // UartReady = RESET;
//     return len;
// }

int _write(int file, char *ptr, int len)
{
    int i;
    for(i=0; (*ptr)&&(i<len); i++) {
      ITM_SendChar(*(ptr+i));
    }
    if(i<len) {
      len=i;
    }
    return len;
}
#endif


void do_lock() {
  BSP_LED_Off(LED4);
}


void do_unlock(){
  BSP_LED_On(LED4);
}


void rx_from_uart(uint32_t size){
    if (size > PINRXBUFFSIZE){
        Error_Handler();
    }
    if(HAL_UART_Receive_IT(&UartHandle, (uint8_t *)PinRxBuffer, size) != HAL_OK) {
        Error_Handler();
    }
    /*##- Wait for the end of the transfer ###################################*/
    // while (UartReady != SET) {break;}
    while (UartReady != SET) {}
    /* Reset transmission flag */
    UartReady = RESET;
}


void System_Init(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t f) {

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();

  /* Configure LEDs */
  BSP_LED_Init(LED4);
  // BSP_LED_Init(LED5);
  // BSP_LED_Init(LED3);
  // BSP_LED_Init(LED6);

  /* Configure the system clock to 168 MHz */
  SystemClock_Config();
}


void Uart_Init() {
   /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART1 configured as follow:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance          = USARTx;

  UartHandle.Init.BaudRate     = 115200;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

  if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    Error_Handler();
  }

  //setbuf(stdout,NULL);
}


/**
 * @brief initialize the key
 * 
 */
void Key_Init()
{
    mbedtls_sha256((unsigned char*)pin,PINBUFFSIZE,key,0);
    // use_test_array(test_array);
    return;
}


void Init_Lock(){
  const char locked[]="System Intial Locked\n";
  print(locked, sizeof(locked));
  do_lock();
}


/**
 * @brief unlock task
 * 
 * @param one 
 * @param exp 
 * @param ms 
 * @param unlock_count 
 * @param unused 
 */
// void Unlock_Task(unsigned int one, struct test_struct test, unsigned int exp, unsigned int ms, unsigned int *unlock_count, unsigned int unused)
void Unlock_Task(unsigned int one, unsigned int exp, unsigned int ms, unsigned int *unlock_count, unsigned int unused)
{
    const char enter[] = "Enter Pin:\n";
    const char unlocked[] = "Unlocked\n";
    const char incorrect[] = "Incorrect Pin\n";
    const char waiting[] = "waiting...\n";
    const char lockout[] = "System Lockout\n";
    unsigned int i;
    unsigned int failures = 0;
    while(1) {
        print(enter, sizeof(enter));
        rx_from_uart(5);
        /* hash password received from uart */
        mbedtls_sha256((unsigned char*)PinRxBuffer,PINRXBUFFSIZE,key_in,0);
        for(i = 0; i < 32; i++) {
            if(key[i]!=key_in[i]) break;
        }
        if (i == 32) {
          print(unlocked, sizeof(unlocked));
          (*unlock_count)++;
#ifdef TODO_SANITIZATION
            Critical++;
#endif
          break;
        }

        failures++;                 // increment number of failures
        print(incorrect, sizeof(incorrect));
        if (failures > 5 && failures <= 10) {
            exp = one << failures;  // essentially 2^failures
            ms = 78*exp;            // after 5 tries, start waiting around 5 secs and then doubles
            print(waiting, sizeof(waiting));
            HAL_Delay(ms);
        }
        else if(failures > 10) {
            print(lockout, sizeof(lockout));
            while(1){}
        }
    }
    do_unlock();
    return;
}

void Lock_Task()
{
    const char locked[]="System Final Locked\n";
    while (1) {
        rx_from_uart(2);
        if (PinRxBuffer[0] == '0'){
            print(locked, sizeof(locked));
#ifdef TODO_SANITIZATION
            Critical++;
#endif
            break;
        }
    }
    do_lock();
    return;
}


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    unsigned int one = 1;
    // struct test_struct test = {"AAAA", 1 , 2, "bbbbbbbb", 3};
    unsigned int exp;
    unsigned int ms;
    unsigned int unlock_count = 0;
    unsigned int unsed_v;
#ifdef OI_TRACE
    // printf("FUNC_FLAG: %s@%s\n", __FUNCTION__, __FILE__);
    printf(__FUNCTION__);
    // record_func(__FILE__);
#endif
#ifdef TIMER_BASELINE
  reset_timer();
  start_timer();
#endif
    System_Init(1, 2, 3, 4, 5);
#ifdef TIMER_BASELINE
  stop_timer();
  ExeTime[1] = get_cycles();
#endif

#ifdef TIMER_BASELINE
  reset_timer();
  start_timer();
#endif
    Uart_Init();
#ifdef TIMER_BASELINE
  stop_timer();
  ExeTime[2] = get_cycles();
#endif

#ifdef TIMER_BASELINE
  reset_timer();
  start_timer();
#endif
    Key_Init();
#ifdef TIMER_BASELINE
  stop_timer();
  ExeTime[3] = get_cycles();
#endif

#ifdef TIMER_BASELINE
  reset_timer();
  start_timer();
#endif
    Init_Lock();
#ifdef TIMER_BASELINE
  stop_timer();
  ExeTime[4] = get_cycles();
#endif

    // while (1) {
#ifdef TIMER_BASELINE
  reset_timer();
  start_timer();
#endif
    Unlock_Task(one, exp, ms, &unlock_count, unsed_v);
#ifdef TIMER_BASELINE
  stop_timer();
  ExeTime[5] = get_cycles();
#endif

#ifdef TIMER_BASELINE
  reset_timer();
  start_timer();
#endif
    // wait for lock command
    Lock_Task();
#ifdef TIMER_BASELINE
  stop_timer();
  ExeTime[6] = get_cycles();
#endif
    // }

    //backup while loop
    while(1) {}
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
void SystemClock_Config  (void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

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
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}


/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle.
  * @note   This example shows a simple way to report end of IT Tx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete */
  UartReady = SET;

  /* Turn LED6 on: Transfer in transmission process is correct */
  //BSP_LED_On(LED6);
}


/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of IT Rx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete */
  UartReady = SET;

  /* Turn LED4 on: Transfer in reception process is correct */
  //BSP_LED_On(LED4);
}


/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
 void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  /* Turn LED3 on: Transfer error in reception/transmission process */
  //BSP_LED_On(LED3);
}


/*
int _write(int fd, char *buf, size_t count) {

  if (HAL_UART_Transmit_IT(&UartHandle, (uint8_t *) buf, count) != HAL_OK) {
    Error_Handler();
  }


  //##- Wait for the end of the transfer ###################################
  while (UartReady != SET)
  {
  }

  // Reset transmission flag
  UartReady = RESET;

  return count;
}
*/


static void print(unsigned char* str, int len) {

  UartReady = RESET;

  if (HAL_UART_Transmit_IT(&UartHandle, (uint8_t *) str, len) != HAL_OK) {
    Error_Handler();
  }
  //##- Wait for the end of the transfer ###################################
  while (UartReady != SET)
  {
  }
  // Reset transmission flag
  UartReady = RESET;

}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Turn LED5 on */
  BSP_LED_On(LED5);
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

/******************END OF FILE******************/