#!/bin/bash
# clean apps that runs on STM32F479I-EVAL board
set -e
set -x

app_list=("
    PinLock
")

if [ $OI_DIR ];then
    echo "OI_DIR = $OI_DIR";
else
    echo "Please define environment variable OI_DIR first"
    exit 1
fi

function clean_PinLock {
    echo "[*] Clean $1 ..."
    cd PinLock/Decode/SW4STM32/STM32F4-DISCO/
    cp Makefile9.mk Makefile; make clean;
    cp Makefile4.mk Makefile; make clean;
	rm Makefile
    cd -;
    echo -e "\n[+] Finish cleaning $1!"
}


function clean_all {
    clean_PinLock "pinlcok"
}


if [ $1 != null ]; then
    if [[ "PinLock"  =~ "${1}" ]]; then
        clean_PinLock $1
    
    elif [[ "all"  =~ "${1}" ]]; then
        clean_all

    elif [[ "help"  =~ "${1}" ]]; then
        echo -e "\nUsage: ./clean_app.sh <app_name>\n"
        echo "[*] Please input the name of app: $app_list"
        echo "[*] Input 'all' to clean all apps"

    elif [[ ! "${app_list[@]}"  =~ "${1}" ]]; then
        echo -e "\nUsage: ./clean_app.sh <app_name>\n"
        echo "[*] Please input the name of app: $app_list"
        echo "[*] Input 'all' to clean all apps"
    fi
else
    echo -e "\nUsage: ./clean_app.sh <app_name>\n"
    echo "[*] Please input the name of app: $app_list"
    echo "[*] Input 'all' to clean all apps"
fi
