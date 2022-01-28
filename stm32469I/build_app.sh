#!/usr/bin/env bash
# build apps that runs on STM32F479I-EVAL board
set -e
set -x

app_list=("
	Camera_To_USBDisk
	FatFs_uSD
	FatFs_RAMDisk
	LCD_AnimatedPictureFromSDCard
	LCD_PicturesFromSDCard
	LCD_FileNamesFromSDCard
	LwIP_TCP_Echo_Server
	CoreMark
")

start_time_s=`date +%s`

if [ $OI_DIR ];then
    echo "OI_DIR = $OI_DIR";
else
    echo "Please define environment variable OI_DIR first"
    exit 1
fi

function build_app {
    echo "[*] Build $1 ..."
    cd $1/SW4STM32/STM32469I_EVAL;
    cp Makefile9.mk Makefile; make -j$(nproc);
    cp Makefile4.mk Makefile; make -j$(nproc);
    cd -
    end_time_s=`date +%s`
    sum_time=$[$end_time_s - $start_time_s]
    echo -e "\n[+] Finish building $1!\n"
    echo -e "\n[+] Total: $sum_time seconds"
}


function build_all {
    for app in $app_list
    do
        build_app $app
    done
}


timer_start=`date "+%Y-%m-%d %H:%M:%S"`


if [ $1 != null ]; then
    if [[ "${app_list[@]}"  =~ "${1}" ]]; then
        build_app $1

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