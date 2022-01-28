#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import json
import logging
import argparse
import numpy as np

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

apps = ["PinLock", "FatFs_uSD", "LCD_AnimatedPictureFromSDCard", "LCD_PicturesFromSDCard", "LwIP_TCP_Echo_Server"]
policys = ["filename", "filename-no-opt", "peripheral"]

# apps = ["LCD_AnimatedPictureFromSDCard"]
# policys = ["filename-no-opt"]

odd_vars = ["_sbrk.heap_end"]
odd_funcs = ["_init", "_fstat", "_close", "_isatty", "_write", "_lseek", "_getpid", "_kill", "_read", "_exit", "HAL_SD_XferErrorCallback"]

compare_dict = {}
compare_results = {}


def store_json_to_file(json_to_dump, filename):
    with open(filename, "w") as f:
        json.dump(json_to_dump, f, sort_keys=True, indent=4)


def load_json_from_file(filename):
    with open(filename, "r") as f:
        json_to_load = json.load(f)
    return json_to_load


def calc_global_data_size(global_data_list, app_gv_info):
    """To sum the global varaible size of the global variables from the list"""
    sum = 0
    for gv in global_data_list:
        if gv in app_gv_info.keys():
            sum += app_gv_info[gv]["size"]
        # try:
        #     sum += app_gv_info[gv]["size"]
        # except KeyError:
        #     # print(gv)
        #     sum += 0
        #     # sys.exit()
        else:
            logger.warn("Unknown global variable: {}".format(gv))
    return sum


def parition_time_overprivilege_check(aces_policy_file, all_gv_file, ro_gv_file, func_dep_file):
    """comparts_info
    "app": {
        "policy": {
            "code_region": {
                "could": [],                    # accessible gv allowed by ACES
                "could_num": num,               # gv number
                "could_size": size,             # gv size(bytes)
                "should": [],                   # accessible gv allowed by OPEC
                "should_num": num,              # gv number
                "should_size": size,            # gv size(bytes)
                "over_privilege": [],           # over-privilege gv
                "over_privilege_num": num,      # over-privilege gv number
                "over_privilege_size": size     # over-privilege gv size(bytes)
            }
        }
    }
    """
    comparts_info = {}
    aces_policy = load_json_from_file(aces_policy_file)
    all_gv      = load_json_from_file(all_gv_file)
    ro_gv       = load_json_from_file(ro_gv_file)
    func_dep    = load_json_from_file(func_dep_file)

    # for could access gv
    for compart_name, compart_body in aces_policy["Compartments"].items():
        if ".CODE_REGION" not in compart_name:
            continue
        comparts_info[compart_name] = {}
        comparts_info[compart_name]["could"] = []
        temp_could_list = []
        comparts_info[compart_name]["could_num"] = 0
        comparts_info[compart_name]["could_size"] = 0

        if len(compart_body["Data"]) == 0:
            continue
        else:
            for data_region in compart_body["Data"]:
                global_data_list = aces_policy["Regions"][data_region]["Objects"]
                pruned_global_data_list = []
                for gv in global_data_list:
                    if gv in odd_vars:
                        continue
                    else:
                        pruned_global_data_list.append(gv)
                global_data_list = pruned_global_data_list
                temp_could_list.extend(global_data_list)
        temp_could_list = list(set(temp_could_list))
        # remove ro data
        ro_gv = list(set(ro_gv))
        for gv in temp_could_list:
            if gv in ro_gv:
                continue
            else:
                comparts_info[compart_name]["could"].append(gv)
        comparts_info[compart_name]["could"] = list(set(comparts_info[compart_name]["could"]))
        comparts_info[compart_name]["could_num"] = len(comparts_info[compart_name]["could"])
        comparts_info[compart_name]["could_size"] = calc_global_data_size(comparts_info[compart_name]["could"], all_gv)

    
    # for should access gv
    for region_name, region_info in aces_policy["Regions"].items():
        if ".CODE_REGION" not in region_name:
            continue
        compart_name = region_name
        comparts_info[compart_name]["should"] = []
        comparts_info[compart_name]["should_num"] = 0
        comparts_info[compart_name]["should_size"] = 0
        comparts_info[compart_name]["functions"] = region_info["Objects"]
        for member_func in region_info["Objects"]:
            # if member_func in odd_funcs:
            #     continue
            try:
                global_data_list = func_dep[member_func]["Variables"]
                comparts_info[compart_name]["should"].extend(global_data_list)
            except KeyError:
                continue
        comparts_info[compart_name]["should"] = list(set(comparts_info[compart_name]["should"]))
        comparts_info[compart_name]["should_num"] = len(comparts_info[compart_name]["should"])
        comparts_info[compart_name]["should_size"] = calc_global_data_size(comparts_info[compart_name]["should"], all_gv)


    # for over-privilege gv
    for compart_name in comparts_info.keys():
        comparts_info[compart_name]["over_privilege"] = []
        comparts_info[compart_name]["over_privilege_num"] = 0
        comparts_info[compart_name]["over_privilege_size"] = 0
        comparts_info[compart_name]["ratio"] = 0
        for gv in comparts_info[compart_name]["could"]:
            if gv not in comparts_info[compart_name]["should"]:
                comparts_info[compart_name]["over_privilege"].append(gv)
        comparts_info[compart_name]["over_privilege"] = list(set(comparts_info[compart_name]["over_privilege"]))
        comparts_info[compart_name]["over_privilege_num"] = len(comparts_info[compart_name]["over_privilege"])
        comparts_info[compart_name]["over_privilege_size"] = calc_global_data_size(comparts_info[compart_name]["over_privilege"], all_gv)
        if comparts_info[compart_name]["could_size"]:
            comparts_info[compart_name]["ratio"] = round(comparts_info[compart_name]["over_privilege_size"]*1.0/comparts_info[compart_name]["could_size"] , 2)

    return comparts_info


def list2str(old_list):
    new_list = []
    for i in old_list:
        new_list.append(str(i))
    return new_list


def data_polish(could_access, should_access):
    new_could_access = []
    for i in range(len(could_access)):
        if could_access[i] < should_access[i]:
            new_could_access.append(should_access[i])
        else:
            new_could_access.append(could_access[i])
    return new_could_access


def over_degree(could_access, should_access):
    result = []
    for i in range(len(could_access)):
        could = could_access[i]
        should = should_access[i]
        if could == 0:
            result.append(0)    # add 0 items. if do not consider domain accessing 0 items, just replace it with continue
        else:
            result.append((could - should) / could)
    return result, np.mean(result)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--cur_dir', dest='CUR_DIR', default="", help='The path of current figure directory')
    args = parser.parse_args()
    dir_name = args.CUR_DIR
    logger.info(dir_name)
    Results = {}       # partition time overprivilege ratio
    for app in apps:
        Results[app] = {}
        compare_results = {}
        for policy in policys:
            Results[app][policy] = []
            logger.info("APP: {}, POLICY: {}".format(app, policy))
            aces_policy_file    = dir_name + "/policies/ACES/" + app + "/hexbox-final-policy--" + policy + "--mpu-8.json"
            all_gv_file         = dir_name + "/policies/OPEC/" + app + "/all_gv.json"
            ro_gv_file          = dir_name + "/policies/OPEC/" + app + "/ro_gv.json"
            func_dep_file       = dir_name + "/policies/OPEC/" + app + "/func_memdep_gv_peri_ptr_bf_operation.json"
            comparts_info = parition_time_overprivilege_check(aces_policy_file, all_gv_file, ro_gv_file, func_dep_file)
            compare_results[policy] = comparts_info
            # collect the partition-time overprivilege ratio
            for compart_name, compart_info in comparts_info.items():
                Results[app][policy].append(compart_info["ratio"])
            Results[app][policy].sort()
            store_json_to_file(comparts_info, app+"_"+policy+"_"+"partition_time_over_privilege.json")
        store_json_to_file(Results, "Results.json")


if __name__ == "__main__":
    main()