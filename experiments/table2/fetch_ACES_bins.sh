#!/bin/sh

ACES_DIR=/home/xiazhou/Desktop/2019OI/ACES/test_apps/

mkdir -p ./bins/baseline
mkdir -p ./bins/aces

# PinLock
echo "[+] PinLock: Baseline"
scp lab_debug:${ACES_DIR}/pinlock/Decode/SW4STM32/STM32F4-DISCO/bin/PinLock--filename--mpu-8--baseline.elf ./bins/baseline/PinLock--baseline.elf
echo "[+] PinLock: ACES"
scp lab_debug:${ACES_DIR}/pinlock/Decode/SW4STM32/STM32F4-DISCO/bin/PinLock--filename--mpu-8--hexbox--final.elf ./bins/aces
scp lab_debug:${ACES_DIR}/pinlock/Decode/SW4STM32/STM32F4-DISCO/bin/PinLock--filename-no-opt--mpu-8--hexbox--final.elf ./bins/aces
scp lab_debug:${ACES_DIR}/pinlock/Decode/SW4STM32/STM32F4-DISCO/bin/PinLock--peripheral--mpu-8--hexbox--final.elf ./bins/aces


# FatFs_uSD
echo "[+] FatFs_uSD: Baseline"
scp lab_debug:${ACES_DIR}/STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/FatFs/FatFs_uSD/SW4STM32/STM32469I_EVAL/bin/FatFs-uSD--filename--mpu-8--baseline.elf ./bins/baseline/FatFs_uSD--baseline.elf
echo "[+] FatFs_uSD: ACES"
scp lab_debug:${ACES_DIR}/STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/FatFs/FatFs_uSD/SW4STM32/STM32469I_EVAL/bin/FatFs-uSD--filename--mpu-8--hexbox--final.elf ./bins/aces/FatFs_uSD--filename--mpu-8--hexbox--final.elf
scp lab_debug:${ACES_DIR}/STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/FatFs/FatFs_uSD/SW4STM32/STM32469I_EVAL/bin/FatFs-uSD--filename-no-opt--mpu-8--hexbox--final.elf ./bins/aces/FatFs_uSD--filename-no-opt--mpu-8--hexbox--final.elf
scp lab_debug:${ACES_DIR}/STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/FatFs/FatFs_uSD/SW4STM32/STM32469I_EVAL/bin/FatFs-uSD--peripheral--mpu-8--hexbox--final.elf ./bins/aces/FatFs_uSD--peripheral--mpu-8--hexbox--final.elf


# LCD_AnimatedPictureFromSDCard
echo "[+] LCD_AnimatedPictureFromSDCard: Baseline"
scp lab_debug:${ACES_DIR}/STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/Display/LCD_AnimatedPictureFromSDCard/SW4STM32/STM32469I_EVAL/bin/Animation--filename--mpu-8--baseline.elf ./bins/baseline/LCD_AnimatedPictureFromSDCard--baseline.elf
echo "[+] LCD_AnimatedPictureFromSDCard: ACES"
scp lab_debug:${ACES_DIR}/STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/Display/LCD_AnimatedPictureFromSDCard/SW4STM32/STM32469I_EVAL/bin/Animation--filename--mpu-8--hexbox--final.elf ./bins/aces/LCD_AnimatedPictureFromSDCard--filename--mpu-8--hexbox--final.elf
scp lab_debug:${ACES_DIR}/STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/Display/LCD_AnimatedPictureFromSDCard/SW4STM32/STM32469I_EVAL/bin/Animation--filename-no-opt--mpu-8--hexbox--final.elf ./bins/aces/LCD_AnimatedPictureFromSDCard--filename-no-opt--mpu-8--hexbox--final.elf
scp lab_debug:${ACES_DIR}/STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/Display/LCD_AnimatedPictureFromSDCard/SW4STM32/STM32469I_EVAL/bin/Animation--peripheral--mpu-8--hexbox--final.elf ./bins/aces/LCD_AnimatedPictureFromSDCard--peripheral--mpu-8--hexbox--final.elf


# LCD_PicturesFromSDCard
echo "[+] LCD_PicturesFromSDCard: Baseline"
scp lab_debug:${ACES_DIR}/STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/Display/LCD_PicturesFromSDCard/SW4STM32/STM32469I_EVAL/bin/LCD_uSD--filename--mpu-8--baseline.elf ./bins/baseline/LCD_PicturesFromSDCard--baseline.elf
echo "[+] LCD_PicturesFromSDCard: ACES"
scp lab_debug:${ACES_DIR}/STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/Display/LCD_PicturesFromSDCard/SW4STM32/STM32469I_EVAL/bin/LCD_uSD--filename--mpu-8--hexbox--final.elf ./bins/aces/LCD_PicturesFromSDCard--filename--mpu-8--hexbox--final.elf
scp lab_debug:${ACES_DIR}/STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/Display/LCD_PicturesFromSDCard/SW4STM32/STM32469I_EVAL/bin/LCD_uSD--filename-no-opt--mpu-8--hexbox--final.elf ./bins/aces/LCD_PicturesFromSDCard--filename-no-opt--mpu-8--hexbox--final.elf
scp lab_debug:${ACES_DIR}/STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/Display/LCD_PicturesFromSDCard/SW4STM32/STM32469I_EVAL/bin/LCD_uSD--peripheral--mpu-8--hexbox--final.elf ./bins/aces/LCD_PicturesFromSDCard--peripheral--mpu-8--hexbox--final.elf


# LwIP_TCP_Echo_Server
echo "[+] LwIP_TCP_Echo_Server: Baseline"
scp lab_debug:${ACES_DIR}/STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/LwIP/LwIP_TCP_Echo_Server/SW4STM32/STM32469I_EVAL/bin/TCP-Echo--filename--mpu-8--baseline.elf ./bins/baseline/LwIP_TCP_Echo_Server--baseline.elf
echo "[+] LwIP_TCP_Echo_Server: ACES"
scp lab_debug:${ACES_DIR}/STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/LwIP/LwIP_TCP_Echo_Server/SW4STM32/STM32469I_EVAL/bin/TCP-Echo--filename--mpu-8--hexbox--final.elf ./bins/aces/LwIP_TCP_Echo_Server--filename--mpu-8--hexbox--final.elf
scp lab_debug:${ACES_DIR}/STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/LwIP/LwIP_TCP_Echo_Server/SW4STM32/STM32469I_EVAL/bin/TCP-Echo--filename-no-opt--mpu-8--hexbox--final.elf ./bins/aces/LwIP_TCP_Echo_Server--filename-no-opt--mpu-8--hexbox--final.elf
scp lab_debug:${ACES_DIR}/STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/LwIP/LwIP_TCP_Echo_Server/SW4STM32/STM32469I_EVAL/bin/TCP-Echo--peripheral--mpu-8--hexbox--final.elf ./bins/aces/LwIP_TCP_Echo_Server--peripheral--mpu-8--hexbox--final.elf