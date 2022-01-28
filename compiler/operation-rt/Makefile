
BUILD_DIR = ./operation-rt-lib
LLVM_PATH = ../../llvm4/build/bin
INCLUDE_PATH = -I ../../gcc/bins/arm-none-eabi/include/ \
-I ./include
CLANG = $(LLVM_PATH)/clang
OPT_LEVEL = -O0


CFLAGS = -flto -fno-exceptions $(INCLUDE_PATH) -fshort-enums
CFLAGS += -fno-builtin -ffreestanding -fmessage-length=0 -ffunction-sections -g
# CFLAGS += -gdwarf-2 -fdata-sections -DTIMER_OPEC
CFLAGS += -gdwarf-2 -fdata-sections


#all: syscalls hexbox-enforce hexbox-record
all: operation-rt

operation-rt: $(BUILD_DIR) $(BUILD_DIR)/operation-rt-v6-m.o \
			  $(BUILD_DIR)/operation-rt-v7-m.o \
			  $(BUILD_DIR)/operation-rt-v7e-m.o
	@echo Done Building operation-rt

$(BUILD_DIR):
	mkdir -p $@

# v6-m
$(BUILD_DIR)/operation-rt-v6-m.o: $(BUILD_DIR)/operation-rt-v6-m.bc $(BUILD_DIR)/thumb-v6-m.bc $(BUILD_DIR)/thumb2-v6-m.bc
	$(LLVM_PATH)/llvm-link $^ -o $(@)

$(BUILD_DIR)/operation-rt-v6-m.bc: operation-rt.c
	$(CLANG) $(OPT_LEVEL)  -c -target arm-none-eabi -mcpu=cortex-m0   \
	$(CFLAGS) $< -o $@

$(BUILD_DIR)/thumb2-v6-m.bc: thumb2.c
	$(CLANG) $(OPT_LEVEL)  -c -target arm-none-eabi -mcpu=cortex-m0   \
	$(CFLAGS) $< -o $@

$(BUILD_DIR)/thumb-v6-m.bc: thumb.c
	$(CLANG) $(OPT_LEVEL)  -c -target arm-none-eabi -mcpu=cortex-m0   \
	$(CFLAGS) $< -o $@

# v7-m
$(BUILD_DIR)/operation-rt-v7-m.o: $(BUILD_DIR)/operation-rt-v7-m.bc $(BUILD_DIR)/thumb-v7-m.bc $(BUILD_DIR)/thumb2-v7-m.bc
	$(LLVM_PATH)/llvm-link $^ -o $(@)

$(BUILD_DIR)/operation-rt-v7-m.bc: operation-rt.c
	$(CLANG) $(OPT_LEVEL)  -c -target arm-none-eabi -mcpu=cortex-m3  \
	$(CFLAGS) $< -o $@

$(BUILD_DIR)/thumb2-v7-m.bc: thumb2.c
	$(CLANG) $(OPT_LEVEL)  -c -target arm-none-eabi -mcpu=cortex-m3  \
	$(CFLAGS) $< -o $@

$(BUILD_DIR)/thumb-v7-m.bc: thumb.c
	$(CLANG) $(OPT_LEVEL)  -c -target arm-none-eabi -mcpu=cortex-m3  \
	$(CFLAGS) $< -o $@

# v7e-m
$(BUILD_DIR)/operation-rt-v7e-m.o: $(BUILD_DIR)/operation-rt-v7e-m.bc $(BUILD_DIR)/thumb-v7e-m.bc $(BUILD_DIR)/thumb2-v7e-m.bc
	$(LLVM_PATH)/llvm-link $^ -o $(@)

$(BUILD_DIR)/operation-rt-v7e-m.bc: operation-rt.c
	$(CLANG) $(OPT_LEVEL)  -c -target arm-none-eabi -mcpu=cortex-m4   \
	$(CFLAGS) $< -o $@

$(BUILD_DIR)/thumb2-v7e-m.bc: thumb2.c
	$(CLANG) $(OPT_LEVEL)  -c -target arm-none-eabi -mcpu=cortex-m4   \
	$(CFLAGS) $< -o $@

$(BUILD_DIR)/thumb-v7e-m.bc: thumb.c
	$(CLANG) $(OPT_LEVEL)  -c -target arm-none-eabi -mcpu=cortex-m4   \
	$(CFLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f $(BUILD_DIR)/*.bc
	rm -f $(BUILD_DIR)/*.o
