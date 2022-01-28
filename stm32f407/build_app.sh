#!/bin/bash
# build apps that runs on STM32F479I-EVAL board
set -e
set -x

app_list=("
    PinLock
")

start_time_s=`date +%s`

if [ $OI_DIR ];then
    echo "OI_DIR = $OI_DIR";
else
    echo "Please define environment variable OI_DIR first"
    exit 1
fi

function build_PinLock {
    echo "[*] Build $1 ..."
    cd PinLock/Decode/SW4STM32/STM32F4-DISCO/;
    cp Makefile9.mk Makefile; make -j$(nproc);
    cp Makefile4.mk Makefile; make -j$(nproc);
    cd -;
	end_time_s=`date +%s`
	sum_time=$[$end_time_s - $start_time_s]
    echo -e "\n[+] Finish building $1!"
	echo -e "\n[+] Total: $sum_time seconds"
}


function build_all {
    build_PinLock "PinLock"
}


if [ $1 != null ]; then
    if [[ "PinLock"  =~ "${1}" ]]; then
        build_PinLock $1
    
    elif [[ "all"  =~ "${1}" ]]; then
        build_all

    elif [[ "help"  =~ "${1}" ]]; then
        echo -e "\nUsage: ./build_app.sh <app_name>\n"
        echo "[*] Please input the name of app: $app_list"
        echo "[*] Input 'all' to build all apps"

    elif [[ ! "${app_list[@]}"  =~ "${1}" ]]; then
        echo -e "\nUsage: ./build_app.sh <app_name>\n"
        echo "[*] Please input the name of app: $app_list"
        echo "[*] Input 'all' to build all apps"
    fi
else
    echo -e "\nUsage: ./build_app.sh <app_name>\n"
    echo "[*] Please input the name of app: $app_list"
    echo "[*] Input 'all' to build all apps"
fi
