DEVICE_DEFS = {
  "STM32F4Discovery":{
    "Include":["ARMv7-M"],
    "Peripherals":[
      {'NAME_KEY':"FSMC",           "Base_Addr":0xA0000000,"End_Addr":0xA0000FFF},
      {'NAME_KEY':"RNG",            "Base_Addr":0x50060800,"End_Addr":0x50060BFF},
      {'NAME_KEY':"HASH",           "Base_Addr":0x50060400,"End_Addr":0x500607FF},
      {'NAME_KEY':"CRYP",           "Base_Addr":0x50060000,"End_Addr":0x500603FF},
      {'NAME_KEY':"DCMI",           "Base_Addr":0x50050000,"End_Addr":0x500503FF},
      {'NAME_KEY':"USB-OTG-FS",     "Base_Addr":0x50000000,"End_Addr":0x5003FFFF},
      {'NAME_KEY':"USB-OTG-HS",     "Base_Addr":0x40040000,"End_Addr":0x4007FFFF},
      {'NAME_KEY':"DMA2D",          "Base_Addr":0x4002B000,"End_Addr":0x4002BBFF},
      {'NAME_KEY':"ETHERNET-MAC5",  "Base_Addr":0x40029000,"End_Addr":0x400293FF},
      {'NAME_KEY':"ETHERNET-MAC4",  "Base_Addr":0x40028C00,"End_Addr":0x40028FFF},
      {'NAME_KEY':"ETHERNET-MAC3",  "Base_Addr":0x40028800,"End_Addr":0x40028BFF},
      {'NAME_KEY':"ETHERNET-MAC2",  "Base_Addr":0x40028400,"End_Addr":0x400287FF},
      {'NAME_KEY':"ETHERNET-MAC1",  "Base_Addr":0x40028000,"End_Addr":0x400283FF},
      {'NAME_KEY':"DMA2",           "Base_Addr":0x40026400,"End_Addr":0x400267FF},
      {'NAME_KEY':"DMA1",           "Base_Addr":0x40026000,"End_Addr":0x400263FF},
      {'NAME_KEY':"BKPSRAM",        "Base_Addr":0x40024000,"End_Addr":0x40024FFF},
      {'NAME_KEY':"FLASH-Inte",     "Base_Addr":0x40023C00,"End_Addr":0x40023FFF},
      {'NAME_KEY':"RCC",            "Base_Addr":0x40023800,"End_Addr":0x40023BFF},
      {'NAME_KEY':"CRC",            "Base_Addr":0x40023000,"End_Addr":0x400233FF},
      {'NAME_KEY':"GPIOK",          "Base_Addr":0x40022800,"End_Addr":0x40022BFF},
      {'NAME_KEY':"GPIOJ",          "Base_Addr":0x40022400,"End_Addr":0x400227FF},
      {'NAME_KEY':"GPIOI",          "Base_Addr":0x40022000,"End_Addr":0x400223FF},
      {'NAME_KEY':"GPIOH",          "Base_Addr":0x40021C00,"End_Addr":0x40021FFF},
      {'NAME_KEY':"GPIOG",          "Base_Addr":0x40021800,"End_Addr":0x40021BFF},
      {'NAME_KEY':"GPIOF",          "Base_Addr":0x40021400,"End_Addr":0x400217FF},
      {'NAME_KEY':"GPIOE",          "Base_Addr":0x40021000,"End_Addr":0x400213FF},
      {'NAME_KEY':"GPIOD",          "Base_Addr":0x40020C00,"End_Addr":0x40020FFF},
      {'NAME_KEY':"GPIOC",          "Base_Addr":0x40020800,"End_Addr":0x40020BFF},
      {'NAME_KEY':"GPIOB",          "Base_Addr":0x40020400,"End_Addr":0x400207FF},
      {'NAME_KEY':"GPIOA",          "Base_Addr":0x40020000,"End_Addr":0x400203FF},
      {'NAME_KEY':"LCD-TFT",        "Base_Addr":0x40016800,"End_Addr":0x40016BFF},
      {'NAME_KEY':"SAI1",           "Base_Addr":0x40015800,"End_Addr":0x40015BFF},
      {'NAME_KEY':"SPI6",           "Base_Addr":0x40015400,"End_Addr":0x400157FF},
      {'NAME_KEY':"SPI5",           "Base_Addr":0x40015000,"End_Addr":0x400153FF},
      {'NAME_KEY':"TIM11",          "Base_Addr":0x40014800,"End_Addr":0x40014BFF},
      {'NAME_KEY':"TIM10",          "Base_Addr":0x40014400,"End_Addr":0x400147FF},
      {'NAME_KEY':"TIM9",           "Base_Addr":0x40014000,"End_Addr":0x400143FF},
      {'NAME_KEY':"EXTI",           "Base_Addr":0x40013C00,"End_Addr":0x40013FFF},
      {'NAME_KEY':"SYSCFG",         "Base_Addr":0x40013800,"End_Addr":0x40013BFF},
      {'NAME_KEY':"SPI4",           "Base_Addr":0x40013400,"End_Addr":0x400137FF},
      {'NAME_KEY':"SPI1",           "Base_Addr":0x40013000,"End_Addr":0x400133FF},
      {'NAME_KEY':"SDIO",           "Base_Addr":0x40012C00,"End_Addr":0x40012FFF},
      {'NAME_KEY':"ADC1-3",         "Base_Addr":0x40012000,"End_Addr":0x400123FF},
      {'NAME_KEY':"USART6",         "Base_Addr":0x40011400,"End_Addr":0x400117FF},
      {'NAME_KEY':"USART1",         "Base_Addr":0x40011000,"End_Addr":0x400113FF},
      {'NAME_KEY':"TIM8",           "Base_Addr":0x40010400,"End_Addr":0x400107FF},
      {'NAME_KEY':"TIM1",           "Base_Addr":0x40010000,"End_Addr":0x400103FF},
      {'NAME_KEY':"USART8",         "Base_Addr":0x40007C00,"End_Addr":0x40007FFF},
      {'NAME_KEY':"USART7",         "Base_Addr":0x40007800,"End_Addr":0x40007BFF},
      {'NAME_KEY':"DAC",            "Base_Addr":0x40007400,"End_Addr":0x400077FF},
      {'NAME_KEY':"PWR",            "Base_Addr":0x40007000,"End_Addr":0x400073FF},
      {'NAME_KEY':"CAN2",           "Base_Addr":0x40006800,"End_Addr":0x40006BFF},
      {'NAME_KEY':"CAN1",           "Base_Addr":0x40006400,"End_Addr":0x400067FF},
      {'NAME_KEY':"I2C3",           "Base_Addr":0x40005C00,"End_Addr":0x40005FFF},
      {'NAME_KEY':"I2C2",           "Base_Addr":0x40005800,"End_Addr":0x40005BFF},
      {'NAME_KEY':"I2C1",           "Base_Addr":0x40005400,"End_Addr":0x400057FF},
      {'NAME_KEY':"UART5",          "Base_Addr":0x40005000,"End_Addr":0x400053FF},
      {'NAME_KEY':"UART4",          "Base_Addr":0x40004C00,"End_Addr":0x40004FFF},
      {'NAME_KEY':"USART3",         "Base_Addr":0x40004800,"End_Addr":0x40004BFF},
      {'NAME_KEY':"USART2",         "Base_Addr":0x40004400,"End_Addr":0x400047FF},
      {'NAME_KEY':"I2S3ext",        "Base_Addr":0x40004000,"End_Addr":0x400043FF},
      {'NAME_KEY':"SPI3",           "Base_Addr":0x40003C00,"End_Addr":0x40003FFF},
      {'NAME_KEY':"SPI2",           "Base_Addr":0x40003800,"End_Addr":0x40003BFF},
      {'NAME_KEY':"I2S2ext",        "Base_Addr":0x40003400,"End_Addr":0x400037FF},
      {'NAME_KEY':"IWDG",           "Base_Addr":0x40003000,"End_Addr":0x400033FF},
      {'NAME_KEY':"WWDG",           "Base_Addr":0x40002C00,"End_Addr":0x40002FFF},
      {'NAME_KEY':"RTC-BKP",        "Base_Addr":0x40002800,"End_Addr":0x40002BFF},
      {'NAME_KEY':"TIM14",          "Base_Addr":0x40002000,"End_Addr":0x400023FF},
      {'NAME_KEY':"TIM13",          "Base_Addr":0x40001C00,"End_Addr":0x40001FFF},
      {'NAME_KEY':"TIM12",          "Base_Addr":0x40001800,"End_Addr":0x40001BFF},
      {'NAME_KEY':"TIM7",           "Base_Addr":0x40001400,"End_Addr":0x400017FF},
      {'NAME_KEY':"TIM6",           "Base_Addr":0x40001000,"End_Addr":0x400013FF},
      {'NAME_KEY':"TIM5",           "Base_Addr":0x40000C00,"End_Addr":0x40000FFF},
      {'NAME_KEY':"TIM4",           "Base_Addr":0x40000800,"End_Addr":0x40000BFF},
      {'NAME_KEY':"TIM3",           "Base_Addr":0x40000400,"End_Addr":0x400007FF},
      {'NAME_KEY':"TIM2",           "Base_Addr":0x40000000,"End_Addr":0x400003FF},
      {'NAME_KEY':"SDIO_bit_band",  "Base_Addr":0x42258000,"End_Addr":0x42258FFF},
      {'NAME_KEY':"RCC_bit_band",   "Base_Addr":0x42470000,"End_Addr":0x4247FFFF},
      {'NAME_KEY':"EXT_DEV",        "Base_Addr":0xC0000000,"End_Addr":0xCFFFFFFF},
      {'NAME_KEY':"Non-Periph",     "Base_Addr":0x00000000,"End_Addr":0x3FFFFFFF,"EXCLUDE":True},
      ],

    "Whitelist":[
    {
        "Base_Addr": 0xE0000000,
        "End_Addr": 0xe0000dff,
        "NAME_KEY": "ITM_STIM0-31",
        "NOTES": "Instrumentation Trace Macrocell (ITM) unit Stimulus Port Registers 0-31"
    },
    {
        "Base_Addr": 0xe0000e00,
        "End_Addr": 0xe0000e3f,
        "NAME_KEY": "ITM_TER",
        "NOTES": "Trace Enable Register  "
    },
    {
        "Base_Addr": 0xe0000e40,
        "End_Addr": 0xe0000e7f,
        "NAME_KEY": "ITM_TPR",
        "NOTES": "ITM Trace Privilege Register  "
    },
    {
        "Base_Addr": 0xe0000e80,
        "End_Addr": 0xe0000fcf,
        "NAME_KEY": "ITM_TCR",
        "NOTES": "Trace Control Register  "
    },
    {
        "Base_Addr": 0xe0000fd0,
        "End_Addr": 0xe0000fef,
        "NAME_KEY": "ITM_PID",
        "NOTES": "ITM Peripheral Identification registers  "
    },
    {
        "Base_Addr": 0xe0000ff0,
        "End_Addr": 0xe0000fff,
        "NAME_KEY": "ITM_CID",
        "NOTES": "ITM Component Identification registers  "
    },
    {
        "Base_Addr": 0xe0001000,
        "End_Addr": 0xe0001003,
        "NAME_KEY": "DWT_CTRL",
        "NOTES": "Data Watchpoint and Trace (DWT) unit "
    },
    {
        "Base_Addr": 0xe0001004,
        "End_Addr": 0xe0001007,
        "NAME_KEY": "DWT_CYCCNT",
        "NOTES": "DWT Cycle Count Register"
    },
    {
        "Base_Addr": 0xe0001008,
        "End_Addr": 0xe000100b,
        "NAME_KEY": "DWT_CPICNT",
        "NOTES": "DWT CPI Count Register"
    },
    {
        "Base_Addr": 0xe000100c,
        "End_Addr": 0xe000100f,
        "NAME_KEY": "DWT_EXCCNT",
        "NOTES": "DWT Exception Overhead Count Register"
    },
    {
        "Base_Addr": 0xe0001010,
        "End_Addr": 0xe0001013,
        "NAME_KEY": "DWT_SLEEPCNT",
        "NOTES": "DWT Sleep Count Register"
    },
    {
        "Base_Addr": 0xe0001014,
        "End_Addr": 0xe0001017,
        "NAME_KEY": "DWT_LSUCNT",
        "NOTES": "DWT LSU Count Register"
    },
    {
        "Base_Addr": 0xe0001018,
        "End_Addr": 0xe000101b,
        "NAME_KEY": "DWT_FOLDCNT",
        "NOTES": "DWT Folded-instruction Count Register"
    },
    {
        "Base_Addr": 0xe000101c,
        "End_Addr": 0xe000101f,
        "NAME_KEY": "DWT_PCSR",
        "NOTES": "DWT Program Counter Sample Register"
    },
    {
        "Base_Addr": 0xe0001020,
        "End_Addr": 0xe0001023,
        "NAME_KEY": "DWT_COMP0",
        "NOTES": "DWT Comparator Register0"
    },
    {
        "Base_Addr": 0xe0001024,
        "End_Addr": 0xe0001027,
        "NAME_KEY": "DWT_MASK0",
        "NOTES": "DWT Mask Register0"
    },
    {
        "Base_Addr": 0xe0001028,
        "End_Addr": 0xe000102f,
        "NAME_KEY": "DWT_FUNCTION0",
        "NOTES": "DWT Function Register0"
    },
    {
        "Base_Addr": 0xe0001030,
        "End_Addr": 0xe0001033,
        "NAME_KEY": "DWT_COMP1",
        "NOTES": "DWT Comparator Register1"
    },
    {
        "Base_Addr": 0xe0001034,
        "End_Addr": 0xe0001037,
        "NAME_KEY": "DWT_MASK1",
        "NOTES": "DWT Mask Register1"
    },
    {
        "Base_Addr": 0xe0001038,
        "End_Addr": 0xe000103f,
        "NAME_KEY": "DWT_FUNCTION1",
        "NOTES": "DWT Function Register1"
    },
    {
        "Base_Addr": 0xe0001040,
        "End_Addr": 0xe0001043,
        "NAME_KEY": "DWT_COMP2",
        "NOTES": "DWT Comparator Register2"
    },
    {
        "Base_Addr": 0xe0001044,
        "End_Addr": 0xe0001047,
        "NAME_KEY": "DWT_MASK2",
        "NOTES": "DWT Mask Register2"
    },
    {
        "Base_Addr": 0xe0001048,
        "End_Addr": 0xe000104f,
        "NAME_KEY": "DWT_FUNCTION2",
        "NOTES": "DWT Function Register2"
    },
    {
        "Base_Addr": 0xe0001050,
        "End_Addr": 0xe0001053,
        "NAME_KEY": "DWT_COMP3",
        "NOTES": "DWT Comparator Register3"
    },
    {
        "Base_Addr": 0xe0001054,
        "End_Addr": 0xe0001057,
        "NAME_KEY": "DWT_MASK3",
        "NOTES": "DWT Mask Register3"
    },
    {
        "Base_Addr": 0xe0001058,
        "End_Addr": 0xe0001fcf,
        "NAME_KEY": "DWT_FUNCTION3",
        "NOTES": "DWT Function Register3"
    },
    {
        "Base_Addr": 0xe0001fd0,
        "End_Addr": 0xe0001fef,
        "NAME_KEY": "DWT_PID",
        "NOTES": "DWT Peripheral identification registers"
    },
    {
        "Base_Addr": 0xe0001ff0,
        "End_Addr": 0xe0001fff,
        "NAME_KEY": "DWT_CID",
        "NOTES": "DWT Component identification registers"
    },
    {
        "Base_Addr": 0xe0002000,
        "End_Addr": 0xe0002003,
        "NAME_KEY": "FP_CTRL",
        "NOTES": "Flash Patch and Breakpoint (FPB) Unit FlashPatch Control Register"
    },
    {
        "Base_Addr": 0xe0002004,
        "End_Addr": 0xe0002007,
        "NAME_KEY": "FP_REMAP",
        "NOTES": "Flash Patch and Breakpoint (FPB) Unit FlashPatch Remap Register"
    },
    {
        "Base_Addr": 0xe0002008,
        "End_Addr": 0xe000200b,
        "NAME_KEY": "FP_COMP0",
        "NOTES": "Flash Patch and Breakpoint (FPB) Unit FlashPatch Comparator Register0"
    },
    {
        "Base_Addr": 0xe000200c,
        "End_Addr": 0xe000200f,
        "NAME_KEY": "FP_COMP1",
        "NOTES": "Flash Patch and Breakpoint (FPB) Unit FlashPatch Comparator Register1"
    },
    {
        "Base_Addr": 0xe0002010,
        "End_Addr": 0xe0002013,
        "NAME_KEY": "FP_COMP2",
        "NOTES": "Flash Patch and Breakpoint (FPB) Unit FlashPatch Comparator Register2"
    },
    {
        "Base_Addr": 0xe0002014,
        "End_Addr": 0xe0002017,
        "NAME_KEY": "FP_COMP3",
        "NOTES": "Flash Patch and Breakpoint (FPB) Unit FlashPatch Comparator Register3"
    },
    {
        "Base_Addr": 0xe0002018,
        "End_Addr": 0xe000201b,
        "NAME_KEY": "FP_COMP4",
        "NOTES": "Flash Patch and Breakpoint (FPB) Unit FlashPatch Comparator Register4"
    },
    {
        "Base_Addr": 0xe000201c,
        "End_Addr": 0xe000201f,
        "NAME_KEY": "FP_COMP5",
        "NOTES": "Flash Patch and Breakpoint (FPB) Unit FlashPatch Comparator Register5"
    },
    {
        "Base_Addr": 0xe0002020,
        "End_Addr": 0xe0002023,
        "NAME_KEY": "FP_COMP6",
        "NOTES": "Flash Patch and Breakpoint (FPB) Unit FlashPatch Comparator Register6"
    },
    {
        "Base_Addr": 0xe0002024,
        "End_Addr": 0xe0002fcf,
        "NAME_KEY": "FP_COMP7",
        "NOTES": "Flash Patch and Breakpoint (FPB) Unit FlashPatch Comparator Register7"
    },
    {
        "Base_Addr": 0xe0002fd0,
        "End_Addr": 0xe0002fef,
        "NAME_KEY": "FP_PID",
        "NOTES": "Flash Patch and Breakpoint (FPB) Unit Peripheral identification registers"
    },
    {
        "Base_Addr": 0xe0002ff0,
        "End_Addr": 0xe000e003,
        "NAME_KEY": "FP_CID",
        "NOTES": "Flash Patch and Breakpoint (FPB) Unit Component identification registers"
    },
    {
        "Base_Addr": 0xe000e004,
        "End_Addr": 0xe000e007,
        "NAME_KEY": "ICTR",
        "NOTES": "Interrupt Controller Type Register, ICTR"
    },
    {
        "Base_Addr": 0xe000e008,
        "End_Addr": 0xe000e00f,
        "NAME_KEY": "ACTLR",
        "NOTES": "Auxiliary Control Register Control Register"
    },
    {
        "Base_Addr": 0xe000e010,
        "End_Addr": 0xe000e013,
        "NAME_KEY": "STCSR",
        "NOTES": "SysTick Control and Status Register"
    },
    {
        "Base_Addr": 0xe000e014,
        "End_Addr": 0xe000e017,
        "NAME_KEY": "STRVR",
        "NOTES": "SysTick Reload Value Register"
    },
    {
        "Base_Addr": 0xe000e018,
        "End_Addr": 0xe000e01b,
        "NAME_KEY": "STCVR",
        "NOTES": "SysTick Current Value Register"
    },
    {
        "Base_Addr": 0xe000e01c,
        "End_Addr": 0xe000e0ff,
        "NAME_KEY": "STCR",
        "NOTES": "SysTick Calibration Value Register"
    },
    {
        "Base_Addr": 0xe000e100,
        "End_Addr": 0xe000e17f,
        "NAME_KEY": "NVIC_ISER",
        "NOTES": "Interrupt Set-Enable Registers"
    },
    {
        "Base_Addr": 0xe000e180,
        "End_Addr": 0xe000e1ff,
        "NAME_KEY": "NVIC_ICER",
        "NOTES": "Interrupt Clear-Enable Registers"
    },
    {
        "Base_Addr": 0xe000e200,
        "End_Addr": 0xe000e27f,
        "NAME_KEY": "NVIC_ISPR",
        "NOTES": "Interrupt Set-Pending Registers"
    },
    {
        "Base_Addr": 0xe000e280,
        "End_Addr": 0xe000e2ff,
        "NAME_KEY": "NVIC_ICPR",
        "NOTES": "Interrupt Clear-Pending Registers"
    },
    {
        "Base_Addr": 0xe000e300,
        "End_Addr": 0xe000e3ff,
        "NAME_KEY": "NVIC_IABR",
        "NOTES": "Interrupt Active Bit Register"
    },
    {
        "Base_Addr": 0xe000e400,
        "End_Addr": 0xe000ecff,
        "NAME_KEY": "NVIC_IPR",
        "NOTES": "Interrupt Priority Register"
    },
    {
        "Base_Addr": 0xe000ed00,
        "End_Addr": 0xe000ed03,
        "NAME_KEY": "CPUID",
        "NOTES": "CPUID Base Register  "
    },
    {
        "Base_Addr": 0xe000ed04,
        "End_Addr": 0xe000ed07,
        "NAME_KEY": "ICSR",
        "NOTES": "Interrupt Control and State Register  "
    },
    {
        "Base_Addr": 0xe000ed08,
        "End_Addr": 0xe000ed0b,
        "NAME_KEY": "VTOR",
        "NOTES": "Vector Table Offset Register  "
    },
    {
        "Base_Addr": 0xe000ed0c,
        "End_Addr": 0xe000ed0f,
        "NAME_KEY": "AIRCR",
        "NOTES": "Application Interrupt and Reset Control Register  "
    },
    {
        "Base_Addr": 0xe000ed10,
        "End_Addr": 0xe000ed13,
        "NAME_KEY": "SCR",
        "NOTES": "System Control Register  "
    },
    {
        "Base_Addr": 0xe000ed14,
        "End_Addr": 0xe000ed17,
        "NAME_KEY": "CCR",
        "NOTES": "Configuration and Control Register.  "
    },
    {
        "Base_Addr": 0xe000ed18,
        "End_Addr": 0xe000ed1b,
        "NAME_KEY": "SHPR1",
        "NOTES": "System Handler Priority Register 1  "
    },
    {
        "Base_Addr": 0xe000ed1c,
        "End_Addr": 0xe000ed1f,
        "NAME_KEY": "SHPR2",
        "NOTES": "System Handler Priority Register 2  "
    },
    {
        "Base_Addr": 0xe000ed20,
        "End_Addr": 0xe000ed23,
        "NAME_KEY": "SHPR3",
        "NOTES": "System Handler Priority Register 3  "
    },
    {
        "Base_Addr": 0xe000ed24,
        "End_Addr": 0xe000ed27,
        "NAME_KEY": "SHCSR",
        "NOTES": "System Handler Control and State Register  "
    },
    {
        "Base_Addr": 0xe000ed28,
        "End_Addr": 0xe000ed2b,
        "NAME_KEY": "CFSR",
        "NOTES": "Configurable Fault Status Registers  "
    },
    {
        "Base_Addr": 0xe000ed2c,
        "End_Addr": 0xe000ed2f,
        "NAME_KEY": "HFSR",
        "NOTES": "HardFault Status register  "
    },
    {
        "Base_Addr": 0xe000ed30,
        "End_Addr": 0xe000ed33,
        "NAME_KEY": "DFSR",
        "NOTES": "Debug Fault Status Register  "
    },
    {
        "Base_Addr": 0xe000ed34,
        "End_Addr": 0xe000ed37,
        "NAME_KEY": "MMFAR",
        "NOTES": "MemManage Address Registerb  "
    },
    {
        "Base_Addr": 0xe000ed38,
        "End_Addr": 0xe000ed3b,
        "NAME_KEY": "BFAR",
        "NOTES": "BusFault Address Registerb  "
    },
    {
        "Base_Addr": 0xe000ed3c,
        "End_Addr": 0xe000ed3f,
        "NAME_KEY": "AFSR",
        "NOTES": "Auxiliary Fault Status Register  "
    },
    {
        "Base_Addr": 0xe000ed40,
        "End_Addr": 0xe000ed43,
        "NAME_KEY": "ID_PFR0",
        "NOTES": "Processor Feature Register 0  "
    },
    {
        "Base_Addr": 0xe000ed44,
        "End_Addr": 0xe000ed47,
        "NAME_KEY": "ID_PFR1",
        "NOTES": "Processor Feature Register 1  "
    },
    {
        "Base_Addr": 0xe000ed48,
        "End_Addr": 0xe000ed4b,
        "NAME_KEY": "ID_DFR0",
        "NOTES": "Debug Features Register 0  "
    },
    {
        "Base_Addr": 0xe000ed4c,
        "End_Addr": 0xe000ed4f,
        "NAME_KEY": "ID_AFR0",
        "NOTES": "Auxiliary Features Register 0  "
    },
    {
        "Base_Addr": 0xe000ed50,
        "End_Addr": 0xe000ed53,
        "NAME_KEY": "ID_MMFR0",
        "NOTES": "Memory Model Feature Register 0  "
    },
    {
        "Base_Addr": 0xe000ed54,
        "End_Addr": 0xe000ed57,
        "NAME_KEY": "ID_MMFR1",
        "NOTES": "Memory Model Feature Register 1  "
    },
    {
        "Base_Addr": 0xe000ed58,
        "End_Addr": 0xe000ed5b,
        "NAME_KEY": "ID_MMFR2",
        "NOTES": "Memory Model Feature Register 2  "
    },
    {
        "Base_Addr": 0xe000ed5c,
        "End_Addr": 0xe000ed5f,
        "NAME_KEY": "ID_MMFR3",
        "NOTES": "Memory Model Feature Register 3  "
    },
    {
        "Base_Addr": 0xe000ed60,
        "End_Addr": 0xe000ed63,
        "NAME_KEY": "ID_ISAR0",
        "NOTES": "Instruction Set Attributes Register 0  "
    },
    {
        "Base_Addr": 0xe000ed64,
        "End_Addr": 0xe000ed67,
        "NAME_KEY": "ID_ISAR1",
        "NOTES": "Instruction Set Attributes Register 1  "
    },
    {
        "Base_Addr": 0xe000ed68,
        "End_Addr": 0xe000ed6b,
        "NAME_KEY": "ID_ISAR2",
        "NOTES": "Instruction Set Attributes Register 2  "
    },
    {
        "Base_Addr": 0xe000ed6c,
        "End_Addr": 0xe000ed6f,
        "NAME_KEY": "ID_ISAR3",
        "NOTES": "Instruction Set Attributes Register 3  "
    },
    {
        "Base_Addr": 0xe000ed70,
        "End_Addr": 0xe000ed77,
        "NAME_KEY": "ID_ISAR4",
        "NOTES": "Instruction Set Attributes Register 4  "
    },
    {
        "Base_Addr": 0xe000ed78,
        "End_Addr": 0xe000ed7b,
        "NAME_KEY": "CLIDR",
        "NOTES": "Cache Level ID Register"
    },
    {
        "Base_Addr": 0xe000ed7c,
        "End_Addr": 0xe000ed7f,
        "NAME_KEY": "CTR",
        "NOTES": "Cache Type Register"
    },
    {
        "Base_Addr": 0xe000ed80,
        "End_Addr": 0xe000ed83,
        "NAME_KEY": "CCSIDR",
        "NOTES": "Cache Size ID RegisterCache Size ID Register"
    },
    {
        "Base_Addr": 0xe000ed84,
        "End_Addr": 0xe000ed87,
        "NAME_KEY": "CSSELR",
        "NOTES": "Cache Size Selection Register"
    },
    {
        "Base_Addr": 0xe000ed88,
        "End_Addr": 0xe000ed8f,
        "NAME_KEY": "CPACR",
        "NOTES": "FPU Coprocessor Access Control Register"
    },
    {
        "Base_Addr": 0xe000ed90,
        "End_Addr": 0xe000ed93,
        "NAME_KEY": "MPU_TYPE",
        "NOTES": "MPU Type Register  "
    },
    {
        "Base_Addr": 0xe000ed94,
        "End_Addr": 0xe000ed97,
        "NAME_KEY": "MPU_CTRL",
        "NOTES": "MPU Control Register  "
    },
    {
        "Base_Addr": 0xe000ed98,
        "End_Addr": 0xe000ed9b,
        "NAME_KEY": "MPU_RNR",
        "NOTES": "MPU Region Number Register  "
    },
    {
        "Base_Addr": 0xe000ed9c,
        "End_Addr": 0xe000ed9f,
        "NAME_KEY": "MPU_RBAR",
        "NOTES": "MPU Region Base Address Register  "
    },
    {
        "Base_Addr": 0xe000eda0,
        "End_Addr": 0xe000eda3,
        "NAME_KEY": "MPU_RASR",
        "NOTES": "MPU Region Attribute and Size Register  "
    },
    {
        "Base_Addr": 0xe000eda4,
        "End_Addr": 0xe000eda7,
        "NAME_KEY": "MPU_RBAR_A1",
        "NOTES": "MPU alias registers  1"
    },
    {
        "Base_Addr": 0xe000eda8,
        "End_Addr": 0xe000edab,
        "NAME_KEY": "MPU_RASR_A1",
        "NOTES": "MPU alias registers  1"
    },
    {
        "Base_Addr": 0xe000edac,
        "End_Addr": 0xe000edaf,
        "NAME_KEY": "MPU_RBAR_A2",
        "NOTES": "MPU alias registers  2"
    },
    {
        "Base_Addr": 0xe000edb0,
        "End_Addr": 0xe000edb3,
        "NAME_KEY": "MPU_RASR_A2",
        "NOTES": "MPU alias registers  2"
    },
    {
        "Base_Addr": 0xe000edb4,
        "End_Addr": 0xe000edb7,
        "NAME_KEY": "MPU_RBAR_A3",
        "NOTES": "MPU alias registers  3"
    },
    {
        "Base_Addr": 0xe000edb8,
        "End_Addr": 0xe000edef,
        "NAME_KEY": "MPU_RASR_A3",
        "NOTES": "MPU alias registers  3"
    },
    {
        "Base_Addr": 0xe000edf0,
        "End_Addr": 0xe000edf3,
        "NAME_KEY": "DHCSR",
        "NOTES": "Debug Halting Control and Status Register  "
    },
    {
        "Base_Addr": 0xe000edf4,
        "End_Addr": 0xe000edf7,
        "NAME_KEY": "DCRSR",
        "NOTES": "Debug Core Register Selector Register  "
    },
    {
        "Base_Addr": 0xe000edf8,
        "End_Addr": 0xe000edfb,
        "NAME_KEY": "DCRDR",
        "NOTES": "Debug Core Register Data Register  "
    },
    {
        "Base_Addr": 0xe000edfc,
        "End_Addr": 0xe000eeff,
        "NAME_KEY": "DEMCR",
        "NOTES": "Debug Exception and Monitor Control Register  "
    },
    {
        "Base_Addr": 0xe000ef00,
        "End_Addr": 0xe000ef33,
        "NAME_KEY": "STIR",
        "NOTES": "Software Trigger Interrupt Register"
    },
    {
        "Base_Addr": 0xe000ef34,
        "End_Addr": 0xe000ef37,
        "NAME_KEY": "FPCCR",
        "NOTES": "FP Context Control Register  "
    },
    {
        "Base_Addr": 0xe000ef38,
        "End_Addr": 0xe000ef3b,
        "NAME_KEY": "FPCAR",
        "NOTES": "FP Context Address Register  "
    },
    {
        "Base_Addr": 0xe000ef3c,
        "End_Addr": 0xe000ef3f,
        "NAME_KEY": "FPDSCR",
        "NOTES": "FP Default Status Control Register  "
    },
    {
        "Base_Addr": 0xe000ef40,
        "End_Addr": 0xe000ef43,
        "NAME_KEY": "MVFR0",
        "NOTES": "Media and VFP Feature Register 0, MVFR0  "
    },
    {
        "Base_Addr": 0xe000ef44,
        "End_Addr": 0xe000ef4f,
        "NAME_KEY": "MVFR1",
        "NOTES": "Media and VFP Feature Register 1, MVFR1  "
    },
    {
        "Base_Addr": 0xe000ef50,
        "End_Addr": 0xe000ef57,
        "NAME_KEY": "ICIALLU1",
        "NOTES": "Instruction cache invalidate all to the Point of Unification  "
    },
    {
        "Base_Addr": 0xe000ef58,
        "End_Addr": 0xe000ef5b,
        "NAME_KEY": "ICIALLU2",
        "NOTES": "Instruction cache invalidate by address to the Point of Unification  "
    },
    {
        "Base_Addr": 0xe000ef5c,
        "End_Addr": 0xe000ef5f,
        "NAME_KEY": "DCIMVAC",
        "NOTES": "Data cache invalidate by address to the Point of Coherency  "
    },
    {
        "Base_Addr": 0xe000ef60,
        "End_Addr": 0xe000ef63,
        "NAME_KEY": "DCISW",
        "NOTES": "Data cache invalidate by set/way  "
    },
    {
        "Base_Addr": 0xe000ef64,
        "End_Addr": 0xe000ef67,
        "NAME_KEY": "DCCMVAU",
        "NOTES": "Data cache clean by address to the Point of Unification  "
    },
    {
        "Base_Addr": 0xe000ef68,
        "End_Addr": 0xe000ef6b,
        "NAME_KEY": "DCCMVAC",
        "NOTES": "Data cache clean by address to the Point of Coherency  "
    },
    {
        "Base_Addr": 0xe000ef6c,
        "End_Addr": 0xe000ef6f,
        "NAME_KEY": "DCCSW",
        "NOTES": "Data cache clean by set/way  "
    },
    {
        "Base_Addr": 0xe000ef70,
        "End_Addr": 0xe000ef73,
        "NAME_KEY": "DCCIMVAC",
        "NOTES": "Data cache clean and invalidate by address to the Point of Unification  "
    },
    {
        "Base_Addr": 0xe000ef74,
        "End_Addr": 0xe000ef77,
        "NAME_KEY": "DCCISW",
        "NOTES": "Data cache clean and invalidate by set/way  "
    },
    {
        "Base_Addr": 0xe000ef78,
        "End_Addr": 0xe000ef8f,
        "NAME_KEY": "BPIALL",
        "NOTES": "The BPIALL register is not implemented  "
    },
    {
        "Base_Addr": 0xe000ef90,
        "End_Addr": 0xe000ef93,
        "NAME_KEY": "ITCMCR",
        "NOTES": "Instruction Tightly-Coupled Memory Control Register  "
    },
    {
        "Base_Addr": 0xe000ef94,
        "End_Addr": 0xe000ef97,
        "NAME_KEY": "DTCMCR",
        "NOTES": "Data Tightly-Coupled Memory Control Register  "
    },
    {
        "Base_Addr": 0xe000ef98,
        "End_Addr": 0xe000ef9b,
        "NAME_KEY": "AHBPCR",
        "NOTES": "AHBP Control Register  "
    },
    {
        "Base_Addr": 0xe000ef9c,
        "End_Addr": 0xe000ef9f,
        "NAME_KEY": "CACR",
        "NOTES": "L1 Cache Control Register  "
    },
    {
        "Base_Addr": 0xe000efa0,
        "End_Addr": 0xe000efa7,
        "NAME_KEY": "AHBSCR",
        "NOTES": "AHB Slave Control Register  "
    },
    {
        "Base_Addr": 0xe000efa8,
        "End_Addr": 0xe000efcf,
        "NAME_KEY": "ABFSR",
        "NOTES": "Auxiliary Bus Fault Status Register  "
    },
    {
        "Base_Addr": 0xe000efd0,
        "End_Addr": 0xe000efdf,
        "NAME_KEY": "SCSID Peripheral ID4",
        "NOTES": "System Control Space identification values"
    },
    {
        "Base_Addr": 0xe000efe0,
        "End_Addr": 0xe000efe3,
        "NAME_KEY": "SCSID Peripheral ID0",
        "NOTES": "System Control Space identification values"
    },
    {
        "Base_Addr": 0xe000efe4,
        "End_Addr": 0xe000efe7,
        "NAME_KEY": "SCSID Peripheral ID1",
        "NOTES": "System Control Space identification values"
    },
    {
        "Base_Addr": 0xe000efe8,
        "End_Addr": 0xe000efeb,
        "NAME_KEY": "SCSID Peripheral ID2",
        "NOTES": "System Control Space identification values"
    },
    {
        "Base_Addr": 0xe000efec,
        "End_Addr": 0xe000efef,
        "NAME_KEY": "SCSID Peripheral ID3",
        "NOTES": "System Control Space identification values"
    },
    {
        "Base_Addr": 0xe000eff0,
        "End_Addr": 0xe000eff3,
        "NAME_KEY": "SCSID Component ID0",
        "NOTES": "System Control Space identification values"
    },
    {
        "Base_Addr": 0xe000eff4,
        "End_Addr": 0xe000eff7,
        "NAME_KEY": "SCSID Component ID1",
        "NOTES": "System Control Space identification values"
    },
    {
        "Base_Addr": 0xe000eff8,
        "End_Addr": 0xe000effb,
        "NAME_KEY": "SCSID Component ID2",
        "NOTES": "System Control Space identification values"
    },
    {
        "Base_Addr": 0xe000effc,
        "End_Addr": 0xe003ffff,
        "NAME_KEY": "SCSID Component ID3",
        "NOTES": "System Control Space identification values"
    },
    {
        "Base_Addr": 0xe0040000,
        "End_Addr": 0xe0040003,
        "NAME_KEY": "TPIU_SSPSR",
        "NOTES": "Trace Port Interface Unit Supported Synchronous Port Size Register"
    },
    {
        "Base_Addr": 0xe0040004,
        "End_Addr": 0xe004000f,
        "NAME_KEY": "TPIU_CSPSR",
        "NOTES": "Trace Port Interface Unit Current Synchronous Port Size Register"
    },
    {
        "Base_Addr": 0xe0040010,
        "End_Addr": 0xe00400ef,
        "NAME_KEY": "TPIU_ACPR",
        "NOTES": "Trace Port Interface Unit Asynchronous Clock Prescaler Register"
    },
    {
        "Base_Addr": 0xe00400f0,
        "End_Addr": 0xe00402ff,
        "NAME_KEY": "TPIU_SPPR",
        "NOTES": "Trace Port Interface Unit Selected Pin Protocol Register"
    },
    {
        "Base_Addr": 0xe0040300,
        "End_Addr": 0xe0040303,
        "NAME_KEY": "TPIU_FFSR",
        "NOTES": "Trace Port Interface Unit Formatter and Flush Status Register"
    },
    {
        "Base_Addr": 0xe0040304,
        "End_Addr": 0xe0040307,
        "NAME_KEY": "TPIU_FFCR",
        "NOTES": "Trace Port Interface Unit Formatter and Flush Control Register"
    },
    {
        "Base_Addr": 0xe0040308,
        "End_Addr": 0xe0040ee7,
        "NAME_KEY": "TPIU_FSCR",
        "NOTES": "Trace Port Interface Unit Formatter Synchronization Counter Register"
    },
    {
        "Base_Addr": 0xe0040ee8,
        "End_Addr": 0xe0040fff,
        "NAME_KEY": "TPIU_OTHER",
        "NOTES": "Trace Port Interface Unit OTHER"
    },
    {
        "Base_Addr": 0xe0041000,
        "End_Addr": 0xe0041fff,
        "NAME_KEY": "ETM",
        "NOTES": "Embedded Trace Macrocell"
    },
    {
        "Base_Addr": 0xe0042000,
        "End_Addr": 0xe00fefff,
        "NAME_KEY": "ExPPB",
        "NOTES": "External PPB"
    },
    {
        "Base_Addr": 0xe00ff000,
        "End_Addr": 0xe00fffcf,
        "NAME_KEY": "ROMTBL",
        "NOTES": "Cortex-M4 ROM table components"
    },
    {
        "Base_Addr": 0xe00fffd0,
        "End_Addr": 0xe00fffdf,
        "NAME_KEY": "ROM Peripheral ID4",
        "NOTES": "Cortex-M4 ROM table identification values  "
    },
    {
        "Base_Addr": 0xe00fffe0,
        "End_Addr": 0xe00fffe3,
        "NAME_KEY": "ROM Peripheral ID0",
        "NOTES": "Cortex-M4 ROM table identification values  "
    },
    {
        "Base_Addr": 0xe00fffe4,
        "End_Addr": 0xe00fffe7,
        "NAME_KEY": "ROM Peripheral ID1",
        "NOTES": "Cortex-M4 ROM table identification values  "
    },
    {
        "Base_Addr": 0xe00fffe8,
        "End_Addr": 0xe00fffeb,
        "NAME_KEY": "ROM Peripheral ID2",
        "NOTES": "Cortex-M4 ROM table identification values  "
    },
    {
        "Base_Addr": 0xe00fffec,
        "End_Addr": 0xe00fffef,
        "NAME_KEY": "ROM Peripheral ID3",
        "NOTES": "Cortex-M4 ROM table identification values  "
    },
    {
        "Base_Addr": 0xe00ffff0,
        "End_Addr": 0xe00ffff3,
        "NAME_KEY": "ROM Component ID0",
        "NOTES": "Cortex-M4 ROM table identification values  "
    },
    {
        "Base_Addr": 0xe00ffff4,
        "End_Addr": 0xe00ffff7,
        "NAME_KEY": "ROM Component ID1",
        "NOTES": "Cortex-M4 ROM table identification values  "
    },
    {
        "Base_Addr": 0xe00ffff8,
        "End_Addr": 0xe00ffffb,
        "NAME_KEY": "ROM Component ID2",
        "NOTES": "Cortex-M4 ROM table identification values  "
    },
    {
        "Base_Addr": 0xe00ffffc,
        "End_Addr": 0xE00FFFFF,
        "NAME_KEY": "ROM Component ID3",
        "NOTES": "Cortex-M4 ROM table identification values  "
    }
]

  }
}

# peripherals = DEVICE_DEFS[device_name][PERIPHERAL_KEY]

def get_peripherals_name(pts_addr):
  if pts_addr > 0:
    for ph in DEVICE_DEFS['STM32F4Discovery']["Peripherals"]:
          if pts_addr>=ph["Base_Addr"] and pts_addr<=ph["End_Addr"]:
            ph_range = ph["End_Addr"] - ph["Base_Addr"] + 1
            return  hex(ph['Base_Addr']),"rw",ph['NAME_KEY'],hex(ph_range)
    return None
  else:
    for ph in DEVICE_DEFS['STM32F4Discovery']["Peripherals"]:
          pts_unaddr = pts_addr+0x100000000
          if pts_unaddr>=ph["Base_Addr"] and pts_unaddr<=ph["End_Addr"]:
            ph_range = ph["End_Addr"] - ph["Base_Addr"] + 1
          # print(hex(pts_unaddr))
            return  hex(ph['Base_Addr']),"rw",ph['NAME_KEY'],hex(ph_range)
    return None

def get_private_peripherals_name(pts_addr):
  if pts_addr > 0:
    for ph in DEVICE_DEFS['STM32F4Discovery']["Whitelist"]:
          if pts_addr>=ph["Base_Addr"] and pts_addr<=ph["End_Addr"]:
            ph_range = ph["End_Addr"] - ph["Base_Addr"] + 1
            return  [hex(ph['Base_Addr']),"rw",ph['NAME_KEY'],hex(ph_range)]
    return None
  else:
    for ph in DEVICE_DEFS['STM32F4Discovery']["Whitelist"]:
          pts_unaddr = pts_addr+0x100000000
          if pts_unaddr>=ph["Base_Addr"] and pts_unaddr<=ph["End_Addr"]:
            ph_range = ph["End_Addr"] - ph["Base_Addr"] + 1
            # print(hex(pts_unaddr))
            return  [hex(ph['Base_Addr']),"rw",ph['NAME_KEY'],hex(ph_range)]
    return None

# if __name__ == '__main__':
#       ph_name = get_peripherals_name(0x40023800)
#       print(ph_name)



