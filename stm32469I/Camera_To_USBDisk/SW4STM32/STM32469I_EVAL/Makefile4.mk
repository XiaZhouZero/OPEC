######################################
# target
######################################
TARGET = Camera_To_USBDisk
OI_ROOT=$(OI_DIR)
######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -O0

#######################################
# paths
#######################################
# Build path
APP_DIR = $(shell pwd)
BUILD4_DIR = build4
BUILD9_DIR = build9
LLVM_ROOT = $(OI_ROOT)llvm4/
GCC_ROOT = $(OI_ROOT)gcc/bins/

######################################
# source
######################################
# C sources
C_SOURCES = \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c \
	../../Src/stm32f4xx_it.c \
	../../../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
	../../../Drivers/BSP/Components/otm8009a/otm8009a.c \
	../../../Middlewares/Third_Party/FatFs/Drivers/usbh_diskio_dma.c \
	../../../Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ioreq.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
	../../Src/usbh_conf.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma2d.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_hcd.c \
	../../../Middlewares/Third_Party/FatFs/src/ff_gen_drv.c \
	../../../Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_core.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c \
	../../../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
	../../../Drivers/BSP/STM32469I_EVAL/stm32469i_eval_camera.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dcmi.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
	../../../Drivers/BSP/STM32469I_EVAL/stm32469i_eval_sdram.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dsi.c \
	../../../Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.c \
	../../../Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_pipes.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sdram.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
	../../../Drivers/BSP/STM32469I_EVAL/stm32469i_eval_io.c \
	../../../Drivers/CMSIS/system_stm32f4xx.c \
	../../../Middlewares/Third_Party/FatFs/src/ff.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fmc.c \
	../../../Drivers/BSP/Components/mfxstm32l152/mfxstm32l152.c \
	../../../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_ltdc.c \
	../../../Drivers/BSP/STM32469I_EVAL/stm32469i_eval_lcd.c \
	../../../Drivers/BSP/STM32469I_EVAL/stm32469i_eval.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_ltdc_ex.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
	../../../Drivers/BSP/Components/s5k5cag/s5k5cag.c \
	../../../Drivers/BSP/Components/ov5640/ov5640.c \
	../../Src/main.c \
	../../../Middlewares/Third_Party/FatFs/src/diskio.c \
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c 

# ASM sources
ASM_SOURCES = \
../startup_stm32f479xx.s


#######################################
# binaries
#######################################
AS = $(LLVM_ROOT)build/bin/llvm-as
OP = $(LLVM_ROOT)build/bin/opt
CC = $(LLVM_ROOT)build/bin/clang
CP = $(LLVM_ROOT)build/bin/llvm-objcopy
SZ = $(LLVM_ROOT)build/bin/llvm-size
DIS = $(LLVM_ROOT)build/bin/llvm-dis

HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4
# fpu
FPU = -mfpu=fpv4-sp-d16
# float-abi
FLOAT-ABI = -mfloat-abi=soft
# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)


# macros for gcc

# C defines
C_DEFS = -DUSE_HAL_DRIVER -DSTM32F469xx -DUSE_STM32479I_EVAL -DUSE_USB_HS -DUSE_IOEXPANDER -DHAL_HCD_MODULE_ENABLED

# C includes
C_INCLUDES = -I ../../Inc \
	-I  ../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include \
	-I  ../../../Drivers/STM32F4xx_HAL_Driver/Inc \
	-I  ../../../Drivers/BSP/STM32469I_EVAL \
	-I  ../../../Drivers/BSP/Components/mfxstm32l152 \
	-I  ../../../Drivers/BSP/Components/Common \
	-I  ../../../Drivers/BSP/Components \
	-I  ../../../Utilities \
	-I  ../../../Utilities/fonts \
	-I  ../../../Middlewares/ST/STM32_USB_Host_Library/Core/Inc \
	-I  ../../../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc \
	-I  ../../../Middlewares/Third_Party/FatFs/src \
	-I  ../../../Drivers/CMSIS/Include

C_INCLUDES += -I $(GCC_ROOT)/arm-none-eabi/include
# compile gcc flags
ASFLAGS = $(MCU)  $(C_INCLUDES) $(OPT) -Wall -target arm-none-eabi 

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -target arm-none-eabi -fshort-enums \
--sysroot=$(ARM_NONE_EABI_PATH)arm-none-eabi -ffreestanding -fno-builtin -fno-exceptions
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2 
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F479NIHx_FLASH
LLVMGOLD=  $(LLVM_ROOT)build/lib/LLVMgold.so
# libraries
LIBS = -lc -lm -lgcc -lnosys 
#-lnosys 
LIBDIR = \
-L $(GCC_ROOT)arm-none-eabi/lib/thumb/v7e-m \
-L $(GCC_ROOT)lib/gcc/arm-none-eabi/6.3.1/thumb/v7e-m  \
-I $(GCC_ROOT)arm-none-eabi/include
LDFLAGS =    $(LIBDIR) $(LIBS) \
-plugin=$(LLVMGOLD) \
--plugin-opt=O0 \
--plugin-opt=save-temps \
--plugin-opt=-float-abi=soft \
--plugin-opt=-mcpu=cortex-m4 \
--gc-sections

all: $(BUILD4_DIR)/$(TARGET)--baseline.elf $(BUILD4_DIR)/$(TARGET).elf $(BUILD4_DIR)/$(TARGET)--oi--final.elf


#######################################
# build the application
########################$(GCC_ROOT)###############
# list of objects
OBJECTS = $(addprefix $(BUILD4_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
ASMOBJECTS += $(addprefix $(BUILD4_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD4_DIR)/%.o: %.c Makefile | $(BUILD4_DIR)
	$(CC) -flto -c $(CFLAGS)  $< -o $@

$(BUILD4_DIR)/%.o: %.s Makefile | $(BUILD4_DIR)
	$(CC) -flto -c $(ASFLAGS) $< -o $@


$(BUILD4_DIR)/$(TARGET)--baseline.elf:$(ASMOBJECTS) $(OBJECTS) Makefile
	$(GCC_ROOT)bin/arm-none-eabi-ld $(ASMOBJECTS) $(OBJECTS) $(LDFLAGS) -T$(LDSCRIPT).ld \
	-o $@ -g
	$(SZ) $@ 


$(BUILD4_DIR)/$(TARGET).elf:$(ASMOBJECTS) $(OBJECTS) Makefile
	# gen the inter link script from the policy file
	# there will be an *_inter.ld  /home/nn/Desktop/OI_project/
	python3 $(OI_ROOT)compiler/analysis/scripts/generate_policy.py \
                    -sp $(BUILD9_DIR)/$(TARGET)_final_policy.json \
                    -ld_src $(LDSCRIPT).ld -ld_num 1

	$(GCC_ROOT)bin/arm-none-eabi-ld $(ASMOBJECTS) $(OBJECTS) $(LDFLAGS) -T$(LDSCRIPT)_inter.ld \
    $(OI_ROOT)compiler/operation-rt/operation-rt-lib/operation-rt-v7e-m.o \
	--plugin-opt=-info-output-file=$@.stats \
	--plugin-opt=-OIpolicy=$(BUILD9_DIR)/$(TARGET)_final_policy.json \
	--plugin-opt=-debug-only=OIApplication \
    -o $@ -g > $(BUILD4_DIR)/oiapplication_pass_1.log 2>&1
	$(SZ) $@
    # update policy file and LINKER_SCRIPT
	python3 $(OI_ROOT)compiler/analysis/scripts/generate_policy.py \
					-o $(BUILD4_DIR)/$(TARGET).elf \
                    -sp $(BUILD9_DIR)/$(TARGET)_final_policy.json \
                    -m 	$(TARGET)_mpu_config.json \
                    -ld_src $(LDSCRIPT).ld -ld_num 2

# Generate final Partitioned Binary
$(BUILD4_DIR)/$(TARGET)--oi--final.elf: $(BUILD4_DIR)/$(TARGET).elf
	$(GCC_ROOT)bin/arm-none-eabi-ld $(ASMOBJECTS) $(OBJECTS) $(LDFLAGS) -T$(LDSCRIPT)_final.ld \
    $(OI_ROOT)compiler/operation-rt/operation-rt-lib/operation-rt-v7e-m.o \
	--plugin-opt=-info-output-file=$@.stats \
	--plugin-opt=-OIpolicy=$(BUILD9_DIR)/$(TARGET)_final_policy.json \
	--plugin-opt=-debug-only=OIApplication \
	-o $@ -g > $(BUILD4_DIR)/oiapplication_pass_2.log 2>&1
	$(SZ) $@
	$(DIS) $(APP_DIR)/$(BUILD4_DIR)/$(TARGET)--oi--final.elf.0.5.precodegen.bc -o $(APP_DIR)/$(BUILD4_DIR)/$(TARGET)--oi--final.elf.0.5.precodegen.ll


$(BUILD4_DIR):
	mkdir $@

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD4_DIR)
	-rm `ls *.json | egrep -v Entry_functions.json | egrep -v gv_2_peripheral.json`
	-rm `ls *.ld | egrep -v $(LDSCRIPT).ld`

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD4_DIR)/*.d)

# *** EOF ***