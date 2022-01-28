######################################
# target
######################################
TARGET = LwIP_TCP_Echo_Server
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
STM32_ROOT = $(OI_ROOT)/STM32Cube_FW_F4_V1.14.0/
APP_ROOT = $(STM32_ROOT)/Projects/STM32469I_EVAL/Applications/LwIP/LwIP_TCP_Echo_Server/
######################################
# source
######################################
# C sources
# C_SOURCES = \
	../syscalls.c\
	../../Src/app_ethernet.c\
	../../Src/ethernetif.c\
	../../Src/main.c\
	../../Src/stm32f4xx_it.c\
	../../Src/tcp_echoserver.c\
	../../../Utilities/Log/lcd_log.c\
	../../../Drivers/CMSIS/system_stm32f4xx.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma2d.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dsi.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_eth.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_ltdc.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_ltdc_ex.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sdram.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c\
	../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fmc.c\
	../../../Drivers/BSP/Components/mfxstm32l152/mfxstm32l152.c\
	../../../Drivers/BSP/Components/otm8009a/otm8009a.c\
	../../../Drivers/BSP/STM32469I_EVAL/stm32469i_eval.c\
	../../../Drivers/BSP/STM32469I_EVAL/stm32469i_eval_io.c\
	../../../Drivers/BSP/STM32469I_EVAL/stm32469i_eval_lcd.c\
	../../../Drivers/BSP/STM32469I_EVAL/stm32469i_eval_sdram.c\
	../../../Middlewares/Third_Party/LwIP/src/api/api_lib.c\
	../../../Middlewares/Third_Party/LwIP/src/api/api_msg.c\
	../../../Middlewares/Third_Party/LwIP/src/api/err.c\
	../../../Middlewares/Third_Party/LwIP/src/api/netbuf.c\
	../../../Middlewares/Third_Party/LwIP/src/api/netdb.c\
	../../../Middlewares/Third_Party/LwIP/src/api/netifapi.c\
	../../../Middlewares/Third_Party/LwIP/src/api/sockets.c\
	../../../Middlewares/Third_Party/LwIP/src/api/tcpip.c\
	../../../Middlewares/Third_Party/LwIP/src/core/def.c\
	../../../Middlewares/Third_Party/LwIP/src/core/dns.c\
	../../../Middlewares/Third_Party/LwIP/src/core/inet_chksum.c\
	../../../Middlewares/Third_Party/LwIP/src/core/init.c\
	../../../Middlewares/Third_Party/LwIP/src/core/ip.c\
	../../../Middlewares/Third_Party/LwIP/src/core/mem.c\
	../../../Middlewares/Third_Party/LwIP/src/core/memp.c\
	../../../Middlewares/Third_Party/LwIP/src/core/netif.c\
	../../../Middlewares/Third_Party/LwIP/src/core/pbuf.c\
	../../../Middlewares/Third_Party/LwIP/src/core/raw.c\
	../../../Middlewares/Third_Party/LwIP/src/core/stats.c\
	../../../Middlewares/Third_Party/LwIP/src/core/sys.c\
	../../../Middlewares/Third_Party/LwIP/src/core/tcp.c\
	../../../Middlewares/Third_Party/LwIP/src/core/tcp_in.c\
	../../../Middlewares/Third_Party/LwIP/src/core/tcp_out.c\
	../../../Middlewares/Third_Party/LwIP/src/core/timeouts.c\
	../../../Middlewares/Third_Party/LwIP/src/core/udp.c\
	../../../Middlewares/Third_Party/LwIP/src/netif/ethernet.c\
	../../../Middlewares/Third_Party/LwIP/src/core/ipv4/autoip.c\
	../../../Middlewares/Third_Party/LwIP/src/core/ipv4/dhcp.c\
	../../../Middlewares/Third_Party/LwIP/src/core/ipv4/etharp.c\
	../../../Middlewares/Third_Party/LwIP/src/core/ipv4/icmp.c\
	../../../Middlewares/Third_Party/LwIP/src/core/ipv4/igmp.c\
	../../../Middlewares/Third_Party/LwIP/src/core/ipv4/ip4.c\
	../../../Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_addr.c\
	../../../Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_frag.c

C_SOURCES = ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/ipv4/icmp.c ${APP_ROOT}Src/tcp_echoserver.c ${APP_ROOT}Src/stm32f4xx_it.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/ipv4/igmp.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/netif/etharp.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/mem.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/api/api_msg.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/stats.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma2d.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/dns.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/memp.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_eth.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/api/sockets.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/tcp_out.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/def.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/api/api_lib.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/sys.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/ipv4/dhcp.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/ipv4/ip4.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/ipv4/ip_frag.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/api/netbuf.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/raw.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/lwip_timers.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/tcp.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/ipv4/autoip.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/api/netifapi.c ${APP_ROOT}Src/ethernetif.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/pbuf.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/api/tcpip.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/udp.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sdram.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c ${STM32_ROOT}Drivers/BSP/STM32469I_EVAL/stm32469i_eval_io.c ${APP_ROOT}Src/app_ethernet.c ${APP_ROOT}Src/system_stm32f4xx.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/inet_chksum.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fmc.c ${STM32_ROOT}Drivers/BSP/Components/mfxstm32l152/mfxstm32l152.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_addr.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/api/netdb.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_ltdc.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/netif.c ${STM32_ROOT}Drivers/BSP/STM32469I_EVAL/stm32469i_eval.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/tcp_in.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/api/err.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/api/pppapi.c ${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c ${STM32_ROOT}Middlewares/Third_Party/LwIP/src/core/init.c ${APP_ROOT}Src/main.c

# ASM sources
ASM_SOURCES = \
../startup_stm32f469xx.s


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
C_DEFS = \
-DUSE_HAL_DRIVER -DSTM32F469xx -DUSE_STM32469I_EVAL -DUSE_IOEXPANDER

# C includes
# C_INCLUDES = \
	-I ../../Inc\
	-I ../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include\
	-I ../../../Drivers/CMSIS/Include\
	-I ../../../Drivers/STM32F4xx_HAL_Driver/Inc\
	-I ../../../Drivers/BSP/STM32469I_EVAL\
	-I ../../../Drivers/BSP/Components\
	-I ../../../Middlewares/Third_Party/LwIP/src/include\
	-I ../../../Middlewares/Third_Party/LwIP/system\
	-I ../../../Utilities\
	-I ../../../Utilities/Log

C_INCLUDES = -I${APP_ROOT}Inc -I${STM32_ROOT}Drivers/CMSIS/Device/ST/STM32F4xx/Include -I${STM32_ROOT}Drivers/CMSIS/Include -I${STM32_ROOT}Drivers/STM32F4xx_HAL_Driver/Inc -I${STM32_ROOT}Drivers/BSP/STM32469I_EVAL -I${STM32_ROOT}Drivers/BSP/Components -I${STM32_ROOT}Middlewares/Third_Party/LwIP/src/include/ -I${STM32_ROOT}Middlewares/Third_Party/LwIP/system

C_INCLUDES += -I $(GCC_ROOT)/arm-none-eabi/include
# compile gcc flags
ASFLAGS = $(MCU)  $(C_INCLUDES) $(OPT) -Wall -target arm-none-eabi 

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -target arm-none-eabi -fshort-enums \
--sysroot=$(ARM_NONE_EABI_PATH)arm-none-eabi -ffreestanding -fno-builtin -fno-exceptions -Wl,--cref
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2 
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F469NIHx_FLASH
LLVMGOLD=  $(LLVM_ROOT)build/lib/LLVMgold.so
# libraries
LIBS = -lc -lm -lgcc -lnosys 
#-lnosys 
LIBDIR = \
-L $(GCC_ROOT)arm-none-eabi/lib/thumb/v7e-m \
-L $(GCC_ROOT)lib/gcc/arm-none-eabi/6.3.1/thumb/v7e-m  \
-I $(GCC_ROOT)arm-none-eabi/include
LDFLAGS = -Map $(BUILD4_DIR)/output.map $(LIBDIR) $(LIBS) \
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