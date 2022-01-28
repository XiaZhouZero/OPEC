#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import logging
import traceback
import json

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

FunctionPointerTargets = "Function Pointer Targets"
NoTargets = "!!!has no targets!!!"

class Target(object):
    """
    Target class of indirect function call
    TOOD: add location info
    """
    def __init__(self, target_name):
        self.name = target_name
    
    def get_name(self):
        return self.name


class FunctionPointer(object):
    """
    FunctionPointer class of indirect callsite
    """
    def __init__(self):
        self.callsite = None
        self.location = None
        self.caller = None
        self.targets = None

    def set_callsite(self, callsite):
        self.callsite = callsite

    def get_callsite(self):
        return self.callsite

    def set_location(self, location):
        self.location = location

    def get_location(self):
        return self.location

    def set_caller(self, caller_name):
        self.caller = caller_name

    def get_caller(self):
        return self.caller

    def get_targets(self):
        return self.targets

    def add_target(self, target_funcname):
        if self.targets == None:
            self.targets = set()
        _target = Target(target_funcname)
        self.targets.add(_target)


def parse_svf_ander(svf_ander, output):
    """
    parse the results wpa ander point-to analysis and save it to the output
    """
    flag = False
    target_flag = False
    current_funcptr = None
    indicall_json = {}
    """TEMPLATE OF indicall_json
    {
        "caller": [
            {
                "callsite": hello,
                "location": location,
                "targets": [
                    {
                        "target_function": target_func
                    }
                ]
            }
        ]
    }
    """
    f_ander = open(svf_ander, "r")
    
    for line in f_ander.readlines():
        """EXAMPLE
        NodeID: 36779
        Caller:disk_initialize
        CallSite:   %call = call zeroext i8 %8(i8 zeroext %11) #4, !dbg !4832	Location: { ln: 60  cl: 12  fl: ../../../Middlewares/Third_Party/FatFs/src/diskio.c }	 with Targets: 
            USBH_initialize

        NodeID: 7536
        CallSite:   call void %52(%struct.__DMA2D_HandleTypeDef.49* %53) #4, !dbg !4983	Location: { ln: 1567  cl: 9  fl: ../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma2d.c }
            !!!has no targets!!!
        """
        if FunctionPointerTargets in line:
            flag = not flag

        if flag==False:
            continue

        if not line.strip():
            # empty line means the end of the current indirect callsite targets
            # first, save old current_funcptr information to json, exclude the initial state
            if current_funcptr == None:
                current_funcptr = FunctionPointer()
                continue
            else:
                caller = current_funcptr.get_caller()
                if caller not in indicall_json.keys():
                    indicall_json[caller] = []
                callsite_info = {}
                callsite_info["callsite"] = current_funcptr.get_callsite()
                callsite_info["location"] = current_funcptr.get_location()
                targets = current_funcptr.get_targets()
                callsite_info["targets"] = None
                if targets:
                    callsite_info["targets"] = []
                    for ele in targets:
                        target_name = ele.get_name()
                        # print(type(ele), type(ele.get_name()), type(target_name), target_name)
                        callsite_info["targets"].append(ele.get_name())
                indicall_json[caller].append(callsite_info)
            # sencond, create a new FunctionPointer object
            current_funcptr = FunctionPointer()
            # finally, set target_flag to False
            target_flag = False

        if line.startswith("Caller"):
            # set caller
            caller = line.split(":")[1].strip()
            current_funcptr.set_caller(caller)
        
        if line.startswith("CallSite"):
            # set callsite and location
            callsite, location = line.split("Location:")
            callsite = callsite.split("CallSite:")[1].strip()
            location = location.split("{")[1].split("}")[0].strip()
            current_funcptr.set_callsite(callsite)
            current_funcptr.set_location(location)
            # this line indicates the start of targets output
            target_flag = True
            continue

        if target_flag:
            if NoTargets in line:
                target_flag = False
            else:
                target = line.strip()
                current_funcptr.add_target(target)


    f_ander.close()
    with open(output, "w") as f_output:
        # print(indicall_json)
        json.dumps(indicall_json, indent=4, sort_keys=True)
        # write json results to files
        json.dump(indicall_json, f_output, sort_keys=True, indent=4)
    return indicall_json


def deduplicate_targets(ander_json):
    """
    deduplicate callsite targets, add pruned_num(for address_taken pruned targets of a callsite), ty_num(for type_based pruned targets of a calliste), and num(total targets of a callsite)
    """
    for caller in ander_json.keys():
        callsites = ander_json[caller]
        callsites_num = len(callsites)
        for i in range(callsites_num):
            callees = callsites[i]["targets"]
            if not callees:
                continue
            callees_set = set(callees)
            callsites[i]["targets"] = list(callees_set)
            callsites[i]["at_num"] = 0
            callsites[i]["ty_num"] = 0
            callsites[i]["num"] = len(callees_set)
    return ander_json


def address_taken_pruning(ander_json, AT_json):
    """
    finding out spurious indirect call edges by identifying un-address taken callee functions
    @ander_json: SVF andersen point-to analysis results
    @AT_json: functions that are address taken
    """
    address_taken_diff = {}
    for caller in ander_json.keys():
        # print("----" + caller)
        callsites = ander_json[caller]
        callsites_num = len(callsites)
        for i in range(callsites_num):
            callees = callsites[i]["targets"]
            if not callees:
                continue
            callees_set = ander_json[caller][i]["targets"]
            for callee in callees_set:
                if callee in AT_json:
                    # print("--------" + callee)
                    continue
                else:
                    # print("--------" + callee + "----pruned")
                    ander_json[caller][i]["targets"].remove(callee)
                    ander_json[caller][i]["at_num"] += 1
                    if caller not in address_taken_diff.keys():
                        address_taken_diff[caller] = []
                    address_taken_diff[caller].append(callee)
                    
    return ander_json, address_taken_diff


def type_pruning(ander_json, type_json):
    """
    finding out spurious indirect call edges by identifying unsatisfied type callee functions
    @ander_json: SVF andersen point-to analysis results
    @AT_json: functions that are unsatisfied typed
    """
    type_based_diff = {}
    for caller in ander_json.keys():
        # print("----" + caller)
        callsites = ander_json[caller]
        callsites_num = len(callsites)
        for i in range(callsites_num):
            callees = callsites[i]["targets"]
            location = callsites[i]["location"]            
            if not callees:
                continue
            callees_set = ander_json[caller][i]["targets"]
            type_callee_list = type_json[caller]
            for type_callee in type_callee_list:
                # first traverse the type_based analysis results
                if location == type_callee["location"]:
                    type_callee_targets = type_callee["targets"]
                    for target in callees_set:
                        if type_callee_targets == None or target in type_callee_targets:
                            # print("--------" + target)
                            continue
                        else:
                            ander_json[caller][i]["targets"].remove(target)
                            ander_json[caller][i]["ty_num"] += 1
                            if caller not in type_based_diff.keys():
                                type_based_diff[caller] = []
                            type_based_diff[caller].append(target)
                            # print("--------" + target + "----pruned")

    return ander_json, type_based_diff


if __name__ == '__main__':
    logger.info("[*] Start to parsing the SVF Andersen pointer analysis results")
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--infile', dest='ander_file', default="/home/zhouxia/OI_project/stm32f469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build9/ander_funcptr.txt", help='Andersen pointer analysis results')
    parser.add_argument('-o', '--outfile', dest='fptr_json', default="/home/zhouxia/OI_project/stm32f469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build9/ander_funcptr.json", help="")
    parser.add_argument('-a', '--address-taken', dest='address_taken_json', default="/home/zhouxia/OI_project/stm32f469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build9/address_taken_functions.json")
    parser.add_argument('-t', '--type-based-funcptr', dest="type_based_funcptr_json", default="/home/zhouxia/OI_project/stm32f469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build9/type_based_analysis_results.json")
    args = parser.parse_args()

    logger.info("[*] Start to parse the SVF Andersen pointer analysis results")
    ander_json = parse_svf_ander(svf_ander=args.ander_file, output=args.fptr_json)
    logger.info("[*] Finish parsing the SVF Andersen pointer analysis results")

    logger.info("[*] Start to pruning wpa function pointer analysis results via address-taken pruning")
    f_at = open(args.address_taken_json, "r")
    at_json = json.load(f_at)
    ander_json, address_taken_diff = address_taken_pruning(ander_json, at_json)
    f_at.close()
    # address_taken_diff is used for pruning callgraph_format.dot via address_taken
    logger.info("[*] Finish pruning wpa function pointer analysis results via address-taken pruning")
    
    logger.info("[*] Start to pruning wpa function pointer analysis results via type pruning")
    # print(ander_json)
    f_type = open(args.type_based_funcptr_json, "r")
    type_json = json.load(f_type)
    ander_json, type_based_diff = type_pruning(ander_json, type_json)
    f_type.close()
    # type_based_diff is used for pruning callgraph_format.dot via function type
    logger.info("[*] Finish pruning wpa function pointer analysis results via type pruning")