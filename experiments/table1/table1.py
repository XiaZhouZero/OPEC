#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os
import json
import logging
import argparse
import operator
import numpy as np
from prettytable import PrettyTable

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

apps = [
    "PinLock",                          # PinLock
    "FatFs_uSD",                        # FatFs-uSD
    "LCD_AnimatedPictureFromSDCard",    # Animation
    "LCD_PicturesFromSDCard",           # LCD-uSD
    "LwIP_TCP_Echo_Server",             # TCP-Echo
    "Camera_To_USBDisk",                # Camera
    "CoreMark"                          # CoreMark
]

aces_apps = ["PinLock", "FatFs_uSD", "LCD_AnimatedPictureFromSDCard", "LCD_PicturesFromSDCard", "LwIP_TCP_Echo_Server"]
# aces_apps = ["PinLock"]

policies = ["filename", "filename-no-opt", "peripheral"]
# policies = ["filename"]

# Use Ghidra to checkout the code address range
original_code_size = {
    "PinLock": (0x08011C40-0x080001C0) + (0x0801928C-0x08013D10),
    "FatFs_uSD": (0x0801B6FE-0x080001B4) + (0x0801DCD4-0x0801D840),
    "LCD_AnimatedPictureFromSDCard": (0x08023798-0x080001C0) + (0x0802A58C-0x080258A0),
    "LCD_PicturesFromSDCard": (0x0802823C-0x080001C0) + (0x0802EFDE-0x0802A2E0),
    "LwIP_TCP_Echo_Server": (0x080248B0-0x080001C0) + (0x08026E68-0x08026AB0),
    "Camera_To_USBDisk": (0x080321C6-0x080001C0) + (0x08038FC8-0x080342EC),
    "CoreMark": (0x08010F6A-0x080001B8) - (0x08013E88-0x08013040)
}


# start: is_SCB_reg
# end: main
# How to calculate this value? Use Ghidra to calculate the size of OPEC-Monitor
privileged_code_size = {
    "PinLock": 0x08013CD8-0x08011C40,
    "FatFs_uSD": 0x0801D7AC-0x0801B700,
    "LCD_AnimatedPictureFromSDCard": 0x08025868-0x0802379A,
    "LCD_PicturesFromSDCard": 0x0802A2AC-0x08028202,
    "LwIP_TCP_Echo_Server": 0x08026A78-0x080248B2,
    "Camera_To_USBDisk": 0x080342B4-0x080321C8,
    "CoreMark": 0x0801300E-0x08010F30
}

def store_json_to_file(json_to_dump, filename):
    with open(filename, "w") as f:
        json.dump(json_to_dump, f, indent=4)


def load_json_from_file(filename):
    with open(filename, "r") as f:
        json_to_load = json.load(f)
    return json_to_load


def opec_calc_avg_gv_size(app, sub_funcs_file, policy_file, all_gv_file, ro_gv_file, memory_pool_file):
    results = {}
    all_gv = load_json_from_file(all_gv_file)
    ro_gv = load_json_from_file(ro_gv_file)
    rw_gv_sum = 0
    rw_gv_num = 0
    max_gv_size = 0

    operation_subfuncs = load_json_from_file(sub_funcs_file)
    memory_pool_info = load_json_from_file(memory_pool_file)

    # operation number
    operation_num = len(operation_subfuncs.keys())
    results["ope_num"] = operation_num
    # average functions of an operation
    subs_sum = 0
    for ope_name, subfuncs in operation_subfuncs.items():
        subs_sum += len(subfuncs)
    results["avg_fun_num"] = round(subs_sum*1.0/operation_num, 2)
    # privileged code size
    results["priv_code_size"] = privileged_code_size[app]
    results["priv_code_ratio"] = round(privileged_code_size[app]*100.0/original_code_size[app], 2)

    # average size of the accessible global vars
    policy = load_json_from_file(policy_file)
    # memory_pools = policy["MemoryPool"].keys()
    # DMA_Buffers = ["ram_heap", "DMARxDscrTab", "DMATxDscrTab", "Rx_Buff", "Tx_Buff"]
    for gv_name, gv_info in all_gv.items():
        # if gv_name.startswith("memp_tab"):
        #     tmp = gv_name.split("memp_tab_")[1].strip()
        #     if tmp in memory_pools:
        #         continue

        # elif gv_name.startswith("memp_memory"):
        #     tmp = gv_name.split("memp_memory_")[1].strip()
        #     if tmp.endswith("_base"):
        #         tmp = tmp.split("_base")[0]
        #         if tmp in memory_pools:
        #             continue

        # if (gv_name not in DMA_Buffers) and (gv_name not in ro_gv):
        #     # print(gv_name)
        #     rw_gv_sum += gv_info["size"]
        #     rw_gv_num += 1
        
        if gv_name not in ro_gv:
            rw_gv_sum += gv_info["size"]
            rw_gv_num += 1

    opec_gv_sum = []
    opec_gv_num_sum = []
    for operation_name, policy_info in policy["Operation"].items():
        operation_gv_size = 0
        operation_gv_num = 0
        for gv_name in policy_info["ExternalData"]:
            operation_gv_size += all_gv[gv_name]["size"]
            operation_gv_num += 1

        for gv_name in policy_info["InternalData"]:
            operation_gv_size += all_gv[gv_name]["size"]
            operation_gv_num += 1

        for memory_pool_name in list(set(policy_info["MemoryPoolNames"])):
            for var, size in memory_pool_info[memory_pool_name].items():
                operation_gv_size += size
                operation_gv_num += 1

        # get the max_gv_size of each operation
        if max_gv_size < operation_gv_size:
            max_gv_size = operation_gv_size

        opec_gv_sum.append(operation_gv_size)
        opec_gv_num_sum.append(operation_gv_num)

    med_gv_size = np.median(opec_gv_sum)
    med_gv_ratio = round(np.median(opec_gv_sum)*100.0/rw_gv_sum, 2)
    avg_gv_size = round(sum(opec_gv_sum)*1.0/len(opec_gv_sum), 2)
    avg_gv_ratio = round(avg_gv_size*100.0/rw_gv_sum, 2)
    max_gv_ratio = round(max_gv_size*100.0/rw_gv_sum, 2)

    results["gv_num_sum"] = rw_gv_num
    results["avg_gv_num"] = round(sum(opec_gv_num_sum)*1.0/len(opec_gv_num_sum), 2)
    results["avg_gv_num_ratio"] = round(sum(opec_gv_num_sum)*100.0/len(opec_gv_num_sum)/rw_gv_num, 2)
    results["med_gv_size"] = med_gv_size
    results["med_gv_ratio"] = med_gv_ratio
    results["avg_gv_size"] = avg_gv_size
    results["avg_gv_ratio"] = avg_gv_ratio
    results['max_gv_size'] = max_gv_size
    results['max_gv_ratio'] = max_gv_ratio
    results["rw_gv_size"] = rw_gv_sum
    return results


def aces_calc_avg_gv_size(policy_file, all_gv_file, ro_gv_file):
    results = {}
    all_gv = load_json_from_file(all_gv_file)
    ro_gv = load_json_from_file(ro_gv_file)
    rw_gv = []
    rw_gv_sum = 0
    max_gv_size = 0
    for gv_name, gv_info in all_gv.items():
        if gv_name not in ro_gv:
            rw_gv_sum += gv_info["size"]
            rw_gv.append(gv_name)

    policy = load_json_from_file(policy_file)

    # compartments
    results["compartments_num"] = len(policy["Compartments"].keys())-1

    # funct
    aces_funct_sum = []
    for region in policy["Regions"].keys():
        if "CODE_REGION" in region:
            aces_funct_sum.append(len(policy["Regions"][region]["Objects"]))
    results["avg_functs"] = round(sum(aces_funct_sum)*1.0/len(aces_funct_sum), 2)

    # avg global vars
    aces_gv_sum = []
    for copmart_name, compart_info in policy["Compartments"].items():
        if "CODE_REGION" not in copmart_name:
            continue
        data_regions = compart_info["Data"]
        tmp = []
        copmart_vars = []
        copmart_vars_size = 0
        for data_region in data_regions:
            tmp.extend(policy["Regions"][data_region]["Objects"])
        for gv_candidate in tmp:
            if gv_candidate in rw_gv:
                copmart_vars.append(gv_candidate)
        for var_name in copmart_vars:
            copmart_vars_size += all_gv[var_name]["size"]
        aces_gv_sum.append(copmart_vars_size)


    results["gv_sum"] = aces_gv_sum

    med_gv_size = np.median(aces_gv_sum)
    med_gv_ratio = round(np.median(aces_gv_sum)*100.0/rw_gv_sum, 2)
    results["med_gv_size"] = med_gv_size
    results["med_gv_ratio"] = med_gv_ratio

    results["avg_gv_size"] = round(sum(aces_gv_sum)*1.0/len(aces_gv_sum), 2)
    results["avg_gv_ratio"] = round(sum(aces_gv_sum)*100.0/len(aces_gv_sum)/rw_gv_sum, 2)

    # priv_code
    return results


def calc_average_results(results):
    average = {}
    average["ope_num"]          = 0
    average["avg_fun_num"]      = 0
    average["priv_code_size"]   = 0
    average["priv_code_ratio"]  = 0
    average["gv_num_sum"]       = 0
    average["avg_gv_num"]       = 0
    average["avg_gv_num_ratio"] = 0
    average["med_gv_size"]      = 0
    average["med_gv_ratio"]     = 0
    average["avg_gv_size"]      = 0
    average["avg_gv_ratio"]     = 0
    average["max_gv_size"]      = 0
    average["max_gv_ratio"]     = 0
    average["rw_gv_size"]       = 0
    for app in apps:
        average["ope_num"]          += results[app]["ope_num"]
        average["avg_fun_num"]      += results[app]["avg_fun_num"]
        average["priv_code_size"]   += results[app]["priv_code_size"]
        average["priv_code_ratio"]  += results[app]["priv_code_ratio"]
        average["gv_num_sum"]       += results[app]["gv_num_sum"]
        average["avg_gv_num"]       += results[app]["avg_gv_num"]
        average["avg_gv_num_ratio"] += results[app]["avg_gv_num_ratio"]
        average["med_gv_size"]      += results[app]["med_gv_size"]
        average["med_gv_ratio"]     += results[app]["med_gv_ratio"]
        average["avg_gv_size"]      += results[app]["avg_gv_size"]
        average["avg_gv_ratio"]     += results[app]["avg_gv_ratio"]
        average["max_gv_size"]      += results[app]["max_gv_size"]
        average["max_gv_ratio"]     += results[app]["max_gv_ratio"]
        average["rw_gv_size"]       += results[app]["rw_gv_size"]
    
    for key, value in average.items():
        average[key] = round(value / len(apps) * 1.00, 2)
    
    results["Average"] = average
    return results

def print_table1(results):
    table1_head = ["Application", "#OPs", "Avg. Funcs", "#Pri. Code(%)",  "#Avg. GVars(%)"]
    app_name_mappings = {
        "PinLock": "PinLock", 
        "Animation": "LCD_AnimatedPictureFromSDCard", 
        "FatFs-uSD": "FatFs_uSD", 
        "LCD-uSD": "LCD_PicturesFromSDCard", 
        "TCP-Echo": "LwIP_TCP_Echo_Server", 
        "Camera": "Camera_To_USBDisk", 
        "CoreMark": "CoreMark"
    }

    table = PrettyTable(table1_head)
    for app_breif, app in app_name_mappings.items():
        priv_code_info = "{0}({1})".format(results[app]["priv_code_size"], results[app]["priv_code_ratio"])
        avg_info = "{0}({1})".format(results[app]["avg_gv_size"], results[app]["avg_gv_ratio"])
        table.add_row([app_breif, results[app]["ope_num"], results[app]["avg_fun_num"], priv_code_info, avg_info])

    print(table)


def main():
    logger.info("Basic metrics evaluation")
    dir_root = os.path.abspath(os.curdir)
    logger.info(dir_root)
    Results = {}
    ACES_Results = {}
    for app in apps:
        Results[app] = {}
        sub_funcs_file = dir_root + "/policies/OPEC/" + app + "/" + app + "_operation_subfunc.json"
        policy_file = dir_root + "/policies/OPEC/" + app + "/" + app + "_final_policy.json"
        all_gv_file = dir_root + "/policies/OPEC/" + app + "/" + "all_gv.json"
        ro_gv_file = dir_root + "/policies/OPEC/" + app + "/" + "ro_gv.json"
        memory_pool_file = dir_root + "/policies/OPEC/" + app + "/" + "memory_pool.json"
        Results[app] = opec_calc_avg_gv_size(app, sub_funcs_file, policy_file, all_gv_file, ro_gv_file, memory_pool_file)
    
    Results = calc_average_results(Results)


    # OPEC results
    result_file = dir_root + "/" + "OPEC_results.json"
    store_json_to_file(Results, result_file)


    for app in aces_apps:
        ACES_Results[app] = {}
        for policy in policies:
            aces_policy_file = dir_root + "/policies/ACES/" + app + "/" + "hexbox-final-policy--" + policy + "--mpu-8.json"
            all_gv_file = dir_root + "/policies/OPEC/" + app + "/" + "all_gv.json"
            ro_gv_file = dir_root + "/policies/OPEC/" + app + "/" + "ro_gv.json"
            ACES_Results[app][policy] = aces_calc_avg_gv_size(aces_policy_file, all_gv_file, ro_gv_file)
    aces_result_file = dir_root + "/" + "ACES_results.json"
    store_json_to_file(ACES_Results, aces_result_file)

    # Print OPEC paper table 1
    print_table1(Results)

if __name__ == '__main__':
    main()