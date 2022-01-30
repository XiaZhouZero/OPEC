#!/bin/sh

mkdir -p ./raw_results

# PinLock
mkdir -p ./raw_results/PinLock
cp ${OI_DIR}stm32f407/PinLock/Decode/SW4STM32/STM32F4-DISCO/build9/ander_funcptr.json ./raw_results/PinLock
cp ${OI_DIR}stm32f407/PinLock/Decode/SW4STM32/STM32F4-DISCO/build9/type_based_analysis_results.json ./raw_results/PinLock
cp ${OI_DIR}stm32f407/PinLock/Decode/SW4STM32/STM32F4-DISCO/build9/PinLock.pts ./raw_results/PinLock

# Camera_To_USBDisk
mkdir -p ./raw_results/Camera_To_USBDisk
cp ${OI_DIR}stm32469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build9/ander_funcptr.json ./raw_results/Camera_To_USBDisk
cp ${OI_DIR}stm32469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build9/type_based_analysis_results.json ./raw_results/Camera_To_USBDisk
cp ${OI_DIR}stm32469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build9/Camera_To_USBDisk.pts ./raw_results/Camera_To_USBDisk

# FatFs_uSD
mkdir -p ./raw_results/FatFs_uSD
cp ${OI_DIR}stm32469I/FatFs_uSD/SW4STM32/STM32469I_EVAL/build9/ander_funcptr.json ./raw_results/FatFs_uSD
cp ${OI_DIR}stm32469I/FatFs_uSD/SW4STM32/STM32469I_EVAL/build9/type_based_analysis_results.json ./raw_results/FatFs_uSD
cp ${OI_DIR}stm32469I/FatFs_uSD/SW4STM32/STM32469I_EVAL/build9/FatFs_uSD.pts ./raw_results/FatFs_uSD

# LCD_AnimatedPictureFromSDCard
mkdir -p ./raw_results/LCD_AnimatedPictureFromSDCard
cp ${OI_DIR}stm32469I/LCD_AnimatedPictureFromSDCard/SW4STM32/STM32469I_EVAL/build9/ander_funcptr.json ./raw_results/LCD_AnimatedPictureFromSDCard
cp ${OI_DIR}stm32469I/LCD_AnimatedPictureFromSDCard/SW4STM32/STM32469I_EVAL/build9/type_based_analysis_results.json ./raw_results/LCD_AnimatedPictureFromSDCard
cp ${OI_DIR}stm32469I/LCD_AnimatedPictureFromSDCard/SW4STM32/STM32469I_EVAL/build9/LCD_AnimatedPictureFromSDCard.pts ./raw_results/LCD_AnimatedPictureFromSDCard

# LCD_PicturesFromSDCard
mkdir -p ./raw_results/LCD_PicturesFromSDCard
cp ${OI_DIR}stm32469I/LCD_PicturesFromSDCard/SW4STM32/STM32469I_EVAL/build9/ander_funcptr.json  ./raw_results/LCD_PicturesFromSDCard
cp ${OI_DIR}stm32469I/LCD_PicturesFromSDCard/SW4STM32/STM32469I_EVAL/build9/type_based_analysis_results.json ./raw_results/LCD_PicturesFromSDCard
cp ${OI_DIR}stm32469I/LCD_PicturesFromSDCard/SW4STM32/STM32469I_EVAL/build9/LCD_PicturesFromSDCard.pts ./raw_results/LCD_PicturesFromSDCard

# LwIP_TCP_Echo_Server
mkdir -p ./raw_results/LwIP_TCP_Echo_Server
cp ${OI_DIR}stm32469I/LwIP_TCP_Echo_Server/SW4STM32/STM32469I_EVAL/build9/ander_funcptr.json  ./raw_results/LwIP_TCP_Echo_Server
cp ${OI_DIR}stm32469I/LwIP_TCP_Echo_Server/SW4STM32/STM32469I_EVAL/build9/type_based_analysis_results.json ./raw_results/LwIP_TCP_Echo_Server
cp ${OI_DIR}stm32469I/LwIP_TCP_Echo_Server/SW4STM32/STM32469I_EVAL/build9/LwIP_TCP_Echo_Server.pts ./raw_results/LwIP_TCP_Echo_Server

# CoreMark
mkdir -p ./raw_results/CoreMark
cp ${OI_DIR}stm32469I/CoreMark/SW4STM32/STM32469I_EVAL/build9/ander_funcptr.json  ./raw_results/CoreMark
cp ${OI_DIR}stm32469I/CoreMark/SW4STM32/STM32469I_EVAL/build9/type_based_analysis_results.json ./raw_results/CoreMark
cp ${OI_DIR}stm32469I/CoreMark/SW4STM32/STM32469I_EVAL/build9/CoreMark.pts ./raw_results/CoreMark