#!/bin/sh

echo ${OI_DIR}

mkdir -p ./maps

# PinLock
cp ${OI_DIR}stm32f407/PinLock/Decode/SW4STM32/STM32F4-DISCO/build9/PinLock.map ./maps

# Camera_To_USBDisk
cp ${OI_DIR}stm32469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build9/Camera_To_USBDisk.map ./maps

# FatFs_uSD
cp ${OI_DIR}stm32469I/FatFs_uSD/SW4STM32/STM32469I_EVAL/build9/FatFs_uSD.map ./maps

# LCD_AnimatedPictureFromSDCard
cp ${OI_DIR}stm32469I/LCD_AnimatedPictureFromSDCard/SW4STM32/STM32469I_EVAL/build9/LCD_AnimatedPictureFromSDCard.map ./maps

# LCD_PicturesFromSDCard
cp ${OI_DIR}stm32469I/LCD_PicturesFromSDCard/SW4STM32/STM32469I_EVAL/build9/LCD_PicturesFromSDCard.map ./maps

# LwIP_TCP_Echo_Server
cp ${OI_DIR}stm32469I/LwIP_TCP_Echo_Server/SW4STM32/STM32469I_EVAL/build9/LwIP_TCP_Echo_Server.map ./maps