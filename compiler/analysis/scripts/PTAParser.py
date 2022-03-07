#!/usr/bin/env python3
# -*- coding: UTF-8 -*-


import argparse
import logging
import traceback
import re
import json
import sys
import os
import oi_utils

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

pointer_name_hint = "##<"
empty_target_hint = "{empty}"

name_regex1 = r'\([a-zA-Z\s]*[0-9a-zA-Z\_\.]*\)'
name_regex2 = r'\([a-zA-Z\s]*[0-9a-zA-Z\_\.]*\)'

global_var_list = []



# def extract_virtual_variable_name(line):
#     """
#     ##<  %2 = alloca i32*, align 8><>Source Loc:
#     ##<%struct.tag* %2><>Source Loc: 2nd arg swap 
#     """
#     try:
#         #pointer_name = line.split("=")[0].strip().split("<")[1].strip()
#         express = line.split("><")[0].split("<")[1].strip()
#         if "=" in express:
#             pointer_name = express.split("=")[0].strip()
#         else:
#             pointer_name = re.findall(r"%\d$", express)
#             assert len(pointer_name) == 1
#             pointer_name = pointer_name[0].strip()
#             #print(pointer_name)
#         return pointer_name

#     except IndexError:
#         logger.error("IndexError")
#         traceback.print_exc()
#         return None
    
#     except AssertionError:
#         logger.error("AssertionError")
#         traceback.print_exc()
#         #print(line)
#         sys.exit()


def fill_value_name(pta_file, global_var_list, parse_result_filename):
    global_var_mapping = {}
    """
    global_var_mapping: 
    {
        "funcname": 
        {
            "pointername":
            {
                {"targetname":"nodetype"}, 
                {"targetname":"nodetype"}
            }
        },
    }
    """
    logger.info("Start to parse SVF point-to analysis results...")
    FIND_NEW_POINTER = False

    with open(pta_file, "r") as pta_reader:
        lines = pta_reader.readlines()
        lines_len = len(lines)
        for i in range(lines_len):
            line = lines[i]

            if line.startswith(pointer_name_hint):
                FIND_NEW_POINTER = True
            
            if FIND_NEW_POINTER:
                FIND_NEW_POINTER = False
                # print(line)
                """
                ##<> Source Loc: { ln: 273  cl: 27  fl: ../../../mbedtls-2.2.1/library/sha256.c }
                Ptr 6805(mbedtls_sha256_update)		PointsTo: { 7273(FIObjNode) }
                !!Target NodeID 7273	 [<ctx> Source Loc: { ln: 326 fl: ../../../mbedtls-2.2.1/library/sha256.c }] 

                ##<Program_Init> Source Loc: { in line: 229 file: ../../Src/main.c }
                Ptr 350(Program_Init)		PointsTo: { 351(FIObjNode) }
                !!Target NodeID 351	 [<Program_Init> Source Loc: { in line: 229 file: ../../Src/main.c }] 

                ##<padn> Source Loc: { ln: 289 fl: ../../../mbedtls-2.2.1/library/sha256.c }
                Ptr 6831(mbedtls_sha256_finish)		PointsTo: {empty}

                ##<output> Source Loc: { 1st arg mbedtls_sha256_finish in line: 287 file: ../../../mbedtls-2.2.1/library/sha256.c }
                Ptr 6823(mbedtls_sha256_finish)		PointsTo: { 28(FIObjNode) 31(FIObjNode) }
                !!Target NodeID 28	 [<key_in> Source Loc: { Glob ln: 25 fl: ../../Src/main.c }] 
                !!Target NodeID 31	 [<key> Source Loc: { Glob ln: 24 fl: ../../Src/main.c }] 
                """
                # generate new pointer object
                pointer_name = oi_utils.extract_name_angle(line).strip()
                line_w_func, line_w_targets = lines[i+1].split("PointsTo:")
                func_name = oi_utils.extract_name_bracket(line_w_func)

                if pointer_name == "":
                    # local pointers may also point to the global variable
                    """
                    ##<> Source Loc: { ln: 1927  cl: 30  fl: ../../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c }
                    Ptr 1096(UART_SetConfig)		PointsTo: { 13(FIObjNode) }
                    !!Target NodeID 13	 [<UartHandle> Source Loc: { Glob ln: 33 fl: ../../Src/main.c }] 
                    """
                    pass
                    # print(line)

                if func_name == "":
                    # Exclude global variable initialization
                    continue

                if pointer_name == func_name:
                    # Exclude function name. In LLVM, function name is a pointer
                    continue

                if empty_target_hint in line_w_targets:
                    continue

                targets = oi_utils.extract_name_brace(line_w_targets).strip().split(" ")
                targets_num =len(targets)
                # pointer = oi_utils.Pointer(pointer_name, func_name, targets_num)        # Pointer object
                
                # if func_name == "HAL_InitTick" and pointer_name == "TickPriority.addr":
                    # print("found")
                

                for idx, tgt in enumerate(targets):
                    target_node_type = oi_utils.extract_name_bracket(tgt)
                    target_id = tgt.split("(")[0].strip()

                    line = lines[i+1+idx+1]
                    target_id_2 = line.split("NodeID")[1].split("[")[0].strip()
                    assert target_id == target_id_2

                    target_name = oi_utils.extract_name_angle(line)
                    if pointer_name == target_name:
                        continue

                    if target_name not in global_var_list:
                        continue
                    # target = oi_utils.PTATarget(target_id, target_name, target_node_type)
                    # pointer.add_pt_target(target)
                    if func_name not in global_var_mapping.keys():
                        global_var_mapping[func_name] = {}
                    if pointer_name not in global_var_mapping[func_name].keys():
                        global_var_mapping[func_name][pointer_name] = {}
                    if target_name not in global_var_mapping[func_name][pointer_name].keys():
                        global_var_mapping[func_name][pointer_name][target_name] = target_node_type

            else:
                continue

    with open(parse_result_filename, "w") as json_writer:
        json.dump(global_var_mapping, json_writer, sort_keys=True, indent=4)
        logger.warning("Finish writing to file parse_pts.json!")


def parse_pts(pts_result_file, global_var_list, parsed_results_file):
    """
    parse SVF point-to analysis results, write the results to parsed_results_file
    """
    fill_value_name(pts_result_file, global_var_list, parsed_results_file)


if __name__ == "__main__":
    """
    Test
    """
    env_dist = os.environ
    OI_DIR = env_dist["OI_DIR"]
    print("OI_DIR: ", OI_DIR)
    BUILD_PATH = "/stm32f407/pinlock/Decode/SW4STM32/STM32F4-DISCO/"

    svf_pts_result = OI_DIR + BUILD_PATH + "build9/Pinlock.pts"
    def_use = OI_DIR + BUILD_PATH + "build9/Pinlock_def_use.output"
    parsed_results_file = OI_DIR + BUILD_PATH + "Pinlock_parsed_pts.json"
    global_var_list = {}

    parse_pts(svf_pts_result, global_var_list, parsed_results_file)