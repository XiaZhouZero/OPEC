#!/usr/bin/env python
# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
from matplotlib.pyplot import MultipleLocator
import matplotlib as mpl
import numpy as np
import json
import argparse

apps = ["PinLock", "Animation", "FatFs-uSD", "LCD-uSD", "TCP-Echo"]
# ACES1: filename
# ACES2: filename-no-opt
# ACES3: peripheral



def load_json_from_file(filename):
    with open(filename, "r") as f:
        json_to_load = json.load(f)
    return json_to_load


def get_results(cur_dir):
    results_summary = {}
    for app in apps:
        results_file = cur_dir + "/" + app + "_results.json"
        temp = load_json_from_file(results_file)
        results_summary[app] = temp
    return results_summary



# results_summary = {
#     "PinLock": {
#         "ACES1": {
#             "Init_Lock": 0.29,
#             "Key_Init": 0.69,
#             "Lock_Task": 0.28,
#             "System_Init": 0.9,
#             "Unlock_Task": 0.01,
#             "Uart_Init": 0.75
#         },
#         "ACES2": {
#             "Init_Lock": 0.28,
#             "Key_Init": 0.7,
#             "Lock_Task": 0.27,
#             "System_Init": 0.91,
#             "Unlock_Task": 0.0,
#             "Uart_Init": 0.73
#         },
#         "ACES3": {
#             "Init_Lock": 0.31,
#             "Key_Init": 0.7,
#             "Lock_Task": 0.29,
#             "System_Init": 0.91,
#             "Unlock_Task": 0.03,
#             "Uart_Init": 0.73
#         },
#         "OPEC": {
#             "Init_Lock": 0.0,
#             "Key_Init": 0.0,
#             "Lock_Task": 0.0,
#             "System_Init": 0.0,
#             "Unlock_Task": 0.01,
#             "Uart_Init": 0.19
#         }
#     },
#     "Animation": {
#         "ACES1": {
#             "LCD_Config": 0.39,
#             "Turn_On_Display": 0.88,
#             "Init_Directory_File_Pointers": 0.23,
#             "Display_Foreground": 0.19,
#             "Display_Background": 0.3,
#             "Infinite_Loop": 0.23,
#             "Link_Driver": 0.99,
#             "System_Init": 1.0
#         },
#         "ACES2": {
#             "LCD_Config": 0.52,
#             "Turn_On_Display": 0.91,
#             "Init_Directory_File_Pointers": 0.17,
#             "Display_Foreground": 0.17,
#             "Display_Background": 0.26,
#             "Infinite_Loop": 0.17,
#             "Link_Driver": 0.99,
#             "System_Init": 0.99
#         },
#         "ACES3": {
#             "LCD_Config": 0.73,
#             "Turn_On_Display": 0.96,
#             "Init_Directory_File_Pointers": 0.21,
#             "Display_Foreground": 0.15,
#             "Display_Background": 0.2,
#             "Infinite_Loop": 0.16,
#             "Link_Driver": 0.98,
#             "System_Init": 1.0
#         },
#         "OPEC": {
#             "LCD_Config": 0.0,
#             "Turn_On_Display": 0.0,
#             "Init_Directory_File_Pointers": 0.11,
#             "Display_Foreground": 0.16,
#             "Display_Background": 0.18,
#             "Infinite_Loop": 0.23,
#             "Link_Driver": 0.88,
#             "System_Init": 0.91
#         }
#     },
#     "FatFs-uSD": {
#         "ACES1": {
#             "Compare_Data": 0.91,
#             "Link_Driver": 0.99,
#             "Read_File": 0.16,
#             "Unlink_Driver": 0.99,
#             "Write_to_File": 0.16,
#             "System_Init": 0.93,
#             "Open_File": 0.1,
#             "Create_FAT_FS": 0.17,
#             "Open_File_Read": 0.16,
#             "Register_FS": 0.69
#         },
#         "ACES2": {
#             "Compare_Data": 1.0,
#             "Link_Driver": 0.99,
#             "Read_File": 0.11,
#             "Unlink_Driver": 0.99,
#             "Write_to_File": 0.11,
#             "System_Init": 0.92,
#             "Open_File": 0.1,
#             "Create_FAT_FS": 0.12,
#             "Open_File_Read": 0.1,
#             "Register_FS": 0.66
#         },
#         "ACES3": {
#             "Compare_Data": 1.0,
#             "Link_Driver": 0.99,
#             "Read_File": 0.16,
#             "Unlink_Driver": 0.99,
#             "Write_to_File": 0.16,
#             "System_Init": 0.93,
#             "Open_File": 0.09,
#             "Create_FAT_FS": 0.17,
#             "Open_File_Read": 0.15,
#             "Register_FS": 0.69
#         },
#         "OPEC": {
#             "Compare_Data": 0.0,
#             "Link_Driver": 0.0,
#             "Read_File": 0.0,
#             "Unlink_Driver": 0.0,
#             "Write_to_File": 0.0,
#             "System_Init": 0.01,
#             "Open_File": 0.09,
#             "Create_FAT_FS": 0.15,
#             "Open_File_Read": 0.15,
#             "Register_FS": 0.74
#         }
#     },
#     "LCD-uSD": {
#         "ACES1": {
#             "BSP_LED_Init_0": 1.0,
#             "HAL_Init": 1.0,
#             "LCD_Config": 0.73,
#             "Link_Driver": 0.99,
#             "SystemClock_Config": 0.99,
#             "Configure_Transparency": 0.92,
#             "Get_BMP_Filenames": 0.18,
#             "Display_Background": 0.23,
#             "Display_Forground": 0.23,
#             "Format_String": 0.14,
#             "Init_Directory_File_Pointers": 0.33
#         },
#         "ACES2": {
#             "BSP_LED_Init_0": 1.0,
#             "HAL_Init": 0.5,
#             "LCD_Config": 0.37,
#             "Link_Driver": 0.95,
#             "SystemClock_Config": 0.98,
#             "Configure_Transparency": 0.72,
#             "Get_BMP_Filenames": 0.01,
#             "Display_Background": 0.18,
#             "Display_Forground": 0.17,
#             "Format_String": 0.12,
#             "Init_Directory_File_Pointers": 0.12
#         },
#         "ACES3": {
#             "BSP_LED_Init_0": 1.0,
#             "HAL_Init": 0.2,
#             "LCD_Config": 0.74,
#             "Link_Driver": 0.0,
#             "SystemClock_Config": 1.0,
#             "Configure_Transparency": 0.9,
#             "Get_BMP_Filenames": 0.1,
#             "Display_Background": 0.1,
#             "Display_Forground": 0.1,
#             "Format_String": 0.19,
#             "Init_Directory_File_Pointers": 0.84
#         },
#         "OPEC": {
#             "BSP_LED_Init_0": 0.0,
#             "HAL_Init": 0.0,
#             "LCD_Config": 0.0,
#             "Link_Driver": 0.0,
#             "SystemClock_Config": 0.0,
#             "Configure_Transparency": 0.02,
#             "Get_BMP_Filenames": 0.07,
#             "Display_Background": 0.18,
#             "Display_Forground": 0.18,
#             "Format_String": 0.25,
#             "Init_Directory_File_Pointers": 0.39
#         }
#     },
#     "TCP-Echo": {
#         "ACES1": {
#             "BSP_Config": 0.99,
#             "HAL_Init": 1.0,
#             "SystemClock_Config": 1.0,
#             "User_notification_0": 1.0,
#             "lwip_init": 0.4,
#             "Netif_Config": 0.43,
#             "ethernetif_input_0": 0.57,
#             "sys_check_timeouts": 0.57,
#             "tcp_echoserver_init": 0.08
#         },
#         "ACES2": {
#             "BSP_Config": 0.28,
#             "HAL_Init": 0.5,
#             "SystemClock_Config": 0.88,
#             "User_notification_0": 0.6,
#             "lwip_init": 0.39,
#             "Netif_Config": 0.44,
#             "ethernetif_input_0": 0.57,
#             "sys_check_timeouts": 0.57,
#             "tcp_echoserver_init": 0.01
#         },
#         "ACES3": {
#             "BSP_Config": 0.99,
#             "HAL_Init": 0.92,
#             "SystemClock_Config": 1.0,
#             "User_notification_0": 1.0,
#             "lwip_init": 0.04,
#             "Netif_Config": 0.6,
#             "ethernetif_input_0": 0.53,
#             "sys_check_timeouts": 0.53,
#             "tcp_echoserver_init": 0.54
#         },
#         "OPEC": {
#             "BSP_Config": 0.0,
#             "HAL_Init": 0.0,
#             "SystemClock_Config": 0.0,
#             "User_notification_0": 0.0,
#             "lwip_init": 0.0,
#             "Netif_Config": 0.42,
#             "ethernetif_input_0": 0.57,
#             "sys_check_timeouts": 0.57,
#             "tcp_echoserver_init": 0.58
#         }
#     }
# }


def main():
    plt.style.use('seaborn-poster')

    mpl.rcParams["font.sans-serif"] = ["Times New Roman"]
    mpl.rcParams["axes.unicode_minus"] = False
    mpl.rcParams["font.size"] = 22
    mpl.rcParams["font.weight"] = "bold"
    mpl.rcParams['figure.figsize'] = (32, 4)

    plt.xlim(0, 1)
    plt.ylim(0, 1)
    n_bins = 5


    # parse results
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--cur_dir', dest='CUR_DIR', default="", help='The path of current figure directory')
    args = parser.parse_args()
    dir_name = args.CUR_DIR
    results_summary = get_results(dir_name)

    # fig = plt.figure()
    fig, axs = plt.subplots(1, len(results_summary.keys()))
    # fig, axs = plt.subplots(2, 3)

    # fig, ax = plt.subplots(figsize=(8, 4))

    # plot the cumulative histogram
    # n, bins, patches = ax.hist(x, n_bins, density=True, histtype='step', cumulative=True, label='Empirical')

    index = "abcdefg"
    markers = "ov^XD"
    app_id = 0
    for app_name, app_info in results_summary.items():
        policy_id = 0
        for policy_name, execution_overprivilege_ratio in app_info.items():
            operation_entries = list(execution_overprivilege_ratio.keys())
            values = list(execution_overprivilege_ratio.values())
            # plot the figure
            keys = list(range(1, len(operation_entries)+1, 1))

            axs[app_id].plot(keys, values, linewidth=3, marker='o')
            # set axis interval
            x_major_locator=MultipleLocator(1)
            y_major_locator=MultipleLocator(0.1)
            axs[app_id].xaxis.set_major_locator(x_major_locator)
            axs[app_id].yaxis.set_major_locator(y_major_locator)
            # set grind
            # axs[app_id].grid(True)
            if app_id == 0:
                axs[app_id].set_ylabel("ET", fontsize=20, fontweight="bold")
            if app_id == 1:
                axs[app_id].legend(loc='upper center', labels=["ACES1", "ACES2", "ACES3", "OPEC"], prop={'size': 18})
            axs[app_id].set_xlabel("Task Number", fontsize=20, fontweight="bold")
            axs[app_id].set_title("("+index[app_id]+") " + app_name, fontsize=20, fontweight="bold")

            # policy_id += 1

        app_id += 1

    plt.savefig("Figure11_Execution_Overprivilege.pdf", dpi=600, format="pdf", bbox_inches='tight')
    # plt.show()

if __name__ == "__main__":
    main()