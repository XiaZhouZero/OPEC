#!/bin/sh

set e
set x

mkdir -p ./policies/OPEC

# Camera_To_USBDisk
echo "[+] Camera_To_USBDisk: OPEC"
mkdir -p ./policies/OPEC/Camera_To_USBDisk
cp ${OI_DIR}stm32469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build9/Camera_To_USBDisk_operation_subfunc.json ./policies/OPEC/Camera_To_USBDisk
cp ${OI_DIR}stm32469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build9/Camera_To_USBDisk_final_policy.json ./policies/OPEC/Camera_To_USBDisk
cp ${OI_DIR}stm32469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build9/all_gv.json ./policies/OPEC/Camera_To_USBDisk
cp ${OI_DIR}stm32469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build9/ro_gv.json ./policies/OPEC/Camera_To_USBDisk
cp ${OI_DIR}stm32469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build9/memory_pool.json ./policies/OPEC/Camera_To_USBDisk


echo "[+] CoreMark: OPEC"
mkdir -p ./policies/OPEC/CoreMark
cp ${OI_DIR}stm32469I/CoreMark/SW4STM32/STM32469I_EVAL/build9/CoreMark_operation_subfunc.json ./policies/OPEC/CoreMark
cp ${OI_DIR}stm32469I/CoreMark/SW4STM32/STM32469I_EVAL/build9/CoreMark_final_policy.json ./policies/OPEC/CoreMark
cp ${OI_DIR}stm32469I/CoreMark/SW4STM32/STM32469I_EVAL/build9/all_gv.json ./policies/OPEC/CoreMark
cp ${OI_DIR}stm32469I/CoreMark/SW4STM32/STM32469I_EVAL/build9/ro_gv.json ./policies/OPEC/CoreMark
cp ${OI_DIR}stm32469I/CoreMark/SW4STM32/STM32469I_EVAL/build9/memory_pool.json ./policies/OPEC/CoreMark


# PinLock
echo "[+] PinLock: OPEC"
mkdir -p ./policies/OPEC/PinLock
cp ${OI_DIR}stm32f407/PinLock/Decode/SW4STM32/STM32F4-DISCO/build9/PinLock_operation_subfunc.json ./policies/OPEC/PinLock
cp ${OI_DIR}stm32f407/PinLock/Decode/SW4STM32/STM32F4-DISCO/build9/PinLock_final_policy.json ./policies/OPEC/PinLock
cp ${OI_DIR}stm32f407/PinLock/Decode/SW4STM32/STM32F4-DISCO/build9/all_gv.json ./policies/OPEC/PinLock
cp ${OI_DIR}stm32f407/PinLock/Decode/SW4STM32/STM32F4-DISCO/build9/ro_gv.json ./policies/OPEC/PinLock
cp ${OI_DIR}stm32f407/PinLock/Decode/SW4STM32/STM32F4-DISCO/build9/memory_pool.json ./policies/OPEC/PinLock


# FatFs_uSD
echo "[+] FatFs_uSD: OPEC"
mkdir -p ./policies/OPEC/FatFs_uSD
cp ${OI_DIR}stm32469I/FatFs_uSD/SW4STM32/STM32469I_EVAL/build9/FatFs_uSD_operation_subfunc.json ./policies/OPEC/FatFs_uSD
cp ${OI_DIR}stm32469I/FatFs_uSD/SW4STM32/STM32469I_EVAL/build9/FatFs_uSD_final_policy.json ./policies/OPEC/FatFs_uSD
cp ${OI_DIR}stm32469I/FatFs_uSD/SW4STM32/STM32469I_EVAL/build9/all_gv.json ./policies/OPEC/FatFs_uSD
cp ${OI_DIR}stm32469I/FatFs_uSD/SW4STM32/STM32469I_EVAL/build9/ro_gv.json ./policies/OPEC/FatFs_uSD
cp ${OI_DIR}stm32469I/FatFs_uSD/SW4STM32/STM32469I_EVAL/build9/memory_pool.json ./policies/OPEC/FatFs_uSD


# LCD_AnimatedPictureFromSDCard
echo "[+] LCD_AnimatedPictureFromSDCard: OPEC"
mkdir -p ./policies/OPEC/LCD_AnimatedPictureFromSDCard
cp ${OI_DIR}stm32469I/LCD_AnimatedPictureFromSDCard/SW4STM32/STM32469I_EVAL/build9/LCD_AnimatedPictureFromSDCard_operation_subfunc.json ./policies/OPEC/LCD_AnimatedPictureFromSDCard
cp ${OI_DIR}stm32469I/LCD_AnimatedPictureFromSDCard/SW4STM32/STM32469I_EVAL/build9/LCD_AnimatedPictureFromSDCard_final_policy.json ./policies/OPEC/LCD_AnimatedPictureFromSDCard
cp ${OI_DIR}stm32469I/LCD_AnimatedPictureFromSDCard/SW4STM32/STM32469I_EVAL/build9/all_gv.json ./policies/OPEC/LCD_AnimatedPictureFromSDCard
cp ${OI_DIR}stm32469I/LCD_AnimatedPictureFromSDCard/SW4STM32/STM32469I_EVAL/build9/ro_gv.json ./policies/OPEC/LCD_AnimatedPictureFromSDCard
cp ${OI_DIR}stm32469I/LCD_AnimatedPictureFromSDCard/SW4STM32/STM32469I_EVAL/build9/memory_pool.json ./policies/OPEC/LCD_AnimatedPictureFromSDCard


# LCD_PicturesFromSDCard
echo "[+] LCD_PicturesFromSDCard: OPEC"
mkdir -p ./policies/OPEC/LCD_PicturesFromSDCard
cp ${OI_DIR}stm32469I/LCD_PicturesFromSDCard/SW4STM32/STM32469I_EVAL/build9/LCD_PicturesFromSDCard_operation_subfunc.json ./policies/OPEC/LCD_PicturesFromSDCard
cp ${OI_DIR}stm32469I/LCD_PicturesFromSDCard/SW4STM32/STM32469I_EVAL/build9/LCD_PicturesFromSDCard_final_policy.json ./policies/OPEC/LCD_PicturesFromSDCard
cp ${OI_DIR}stm32469I/LCD_PicturesFromSDCard/SW4STM32/STM32469I_EVAL/build9/all_gv.json ./policies/OPEC/LCD_PicturesFromSDCard
cp ${OI_DIR}stm32469I/LCD_PicturesFromSDCard/SW4STM32/STM32469I_EVAL/build9/ro_gv.json ./policies/OPEC/LCD_PicturesFromSDCard
cp ${OI_DIR}stm32469I/LCD_PicturesFromSDCard/SW4STM32/STM32469I_EVAL/build9/memory_pool.json ./policies/OPEC/LCD_PicturesFromSDCard


# LwIP_TCP_Echo_Server
echo "[+] LwIP_TCP_Echo_Server: OPEC"
mkdir -p ./policies/OPEC/LwIP_TCP_Echo_Server
cp ${OI_DIR}stm32469I/LwIP_TCP_Echo_Server/SW4STM32/STM32469I_EVAL/build9/LwIP_TCP_Echo_Server_operation_subfunc.json ./policies/OPEC/LwIP_TCP_Echo_Server
cp ${OI_DIR}stm32469I/LwIP_TCP_Echo_Server/SW4STM32/STM32469I_EVAL/build9/LwIP_TCP_Echo_Server_final_policy.json ./policies/OPEC/LwIP_TCP_Echo_Server
cp ${OI_DIR}stm32469I/LwIP_TCP_Echo_Server/SW4STM32/STM32469I_EVAL/build9/all_gv.json ./policies/OPEC/LwIP_TCP_Echo_Server
cp ${OI_DIR}stm32469I/LwIP_TCP_Echo_Server/SW4STM32/STM32469I_EVAL/build9/ro_gv.json ./policies/OPEC/LwIP_TCP_Echo_Server
cp ${OI_DIR}stm32469I/LwIP_TCP_Echo_Server/SW4STM32/STM32469I_EVAL/build9/memory_pool.json ./policies/OPEC/LwIP_TCP_Echo_Server