#!/bin/sh

echo ${OI_DIR}

mkdir -p ./bins/baseline
mkdir -p ./bins/opec

# PinLock
cp ${OI_DIR}stm32f407/PinLock/Decode/SW4STM32/STM32F4-DISCO/build4/PinLock--baseline.elf ./bins/baseline
cp ${OI_DIR}stm32f407/PinLock/Decode/SW4STM32/STM32F4-DISCO/build4/PinLock--oi--final.elf ./bins/opec

# Camera_To_USBDisk
cp ${OI_DIR}stm32f469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build4/Camera_To_USBDisk--baseline.elf ./bins/baseline
cp ${OI_DIR}stm32f469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build4/Camera_To_USBDisk--oi--final.elf ./bins/opec

# FatFs_uSD
cp ${OI_DIR}stm32f469I/FatFs_uSD/SW4STM32/STM32469I_EVAL/build4/FatFs_uSD--baseline.elf ./bins/baseline
cp ${OI_DIR}stm32f469I/FatFs_uSD/SW4STM32/STM32469I_EVAL/build4/FatFs_uSD--oi--final.elf ./bins/opec

# LCD_AnimatedPictureFromSDCard
cp ${OI_DIR}stm32f469I/LCD_AnimatedPictureFromSDCard/SW4STM32/STM32469I_EVAL/build4/LCD_AnimatedPictureFromSDCard--baseline.elf ./bins/baseline
cp ${OI_DIR}stm32f469I/LCD_AnimatedPictureFromSDCard/SW4STM32/STM32469I_EVAL/build4/LCD_AnimatedPictureFromSDCard--oi--final.elf ./bins/opec

# LCD_PicturesFromSDCard
cp ${OI_DIR}stm32f469I/LCD_PicturesFromSDCard/SW4STM32/STM32469I_EVAL/build4/LCD_PicturesFromSDCard--baseline.elf ./bins/baseline
cp ${OI_DIR}stm32f469I/LCD_PicturesFromSDCard/SW4STM32/STM32469I_EVAL/build4/LCD_PicturesFromSDCard--oi--final.elf ./bins/opec

# LwIP_TCP_Echo_Server
cp ${OI_DIR}stm32f469I/LwIP_TCP_Echo_Server/SW4STM32/STM32469I_EVAL/build4/LwIP_TCP_Echo_Server--baseline.elf ./bins/baseline
cp ${OI_DIR}stm32f469I/LwIP_TCP_Echo_Server/SW4STM32/STM32469I_EVAL/build4/LwIP_TCP_Echo_Server--oi--final.elf ./bins/opec

# CoreMark
cp ${OI_DIR}stm32f469I/CoreMark/SW4STM32/STM32469I_EVAL/build4/CoreMark--baseline.elf ./bins/baseline
cp ${OI_DIR}stm32f469I/CoreMark/SW4STM32/STM32469I_EVAL/build4/CoreMark--oi--final.elf ./bins/opec