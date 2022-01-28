DEVICE_DEFS = {
    "STM32F4Discovery": {
        "Include": ["ARMv7-M"],
        "Peripherals": [
            {'NAME_KEY':"FSMC",           "Base_Addr":0xA0000000,"End_Addr":0xA0000FFF},
            {'NAME_KEY':"RNG",            "Base_Addr":0x50060800,"End_Addr":0x50060BFF},
            {'NAME_KEY':"HASH",           "Base_Addr":0x50060400,"End_Addr":0x500607FF},
            {'NAME_KEY':"CRYP",           "Base_Addr":0x50060000,"End_Addr":0x500603FF},
            {'NAME_KEY':"DCMI",           "Base_Addr":0x50050000,"End_Addr":0x500503FF},
            {'NAME_KEY':"USB-OTG-FS",     "Base_Addr":0x50000000,"End_Addr":0x5003FFFF},
            {'NAME_KEY':"USB-OTG-HS",     "Base_Addr":0x40040000,"End_Addr":0x4007FFFF},
            {'NAME_KEY':"DMA2D",          "Base_Addr":0x4002B000,"End_Addr":0x4002BBFF},
            # {'NAME_KEY':"ETHERNET-MAC5",  "Base_Addr":0x40029000,"End_Addr":0x400293FF},
            # {'NAME_KEY':"ETHERNET-MAC4",  "Base_Addr":0x40028C00,"End_Addr":0x40028FFF},
            # {'NAME_KEY':"ETHERNET-MAC3",  "Base_Addr":0x40028800,"End_Addr":0x40028BFF},
            # {'NAME_KEY':"ETHERNET-MAC2",  "Base_Addr":0x40028400,"End_Addr":0x400287FF},
            # {'NAME_KEY':"ETHERNET-MAC1",  "Base_Addr":0x40028000,"End_Addr":0x400283FF},
            {'NAME_KEY':"ETHERNET-MAC",   "Base_Addr":0x40028000,"End_Addr":0x400293FF},
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
            {'NAME_KEY':"DSI-Host_CP",    "Base_Addr":0x40017000,"End_Addr":0x400173FF},
            {'NAME_KEY':"DSI-Host",       "Base_Addr":0x40016C00,"End_Addr":0x40016fff},    # due to MPU region alignment issue, split the Peripheral from 0x800 to 0x400*2
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
            # {'NAME_KEY':"Non-Periph",     "Base_Addr":0x00000000,"End_Addr":0x3FFFFFFF,"EXCLUDE":True},
            {"NAME_KEY":"USB-OTG-HS_bit_band",     "Base_Addr":0x42800000, "End_Addr":0x42FFFFFF},
            {"NAME_KEY":"DMA2D_bit_band",          "Base_Addr":0x42560000, "End_Addr":0x42577FFF},
            {"NAME_KEY":"ETHERNET-MAC5_bit_band",  "Base_Addr":0x42520000, "End_Addr":0x42527FFF},
            {"NAME_KEY":"ETHERNET-MAC4_bit_band",  "Base_Addr":0x42518000, "End_Addr":0x4251FFFF},
            {"NAME_KEY":"ETHERNET-MAC3_bit_band",  "Base_Addr":0x42510000, "End_Addr":0x42517FFF},
            {"NAME_KEY":"ETHERNET-MAC2_bit_band",  "Base_Addr":0x42508000, "End_Addr":0x4250FFFF},
            {"NAME_KEY":"ETHERNET-MAC1_bit_band",  "Base_Addr":0x42500000, "End_Addr":0x42507FFF},
            {"NAME_KEY":"DMA2_bit_band",           "Base_Addr":0x424c8000, "End_Addr":0x424CFFFF},
            {"NAME_KEY":"DMA1_bit_band",           "Base_Addr":0x424c0000, "End_Addr":0x424C7FFF},
            {"NAME_KEY":"BKPSRAM_bit_band",        "Base_Addr":0x42480000, "End_Addr":0x4249FFFF},
            {"NAME_KEY":"FLASH-Inte_bit_band",     "Base_Addr":0x42478000, "End_Addr":0x4247FFFF},
            {"NAME_KEY":"RCC_bit_band",            "Base_Addr":0x42470000, "End_Addr":0x42477FFF},
            {"NAME_KEY":"CRC_bit_band",            "Base_Addr":0x42460000, "End_Addr":0x42467FFF},
            {"NAME_KEY":"GPIOK_bit_band",          "Base_Addr":0x42450000, "End_Addr":0x42457FFF},
            {"NAME_KEY":"GPIOJ_bit_band",          "Base_Addr":0x42448000, "End_Addr":0x4244FFFF},
            {"NAME_KEY":"GPIOI_bit_band",          "Base_Addr":0x42440000, "End_Addr":0x42447FFF},
            {"NAME_KEY":"GPIOH_bit_band",          "Base_Addr":0x42438000, "End_Addr":0x4243FFFF},
            {"NAME_KEY":"GPIOG_bit_band",          "Base_Addr":0x42430000, "End_Addr":0x42437FFF},
            {"NAME_KEY":"GPIOF_bit_band",          "Base_Addr":0x42428000, "End_Addr":0x4242FFFF},
            {"NAME_KEY":"GPIOE_bit_band",          "Base_Addr":0x42420000, "End_Addr":0x42427FFF},
            {"NAME_KEY":"GPIOD_bit_band",          "Base_Addr":0x42418000, "End_Addr":0x4241FFFF},
            {"NAME_KEY":"GPIOC_bit_band",          "Base_Addr":0x42410000, "End_Addr":0x42417FFF},
            {"NAME_KEY":"GPIOB_bit_band",          "Base_Addr":0x42408000, "End_Addr":0x4240FFFF},
            {"NAME_KEY":"GPIOA_bit_band",          "Base_Addr":0x42400000, "End_Addr":0x42407FFF},
            {"NAME_KEY":"LCD-TFT_bit_band",        "Base_Addr":0x422D0000, "End_Addr":0x422d7FFF},
            {"NAME_KEY":"SAI1_bit_band",           "Base_Addr":0x422B0000, "End_Addr":0x422B7FFF},
            {"NAME_KEY":"SPI6_bit_band",           "Base_Addr":0x422A8000, "End_Addr":0x422AFFFF},
            {"NAME_KEY":"SPI5_bit_band",           "Base_Addr":0x422A0000, "End_Addr":0x422A7FFF},
            {"NAME_KEY":"TIM11_bit_band",          "Base_Addr":0x42290000, "End_Addr":0x42297FFF},
            {"NAME_KEY":"TIM10_bit_band",          "Base_Addr":0x42288000, "End_Addr":0x4228FFFF},
            {"NAME_KEY":"TIM9_bit_band",           "Base_Addr":0x42280000, "End_Addr":0x42287FFF},
            {"NAME_KEY":"EXTI_bit_band",           "Base_Addr":0x42278000, "End_Addr":0x4227FFFF},
            {"NAME_KEY":"SYSCFG_bit_band",         "Base_Addr":0x42270000, "End_Addr":0x42277FFF},
            {"NAME_KEY":"SPI4_bit_band",           "Base_Addr":0x42268000, "End_Addr":0x4226FFFF},
            {"NAME_KEY":"SPI1_bit_band",           "Base_Addr":0x42260000, "End_Addr":0x42267FFF},
            {"NAME_KEY":"SDIO_bit_band",           "Base_Addr":0x42258000, "End_Addr":0x4225FFFF},
            {"NAME_KEY":"ADC1-3_bit_band",         "Base_Addr":0x42240000, "End_Addr":0x42247FFF},
            {"NAME_KEY":"USART6_bit_band",         "Base_Addr":0x42228000, "End_Addr":0x4222FFFF},
            {"NAME_KEY":"USART1_bit_band",         "Base_Addr":0x42220000, "End_Addr":0x42227FFF},
            {"NAME_KEY":"TIM8_bit_band",           "Base_Addr":0x42208000, "End_Addr":0x4220FFFF},
            {"NAME_KEY":"TIM1_bit_band",           "Base_Addr":0x42200000, "End_Addr":0x42207FFF},
            {"NAME_KEY":"USART8_bit_band",         "Base_Addr":0x420F8000, "End_Addr":0x420FFFFF},
            {"NAME_KEY":"USART7_bit_band",         "Base_Addr":0x420F0000, "End_Addr":0x420F7FFF},
            {"NAME_KEY":"DAC_bit_band",            "Base_Addr":0x420E8000, "End_Addr":0x420EFFFF},
            {"NAME_KEY":"PWR_bit_band",            "Base_Addr":0x420E0000, "End_Addr":0x420E7FFF},
            {"NAME_KEY":"CAN2_bit_band",           "Base_Addr":0x420D0000, "End_Addr":0x420d7FFF},
            {"NAME_KEY":"CAN1_bit_band",           "Base_Addr":0x420C8000, "End_Addr":0x420cFFFF},
            {"NAME_KEY":"I2C3_bit_band",           "Base_Addr":0x420B8000, "End_Addr":0x420BFFFF},
            {"NAME_KEY":"I2C2_bit_band",           "Base_Addr":0x420B0000, "End_Addr":0x420B7FFF},
            {"NAME_KEY":"I2C1_bit_band",           "Base_Addr":0x420A8000, "End_Addr":0x420AFFFF},
            {"NAME_KEY":"UART5_bit_band",          "Base_Addr":0x420A0000, "End_Addr":0x420A7FFF},
            {"NAME_KEY":"UART4_bit_band",          "Base_Addr":0x42098000, "End_Addr":0x4209FFFF},
            {"NAME_KEY":"USART3_bit_band",         "Base_Addr":0x42090000, "End_Addr":0x42097FFF},
            {"NAME_KEY":"USART2_bit_band",         "Base_Addr":0x42088000, "End_Addr":0x4208FFFF},
            {"NAME_KEY":"I2S3ext_bit_band",        "Base_Addr":0x42080000, "End_Addr":0x42087FFF},
            {"NAME_KEY":"SPI3_bit_band",           "Base_Addr":0x42078000, "End_Addr":0x4207FFFF},
            {"NAME_KEY":"SPI2_bit_band",           "Base_Addr":0x42070000, "End_Addr":0x42077FFF},
            {"NAME_KEY":"I2S2ext_bit_band",        "Base_Addr":0x42068000, "End_Addr":0x4206FFFF},
            {"NAME_KEY":"IWDG_bit_band",           "Base_Addr":0x42060000, "End_Addr":0x42067FFF},
            {"NAME_KEY":"WWDG_bit_band",           "Base_Addr":0x42058000, "End_Addr":0x4205FFFF},
            {"NAME_KEY":"RTC-BKP_bit_band",        "Base_Addr":0x42050000, "End_Addr":0x42057FFF},
            {"NAME_KEY":"TIM14_bit_band",          "Base_Addr":0x42040000, "End_Addr":0x42047FFF},
            {"NAME_KEY":"TIM13_bit_band",          "Base_Addr":0x42038000, "End_Addr":0x4203FFFF},
            {"NAME_KEY":"TIM12_bit_band",          "Base_Addr":0x42030000, "End_Addr":0x42037FFF},
            {"NAME_KEY":"TIM7_bit_band",           "Base_Addr":0x42028000, "End_Addr":0x4202FFFF},
            {"NAME_KEY":"TIM6_bit_band",           "Base_Addr":0x42020000, "End_Addr":0x42027FFF},
            {"NAME_KEY":"TIM5_bit_band",           "Base_Addr":0x42018000, "End_Addr":0x4201FFFF},
            {"NAME_KEY":"TIM4_bit_band",           "Base_Addr":0x42010000, "End_Addr":0x42017FFF},
            {"NAME_KEY":"TIM3_bit_band",           "Base_Addr":0x42008000, "End_Addr":0x4200FFFF},
            {"NAME_KEY":"TIM2_bit_band",           "Base_Addr":0x42000000, "End_Addr":0x42007FFF}
            # {"NAME_KEY":"LTDC",                    "Base_Addr":0xC0000000, "End_Addr":0xC00091FF}
        ],

        "Whitelist":[
            {
                "Base_Addr": 0xe0000000,
                "End_Addr": 0xe0000FFF,
                "NAME_KEY": "ITM",
                "NOTES": "Instrumentation Trace Macrocell (ITM) unit Stimulus Port Registers 0-31"
            },
            {
                "Base_Addr": 0xe0001000,
                "End_Addr": 0xe0001FFF,
                "NAME_KEY": "DWT",
                "NOTES": "Data Watchpoint and Trace (DWT) unit "
            },
            {
                "Base_Addr": 0xe0002000,
                "End_Addr": 0xe000e003,
                "NAME_KEY": "FP",
                "NOTES": "Flash Patch and Breakpoint (FPB) Unit FlashPatch Control Register"
            },
            {
                "Base_Addr": 0xe000e004,
                "End_Addr": 0xe000e007,
                "NAME_KEY": "ICTR",
                "NOTES": "Interrupt Controller Type Register, ICTR"
            },
            {
                "Base_Addr": 0xe000e008,
                "End_Addr": 0xe000e00F,
                "NAME_KEY": "ACTLR",
                "NOTES": "Auxiliary Control Register Control Register"
            },
            {
                "Base_Addr": 0xe000e010,
                "End_Addr": 0xe000e17F,
                "NAME_KEY": "SysTick",
                "NOTES": "SysTick Control and Status Register"
            },
            {
                "Base_Addr": 0xe000e100,
                "End_Addr": 0xe000ecFF,
                "NAME_KEY": "NVIC",
                "NOTES": "Interrupt Set-Enable Registers"
            },
            {
                "Base_Addr": 0xe000ed00,
                "End_Addr": 0xe000ed8F,
                "NAME_KEY": "SCB",
                "NOTES": "SCB registers"
            },
            {
                "Base_Addr": 0xe000ed90,
                "End_Addr": 0xe000edeF,
                "NAME_KEY": "MPU",
                "NOTES": "MPU All Registers"
            },
            {
                "Base_Addr": 0xe000edF0,
                "End_Addr": 0xe000edF3,
                "NAME_KEY": "DHCSR",
                "NOTES": "Debug Halting Control and Status Register  "
            },
            {
                "Base_Addr": 0xe000edF4,
                "End_Addr": 0xe000edF7,
                "NAME_KEY": "DCRSR",
                "NOTES": "Debug Core Register Selector Register  "
            },
            {
                "Base_Addr": 0xe000edF8,
                "End_Addr": 0xe000edFb,
                "NAME_KEY": "DCRDR",
                "NOTES": "Debug Core Register Data Register  "
            },
            {
                "Base_Addr": 0xe000edFc,
                "End_Addr": 0xe000eeFF,
                "NAME_KEY": "DEMCR",
                "NOTES": "Debug Exception and Monitor Control Register  "
            },
            {
                "Base_Addr": 0xe000eF00,
                "End_Addr": 0xe000eF33,
                "NAME_KEY": "STIR",
                "NOTES": "Software Trigger Interrupt Register"
            },
            {
                "Base_Addr": 0xe000eF34,
                "End_Addr": 0xe000eF37,
                "NAME_KEY": "FPCCR",
                "NOTES": "FP Context Control Register  "
            },
            {
                "Base_Addr": 0xe000eF38,
                "End_Addr": 0xe000eF3b,
                "NAME_KEY": "FPCAR",
                "NOTES": "FP Context Address Register  "
            },
            {
                "Base_Addr": 0xe000eF3c,
                "End_Addr": 0xe000eF3F,
                "NAME_KEY": "FPDSCR",
                "NOTES": "FP Default Status Control Register  "
            },
            {
                "Base_Addr": 0xe000eF40,
                "End_Addr": 0xe000eF43,
                "NAME_KEY": "MVFR0",
                "NOTES": "Media and VFP Feature Register 0, MVFR0  "
            },
            {
                "Base_Addr": 0xe000eF44,
                "End_Addr": 0xe000eF4F,
                "NAME_KEY": "MVFR1",
                "NOTES": "Media and VFP Feature Register 1, MVFR1  "
            },
            {
                "Base_Addr": 0xe000eF50,
                "End_Addr": 0xe000eF57,
                "NAME_KEY": "ICIALLU1",
                "NOTES": "Instruction cache invalidate all to the Point of Unification  "
            },
            {
                "Base_Addr": 0xe000eF58,
                "End_Addr": 0xe000eF5b,
                "NAME_KEY": "ICIALLU2",
                "NOTES": "Instruction cache invalidate by address to the Point of Unification  "
            },
            {
                "Base_Addr": 0xe000eF5c,
                "End_Addr": 0xe000eF5F,
                "NAME_KEY": "DCIMVAC",
                "NOTES": "Data cache invalidate by address to the Point of Coherency  "
            },
            {
                "Base_Addr": 0xe000eF60,
                "End_Addr": 0xe000eF63,
                "NAME_KEY": "DCISW",
                "NOTES": "Data cache invalidate by set/way  "
            },
            {
                "Base_Addr": 0xe000eF64,
                "End_Addr": 0xe000eF67,
                "NAME_KEY": "DCCMVAU",
                "NOTES": "Data cache clean by address to the Point of Unification  "
            },
            {
                "Base_Addr": 0xe000eF68,
                "End_Addr": 0xe000eF6b,
                "NAME_KEY": "DCCMVAC",
                "NOTES": "Data cache clean by address to the Point of Coherency  "
            },
            {
                "Base_Addr": 0xe000eF6c,
                "End_Addr": 0xe000eF6F,
                "NAME_KEY": "DCCSW",
                "NOTES": "Data cache clean by set/way  "
            },
            {
                "Base_Addr": 0xe000eF70,
                "End_Addr": 0xe000eF73,
                "NAME_KEY": "DCCIMVAC",
                "NOTES": "Data cache clean and invalidate by address to the Point of Unification  "
            },
            {
                "Base_Addr": 0xe000eF74,
                "End_Addr": 0xe000eF77,
                "NAME_KEY": "DCCISW",
                "NOTES": "Data cache clean and invalidate by set/way  "
            },
            {
                "Base_Addr": 0xe000eF78,
                "End_Addr": 0xe000eF8F,
                "NAME_KEY": "BPIALL",
                "NOTES": "The BPIALL register is not implemented  "
            },
            {
                "Base_Addr": 0xe000eF90,
                "End_Addr": 0xe000eF93,
                "NAME_KEY": "ITCMCR",
                "NOTES": "Instruction Tightly-Coupled Memory Control Register  "
            },
            {
                "Base_Addr": 0xe000eF94,
                "End_Addr": 0xe000eF97,
                "NAME_KEY": "DTCMCR",
                "NOTES": "Data Tightly-Coupled Memory Control Register  "
            },
            {
                "Base_Addr": 0xe000eF98,
                "End_Addr": 0xe000eF9b,
                "NAME_KEY": "AHBPCR",
                "NOTES": "AHBP Control Register  "
            },
            {
                "Base_Addr": 0xe000eF9c,
                "End_Addr": 0xe000eF9F,
                "NAME_KEY": "CACR",
                "NOTES": "L1 Cache Control Register  "
            },
            {
                "Base_Addr": 0xe000eFa0,
                "End_Addr": 0xe000eFa7,
                "NAME_KEY": "AHBSCR",
                "NOTES": "AHB Slave Control Register  "
            },
            {
                "Base_Addr": 0xe000eFa8,
                "End_Addr": 0xe000eFcF,
                "NAME_KEY": "ABFSR",
                "NOTES": "Auxiliary Bus Fault Status Register  "
            },
            {
                "Base_Addr": 0xe000eFd0,
                "End_Addr": 0xe003FFFF,
                "NAME_KEY": "SCSID",
                "NOTES": "System Control Space identification values"
            },
            {
                "Base_Addr": 0xe0040000,
                "End_Addr": 0xe0040FFF,
                "NAME_KEY": "TPIU",
                "NOTES": "Trace Port Interface Unit"
            },
            {
                "Base_Addr": 0xe0041000,
                "End_Addr": 0xe0041FFF,
                "NAME_KEY": "ETM",
                "NOTES": "Embedded Trace Macrocell"
            },
            {
                "Base_Addr": 0xe0042000,
                "End_Addr": 0xe00FeFFF,
                "NAME_KEY": "ExPPB",
                "NOTES": "External PPB"
            },
            {
                "Base_Addr": 0xe00FF000,
                "End_Addr": 0xe00FFFcF,
                "NAME_KEY": "ROMTBL",
                "NOTES": "Cortex-M4 ROM table components"
            },
            {
                "Base_Addr": 0xe00FFFd0,
                "End_Addr": 0xe00FFFdF,
                "NAME_KEY": "ROM Peripheral ID4",
                "NOTES": "Cortex-M4 ROM table identification values  "
            },
            {
                "Base_Addr": 0xe00FFFe0,
                "End_Addr": 0xE00FFFFF,
                "NAME_KEY": "ROM Peripheral and Component",
                "NOTES": "Cortex-M4 ROM table identification values  "
            }
        ]
    }
}

# peripherals = DEVICE_DEFS[device_name][PERIPHERAL_KEY]

def get_peripherals_name(pts_addr):
  if pts_addr > 0:
    for ph in DEVICE_DEFS['STM32F4Discovery']["Peripherals"]:
          if pts_addr >= ph["Base_Addr"] and pts_addr <= ph["End_Addr"]:
            ph_range = ph["End_Addr"] - ph["Base_Addr"] + 1
            return hex(ph['Base_Addr']), "rw", ph['NAME_KEY'], hex(ph_range)
    return None
  else:
    for ph in DEVICE_DEFS['STM32F4Discovery']["Peripherals"]:
          pts_unaddr = pts_addr+0x100000000
          if pts_unaddr >= ph["Base_Addr"] and pts_unaddr <= ph["End_Addr"]:
            ph_range = ph["End_Addr"] - ph["Base_Addr"] + 1
          # print(hex(pts_unaddr))
            return hex(ph['Base_Addr']), "rw", ph['NAME_KEY'], hex(ph_range)
    return None


def get_ppb_name(pts_addr):
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


# tool functions for calculating bit band memory addr and region addr
def alias_word_2_regin_bit(alias_word_addr, alias_word_base_addr, regin_bit_base_addr):
    bit_band_byte = (alias_word_addr - alias_word_base_addr ) / 32
    bit_band_bit = ((alias_word_addr - alias_word_base_addr ) % 32 / 4)
    region_byte_addr = regin_bit_base_addr + bit_band_byte
    print("alias_word_addr:{}({}) maps to the {}:[{}]".format(hex(alias_word_addr), alias_word_addr, hex(region_byte_addr), bit_band_bit))

def regin_bit_2_alias_word(region_byte_addr, regin_bit_base_addr, bit_num, alias_word_base):
    alias_word_addr = (region_byte_addr - regin_bit_base_addr) * 32
    alias_word_addr += bit_num * 4 + alias_word_base
    print("region_addr: {}({}), bit: {} maps to alias_addr: {}".format(hex(region_byte_addr), region_byte_addr, bit_num, hex(alias_word_addr)))

# if __name__ == '__main__':
#       ph_name = get_peripherals_name(0x40023800)
#       print(ph_name)