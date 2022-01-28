#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os
import json
import logging
import argparse
import operator

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

apps = ["PinLock", "FatFs_uSD", "LCD_AnimatedPictureFromSDCard", "LCD_PicturesFromSDCard", "LwIP_TCP_Echo_Server"]

app_name_mappings = {
    "PinLock": "PinLock", 
    "LCD_AnimatedPictureFromSDCard": "Animation", 
    "FatFs_uSD": "FatFs-uSD", 
    "LCD_PicturesFromSDCard": "LCD-uSD", 
    "LwIP_TCP_Echo_Server": "TCP-Echo", 
    "Camera_To_USBDisk": "Camera", 
    "CoreMark": "CoreMark"
}

policies = ["filename", "filename-no-opt", "peripheral"]
policy_name_mappings = {
    "filename": "ACES1",
    "filename-no-opt": "ACES2",
    "peripheral": "ACES3"
}

odd_funcs = ["_init", "_fstat", "_close", "_isatty", "_write", "_lseek", "_getpid", "_kill", "_read", "_exit", "HAL_SD_XferErrorCallback"]

inline_funcs = ["__RBIT"]

def store_json_to_file(json_to_dump, filename):
    with open(filename, "w") as f:
        json.dump(json_to_dump, f, indent=4)


def load_json_from_file(filename):
    with open(filename, "r") as f:
        json_to_load = json.load(f)
    return json_to_load


def extract_function_size_info(filename):
    map_info = {}
    with open(filename, "r") as f_map:
        for line in f_map:
            symbol_info = line.split(" ")
            
            if len(symbol_info) != 4:
                continue

            symbol_addr = symbol_info[0].strip()
            symbol_size = symbol_info[1].strip()
            symbol_name = symbol_info[3].strip()
            map_info[symbol_name] = {}
            map_info[symbol_name]["addr"] = symbol_addr
            map_info[symbol_name]["size"] = symbol_size
    return map_info


def recover_trace(file_name, entry_funcs):
    trace = {}
    current_operation = None
    with open(file_name, "r", encoding="utf-8") as f:
        for line in f.readlines():
            if "(" not in line and ")" not in line:
                continue
            function_name = line.split("\t")[0].strip()
            try:
                a = int(function_name)

            except ValueError:
                function_name = function_name.split(" ")[0]
                # logger.info("Execute function: {}".format(function_name))
                if function_name == "main":
                    continue

                if function_name in entry_funcs:
                    current_operation = function_name
                    if function_name not in trace.keys():
                        trace[function_name] = []
                else:
                    trace[current_operation].append(function_name)
    return trace


def extract_opec_not_executed_funcs(operation_subfuncs, app_trace):
    opec_not_executed_funcs = {}
    for operation, subfuncs in operation_subfuncs.items():
        executed_funcs = app_trace[operation]
        # find out operation subfuncs that do not appear in trace
        opec_not_executed_funcs[operation] = {}
        opec_not_executed_funcs[operation]["Should_not_funcs"] = []
        executed_funcs = set(list(executed_funcs))
        executed_funcs.add(operation)
        opec_not_executed_funcs[operation]["Should_funcs"] = list(executed_funcs)
        opec_not_executed_funcs[operation]["Should_funcs"].sort()
        
        for func in subfuncs:
            if func not in executed_funcs:
                opec_not_executed_funcs[operation]["Should_not_funcs"].append(func)
        opec_not_executed_funcs[operation]["Should_not_funcs"] = list(set(opec_not_executed_funcs[operation]["Should_not_funcs"] ))
        if opec_not_executed_funcs[operation]["Should_not_funcs"]:
            opec_not_executed_funcs[operation]["Should_not_funcs"].sort()

    return opec_not_executed_funcs


def extract_aces_not_executed_funcs(aces_policy, app_trace):
    aces_not_executed_funcs = {}
    for operation, executed_funcs in app_trace.items():
        aces_not_executed_funcs[operation] = {}
        aces_not_executed_funcs[operation]["Should_not_funcs"] = []
        executed_funcs = set(list(executed_funcs))
        executed_funcs.add(operation)
        aces_not_executed_funcs[operation]["Should_funcs"] = list(executed_funcs)
        aces_not_executed_funcs[operation]["Should_funcs"].sort()
        for func in executed_funcs:

            for region_name, region_content in aces_policy["Regions"].items():
                # find code compartment
                if not region_name.startswith(".CODE_REGION"):
                    continue

                # find the compartment
                if func not in region_content["Objects"]:
                    continue

                # check the Should_not_funcs within one code compartment
                for other_func in region_content["Objects"]:
                    if other_func == "main":
                        continue

                    if other_func not in executed_funcs:
                        aces_not_executed_funcs[operation]["Should_not_funcs"].append(other_func)
        aces_not_executed_funcs[operation]["Should_not_funcs"] = list(set(aces_not_executed_funcs[operation]["Should_not_funcs"]))
        if aces_not_executed_funcs[operation]["Should_not_funcs"]:
            aces_not_executed_funcs[operation]["Should_not_funcs"].sort()

    return aces_not_executed_funcs


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


def add_global_data_info(operation_executed_func_info, func_dep, all_gv):
    for operation in operation_executed_func_info.keys():
        # find global data accessed by excuted Should_funcs
        global_data = []
        for func in operation_executed_func_info[operation]["Should_funcs"]:
            if func in odd_funcs or func in inline_funcs:
                continue
            try:
                global_data.extend(func_dep[func]["Variables"])
            except KeyError:
                # logger.error("Unknown function: {}".format(func))
                pass
        global_data = list(set(global_data))
        operation_executed_func_info[operation]["Should_vars"] = global_data
        # calc global variables size
        global_data_size = calc_global_data_size(global_data, all_gv)
        operation_executed_func_info[operation]["Should_size"] = global_data_size 

        # find global data accessed by unexcuted Should_not_funcs
        global_data = []
        for func in operation_executed_func_info[operation]["Should_not_funcs"]:
            if func in odd_funcs or func in inline_funcs:
                continue
            try:
                global_data.extend(func_dep[func]["Variables"])
            except KeyError:
                # logger.error("Unknown function: {}".format(func))
                pass
        global_data = list(set(global_data))

        prun_global_data = []
        for var in global_data:
            if var in operation_executed_func_info[operation]["Should_vars"]:
                continue
            else:
                prun_global_data.append(var)
        operation_executed_func_info[operation]["Should_not_vars"] = prun_global_data
        # calc global variables size
        global_data_size = calc_global_data_size(prun_global_data, all_gv)
        operation_executed_func_info[operation]["Should_not_size"] = global_data_size  

    return operation_executed_func_info



def opec_execution_time_overprivilege_check(opec_policy_file, operation_subfuncs, all_gv_file, ro_gv_file, func_dep_file, app_map_info, app_trace):
    """
    find out the not executed functions based on the trace
    {
        "operation": {
            "Should_not_funcs": [],
            "Should_not_vars": [],
            "Should_not_size": None,
            "Should_funcs": [],
            "Should_vars": []
            "Should_size": None
        }
    }
    """
    all_gv      = load_json_from_file(all_gv_file)
    ro_gv       = load_json_from_file(ro_gv_file)
    func_dep    = load_json_from_file(func_dep_file)
    opec_policy = load_json_from_file(opec_policy_file)

    opec_not_executed_funcs = extract_opec_not_executed_funcs(operation_subfuncs, app_trace)
    opec_not_executed_funcs = add_global_data_info(opec_not_executed_funcs, func_dep, all_gv)

    return opec_not_executed_funcs


def aces_execution_time_overprivilege_check(aces_policy_file, all_gv_file, ro_gv_file, func_dep_file, app_map_info, app_trace):
    """
    find out the not executed functions based on the trace
    {
        "operation": {
            "Should_not_funcs": [],
            "Should_not_vars": [],
            "Should_not_size": None,
            "Should_funcs": [],
            "Should_vars": []
            "Should_size": None
        }
    }
    """
    aces_policy = load_json_from_file(aces_policy_file)
    all_gv      = load_json_from_file(all_gv_file)
    ro_gv       = load_json_from_file(ro_gv_file)
    func_dep    = load_json_from_file(func_dep_file)

    aces_not_executed_funcs = extract_aces_not_executed_funcs(aces_policy, app_trace)
    aces_not_executed_funcs = add_global_data_info(aces_not_executed_funcs, func_dep, all_gv)

    return aces_not_executed_funcs


def sort_results(raw_results):
    sorted_OPEC = sorted(raw_results["OPEC"].items(), key=operator.itemgetter(1))
    sorted_keys = []
    for i in sorted_OPEC:
        sorted_keys.append(i[0])

    sorted_results = {}
    for policy, results in raw_results.items():
        sorted_results[policy] = {}
        for key in sorted_keys:
            sorted_results[policy][key] = raw_results[policy][key]

    return sorted_results


def main():
    logger.info("Execution time overprivilege")
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--cur_dir', dest='CUR_DIR', default="", help='The root path of the project')
    args = parser.parse_args()
    dir_name = args.CUR_DIR
    logger.info(dir_name)

    for app in apps:
        # parse binary map information
        app_map_info = extract_function_size_info(dir_name + "/maps/" + app + ".map")
        store_json_to_file(app_map_info, dir_name + "/maps/" + app + "_map.json")

        # parse application execution trace recored by gdb
        operation_subfuncs_file = dir_name + "/policies/OPEC/" + app + "/" + app + "_operation_subfunc.json"
        operation_subfuncs = load_json_from_file(operation_subfuncs_file)
        app_trace = recover_trace(dir_name + "/logs/" + app + "_final.log", operation_subfuncs.keys())
        store_json_to_file(app_trace, dir_name + "/traces/" + app + ".json")

        opec_policy_file    = dir_name + "/policies/OPEC/" + app + "/" + app + "_final_policy.json"
        all_gv_file         = dir_name + "/policies/OPEC/" + app + "/all_gv.json"
        ro_gv_file          = dir_name + "/policies/OPEC/" + app + "/ro_gv.json"
        func_dep_file       = dir_name + "/policies/OPEC/" + app + "/func_memdep_gv_peri_ptr_bf_operation.json"
        application_map_file = dir_name + "/maps/" + app + "_map.json"

        # calculate Execution Time Overprivilege
        opec_not_executed_funcs = opec_execution_time_overprivilege_check(opec_policy_file, operation_subfuncs, all_gv_file, ro_gv_file, func_dep_file, app_map_info, app_trace)

        ########## collect results for plotting ##########
        results_for_plot = {}
        results_for_plot["OPEC"] = {}
        for operation, et_info in opec_not_executed_funcs.items():
            if et_info["Should_size"] + et_info["Should_not_size"] == 0:
                results_for_plot["OPEC"][operation] = 0.0
            else:
                results_for_plot["OPEC"][operation] = round(et_info["Should_not_size"] * 1.0 /(et_info["Should_size"] + et_info["Should_not_size"]), 2)
        ########## collect results for plotting ##########

        store_json_to_file(opec_not_executed_funcs, dir_name + "/" + app + "_opec" + "_execution_time_over_privilege.json")

        # calc ET for app-policy
        for policy in policies:          
            logger.info("APP: {}, POLICY: {}".format(app, policy))
            aces_policy_file    = dir_name + "/policies/ACES/" + app + "/hexbox-final-policy--" + policy + "--mpu-8.json"

            # calculate Execution Time Overprivilege
            aces_not_executed_funcs = aces_execution_time_overprivilege_check(aces_policy_file, all_gv_file, ro_gv_file, func_dep_file, app_map_info, app_trace)

            ########## collect results for plotting ##########
            results_for_plot[policy_name_mappings[policy]] = {}
            for operation, et_info in aces_not_executed_funcs.items():
                if et_info["Should_size"] + et_info["Should_not_size"] == 0:
                    results_for_plot[policy_name_mappings[policy]][operation] = 0.0
                else:
                    results_for_plot[policy_name_mappings[policy]][operation] = round(et_info["Should_not_size"] * 1.0 /(et_info["Should_size"] + et_info["Should_not_size"]), 2)
            
            results_for_plot = sort_results(results_for_plot)
            
            logger.info(dir_name + "/" + app_name_mappings[app] + "_results.json")
            store_json_to_file(results_for_plot, dir_name + "/" + app_name_mappings[app] + "_results.json")
            ########## collect results for plotting ##########

            store_json_to_file(aces_not_executed_funcs, dir_name + "/" + app_name_mappings[app] + "_" + policy_name_mappings[policy] + "_execution_time_over_privilege.json")
            del aces_not_executed_funcs

if __name__ == '__main__':
    main()