#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import subprocess
import json
import logging
import argparse

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

apps = ["PinLock", "FatFs_uSD", "LCD_AnimatedPictureFromSDCard", "LCD_PicturesFromSDCard", "LwIP_TCP_Echo_Server"]
policies = ["filename", "filename-no-opt", "peripheral"]

FLASH_SIZE_1 = 1 * 1024 * 1024
FLASH_SIZE_2 = 2 * 1024 * 1024
SRAM_SIZE_1 = 192 * 1024
SRAM_SIZE_2 = 288 * 1024

Default_Flash_Sections = [".text", ".rodata", ".data"]
Default_SRAM_Sections = [".data", ".bss"]

ACES_code_section_prefix = "CODE_REGION"
ACES_data_section_prefix = "DATA_REGION"
ACES_hexbox_code = "hexbox_rt_code"
ACES_hexbox_data = "hexbox_rt_rm"


def store_json_to_file(json_to_dump, filename):
    with open(filename, "w") as f:
        json.dump(json_to_dump, f, sort_keys=True, indent=4)


def load_json_from_file(filename):
    with open(filename, "r") as f:
        json_to_load = json.load(f)
    return json_to_load


def next_power_2(size):
    if size == 0:
        return 0
    elif 0< size <=32:
        return 32
    else:
        return 1 << (size - 1).bit_length()


"""
$ readelf -S bin/TCP-Echo--filename-no-opt--mpu-8--hexbox--final.elf
There are 111 section headers, starting at offset 0x13d6dc:

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .isr_vector       PROGBITS        08000000 010000 0001b4 00   A  0   0  1
  [ 2] .text             PROGBITS        080001b4 0101b4 000ea0 00  AX  0   0  4
  [ 3] .rodata           PROGBITS        08001060 011060 0036d0 00   A  0   0 16
  [ 4] .ARM              ARM_EXIDX       08004730 014730 000008 00  AL 43   0  4
  [ 5] .data             PROGBITS        20001000 021000 000430 00  WA  0   0  8
  [ 6] .hexbox_rt_code   PROGBITS        08004738 014738 001078 00  AX  0   0  4
  [ 7] .CODE_REGION_14_  PROGBITS        080fc000 0dc000 002672 00  AX  0   0  4
  [ 8] .CODE_REGION_16_  PROGBITS        080fa000 0da000 001580 00  AX  0   0  2
  [ 9] .CODE_REGION_6_   PROGBITS        080f8000 0d8000 0011f2 00  AX  0   0  2
  [10] .CODE_REGION_0_   PROGBITS        080f6000 0d6000 0013e4 00  AX  0   0  2
  [11] .CODE_REGION_20_  PROGBITS        080f4000 0d4000 001c1c 00  AX  0   0  4
  [12] .CODE_REGION_35_  PROGBITS        080f2000 0d2000 001494 00  AX  0   0  2
  [13] .CODE_REGION_5_   PROGBITS        080f0000 0d0000 001d78 00  AX  0   0  2
  [14] .CODE_REGION_12_  PROGBITS        080ef000 0cf000 000fc6 00  AX  0   0  4
  [15] .CODE_REGION_22_  PROGBITS        080ee000 0ce000 000f44 00  AX  0   0  2
  [16] .CODE_REGION_28_  PROGBITS        080ed000 0cd000 0008de 00  AX  0   0  2
  [17] .CODE_REGION_26_  PROGBITS        080ec000 0cc000 000820 00  AX  0   0  2
  [18] .CODE_REGION_31_  PROGBITS        080eb000 0cb000 000a70 00  AX  0   0  2
  [19] .CODE_REGION_1_   PROGBITS        080ea000 0ca000 000cec 00  AX  0   0  4
  [20] .CODE_REGION_23_  PROGBITS        080e9800 0c9800 00052a 00  AX  0   0  2
  [21] .CODE_REGION_15_  PROGBITS        080e9000 0c9000 0007a2 00  AX  0   0  2
  [22] .CODE_REGION_8_   PROGBITS        080e8800 0c8800 00045a 00  AX  0   0  2
  [23] .CODE_REGION_32_  PROGBITS        080e8000 0c8000 00049e 00  AX  0   0  2
  [24] .CODE_REGION_4_   PROGBITS        080e7800 0c7800 00043c 00  AX  0   0  2
  [25] .CODE_REGION_13_  PROGBITS        080e7000 0c7000 000650 00  AX  0   0  2
  [26] .CODE_REGION_9_   PROGBITS        080e6800 0c6800 0004ba 00  AX  0   0  2
  [27] .CODE_REGION_11_  PROGBITS        080e6000 0c6000 000722 00  AX  0   0  2
  [28] .CODE_REGION_33_  PROGBITS        080e5800 0c5800 00050a 00  AX  0   0  2
  [29] .CODE_REGION_21_  PROGBITS        080e5400 0c5400 000376 00  AX  0   0  2
  [30] .CODE_REGION_25_  PROGBITS        080e5200 0c5200 000104 00  AX  0   0  2
  [31] .CODE_REGION_27_  PROGBITS        080e5000 0c5000 000168 00  AX  0   0  2
  [32] .CODE_REGION_24_  PROGBITS        080e4e00 0c4e00 0001ac 00  AX  0   0  2
  [33] .CODE_REGION_19_  PROGBITS        080e4c00 0c4c00 00011c 00  AX  0   0  2
  [34] .CODE_REGION_18_  PROGBITS        080e4a00 0c4a00 000122 00  AX  0   0  2
  [35] .CODE_REGION_3_   PROGBITS        080e4800 0c4800 0001ca 00  AX  0   0  2
  [36] .CODE_REGION_30_  PROGBITS        080e4600 0c4600 000148 00  AX  0   0  2
  [37] .CODE_REGION_2_   PROGBITS        080e4500 0c4500 0000da 00  AX  0   0  2
  [38] .CODE_REGION_7_   PROGBITS        080e4400 0c4400 000090 00  AX  0   0  2
  [39] .CODE_REGION_29_  PROGBITS        080e4380 0c4380 00005e 00  AX  0   0  2
  [40] .CODE_REGION_34_  PROGBITS        080e4300 0c4300 000070 00  AX  0   0  2
  [41] .CODE_REGION_17_  PROGBITS        080e4280 0c4280 00005e 00  AX  0   0  2
  [42] .IRQ_CODE_REGION  PROGBITS        080e4200 0c4200 000052 00  AX  0   0  2
  [43] .CODE_REGION_10_  PROGBITS        080e41c0 0c41c0 000026 00  AX  0   0  2
  [44] .stack            NOBITS          20000000 0e0000 001000 00  WA  0   0  1
  [45] .ccmram           PROGBITS        10000000 0ee4dc 000000 00   W  0   0  1
  [46] .bss              NOBITS          20001430 0e0000 000004 00  WA  0   0  4
  [47] ._user_heap_stack NOBITS          20001434 0e0000 000204 00  WA  0   0  1
  [48] .hexbox_rt_ram    PROGBITS        20001638 021638 00086c 00  WA  0   0  4
  [49] .DATA_REGION_16__ PROGBITS        2001c000 02c000 000090 00   A  0   0  4
  [50] .DATA_REGION_16__ PROGBITS        2001c090 0ec090 00244c 00  WA  0   0  4
  [51] .DATA_REGION_21__ PROGBITS        20018000 0ee4dc 000000 00   W  0   0  1
  [52] .DATA_REGION_21__ PROGBITS        20018000 0e8000 0030ec 00  WA  0   0  4
  [53] .DATA_REGION_6__d PROGBITS        20014000 0ee4dc 000000 00   W  0   0  1
  [54] .DATA_REGION_6__b PROGBITS        20014000 0e4000 002820 00  WA  0   0  4
  [55] .DATA_REGION_22__ PROGBITS        20013e00 033e00 000008 00   A  0   0  1
  [56] .DATA_REGION_22__ PROGBITS        20013e08 0e3e08 0000fc 00   A  0   0  4
  [57] .DATA_REGION_12__ PROGBITS        20013d80 043d80 00002c 00   A  0   0  4
  [58] .DATA_REGION_12__ PROGBITS        20013dac 0e3dac 00001c 00  WA  0   0  4
  [59] .DATA_REGION_7__d PROGBITS        20013d00 053d00 00002c 00  WA  0   0  4
  [60] .DATA_REGION_7__b PROGBITS        20013d2c 0e3d2c 000004 00  WA  0   0  1
  [61] .DATA_REGION_15__ PROGBITS        20013c80 063c80 000010 00   A  0   0  4
  [62] .DATA_REGION_15__ PROGBITS        20013c90 0e3c90 000058 00  WA  0   0  4
  [63] .DATA_REGION_9__d PROGBITS        20013c40 0ee4dc 000000 00   W  0   0  1
  [64] .DATA_REGION_9__b PROGBITS        20013c40 0e3c40 00003c 00  WA  0   0  4
  [65] .DATA_REGION_20__ PROGBITS        20013c00 073c00 000004 00  WA  0   0  2
  [66] .DATA_REGION_20__ PROGBITS        20013c04 0e3c04 000004 00  WA  0   0  4
  [67] .DATA_REGION_8__d PROGBITS        20013bc0 083bc0 000004 00  WA  0   0  1
  [68] .DATA_REGION_8__b PROGBITS        20013bc4 0e3bc4 000004 00  WA  0   0  4
  [69] .DATA_REGION_13__ PROGBITS        20013b80 0ee4dc 000000 00   W  0   0  1
  [70] .DATA_REGION_13__ PROGBITS        20013b80 0e3b80 000038 00  WA  0   0  4
  [71] .DATA_REGION_0__d PROGBITS        20013b40 093b40 000024 00  WA  0   0  4
  [72] .DATA_REGION_0__b PROGBITS        20013b64 0ee4dc 000000 00   W  0   0  1
  [73] .DATA_REGION_2__d PROGBITS        20013b00 0a3b00 000004 00   A  0   0  4
  [74] .DATA_REGION_2__b PROGBITS        20013b04 0e3b04 000004 00   A  0   0  4
  [75] .DATA_REGION_14__ PROGBITS        20013ae0 0ee4dc 000000 00   W  0   0  1
  [76] .DATA_REGION_14__ PROGBITS        20013ae0 0e3ae0 000008 00  WA  0   0  4
  [77] .DATA_REGION_10__ PROGBITS        20013ac0 0ee4dc 000000 00   W  0   0  1
  [78] .DATA_REGION_10__ PROGBITS        20013ac0 0e3ac0 000004 00  WA  0   0  4
  [79] .DATA_REGION_3__d PROGBITS        20013aa0 0ee4dc 000000 00   W  0   0  1
  [80] .DATA_REGION_3__b PROGBITS        20013aa0 0e3aa0 00000c 00  WA  0   0  4
  [81] .DATA_REGION_5__d PROGBITS        20013a80 0b3a80 000010 00   A  0   0  1
  [82] .DATA_REGION_5__b PROGBITS        20013a90 0ee4dc 000000 00   W  0   0  1
  [83] .DATA_REGION_19__ PROGBITS        20013a60 0ee4dc 000000 00   W  0   0  1
  [84] .DATA_REGION_19__ PROGBITS        20013a60 0e3a60 000004 00  WA  0   0  4
  [85] .DATA_REGION_1__d PROGBITS        20013a40 0ee4dc 000000 00   W  0   0  1
  [86] .DATA_REGION_1__b PROGBITS        20013a40 0e3a40 00000c 00  WA  0   0  4
  [87] .DATA_REGION_11__ PROGBITS        20013a20 0c3a20 000004 00  WA  0   0  4
  [88] .DATA_REGION_11__ PROGBITS        20013a24 0ee4dc 000000 00   W  0   0  1
  [89] .DATA_REGION_17__ PROGBITS        20013a00 0ee4dc 000000 00   W  0   0  1
  [90] .DATA_REGION_17__ PROGBITS        20013a00 0e3a00 000004 00  WA  0   0  4
  [91] .DATA_REGION_18__ PROGBITS        200139e0 0ee4dc 000000 00   W  0   0  1
  [92] .DATA_REGION_18__ PROGBITS        200139e0 0e39e0 00001c 00  WA  0   0  4
  [93] .DATA_REGION_4__d PROGBITS        200139e0 0ee4dc 000000 00   W  0   0  1
  [94] .DATA_REGION_4__b PROGBITS        200139e0 0ee4dc 000000 00   W  0   0  1
  [95] .ARM.attributes   ARM_ATTRIBUTES  00000000 0ee4dc 000032 00      0   0  1
  [96] .debug_info       PROGBITS        00000000 0ee50e 012ca0 00      0   0  1
  [97] .debug_abbrev     PROGBITS        00000000 1011ae 00072f 00      0   0  1
  [98] .debug_aranges    PROGBITS        00000000 1018dd 000028 00      0   0  1
  [99] .debug_line       PROGBITS        00000000 101905 01050c 00      0   0  1
  [100] .debug_str        PROGBITS        00000000 111e11 006513 01  MS  0   0  1
  [101] .debug_loc        PROGBITS        00000000 118324 000bb6 00      0   0  1
  [102] .debug_ranges     PROGBITS        00000000 118eda 0006a0 00      0   0  1
  [103] .debug_macinfo    PROGBITS        00000000 11957a 000001 00      0   0  1
  [104] .debug_pubnames   PROGBITS        00000000 11957b 002aac 00      0   0  1
  [105] .debug_pubtypes   PROGBITS        00000000 11c027 001d18 00      0   0  1
  [106] .comment          PROGBITS        00000000 11dd3f 000015 01  MS  0   0  1
  [107] .debug_frame      PROGBITS        00000000 11dd54 0024fc 00      0   0  4
  [108] .symtab           SYMTAB          00000000 120250 013a90 10     109 3992  4
  [109] .strtab           STRTAB          00000000 133ce0 009292 00      0   0  1
  [110] .shstrtab         STRTAB          00000000 13cf72 000767 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  y (purecode), p (processor specific)
"""


def readelf(object_filename):
    cmd = ['readelf', '-S']
    cmd.append(object_filename)
    stdout = subprocess.check_output(cmd).decode()
    return stdout.split("\n")


def parse_readelf_data(raw_readelf_data):
    """
    {
        "section_name": {
            "section_addr":x,
            "section_offset":x,
            "section_size":x,
            "section_type":"xxx",
            "frag_size":x
        }
    }
    """
    format_readelf_data = {}
    for line in raw_readelf_data:
        if "[" in line and "]" in line and "Nr" not in line and "NULL" not in line:
            elements = line.split("]")[1].split()
            section_name = elements[0]
            section_type = elements[1]
            section_addr = int(elements[2], 16)
            section_offset = int(elements[3], 16)
            section_size = int(elements[4], 16)
            frag_size = next_power_2(section_size) - section_size
            # print("frag_size:", frag_size)
            format_readelf_data[section_name] = {
                "section_type": section_type, 
                "section_addr": section_addr, 
                "section_offset": section_offset, 
                "section_size": section_size, 
                "frag_size": frag_size
                }
    # print(format_readelf_data)
    return format_readelf_data


def calculate_metadata_size(policy):
    operation_cnt = len(policy["Operation"].keys())
    TOTAL = 0
    struct_operation_metadata_size = 4*2*operation_cnt
    struct_operation_policy_size = 4*10*operation_cnt
    MPU_region_size = (4+4)*8*operation_cnt
    shadow_stack_table_size = 4*3+4*5*operation_cnt
    PPB_register_white_size = 4*3
    shadow_data_tbl_size = 4*5
    TOTAL += struct_operation_metadata_size + struct_operation_policy_size + MPU_region_size + shadow_stack_table_size
    for ope_name, operation_policy in policy["Operation"].items():
        TOTAL += PPB_register_white_size * len(operation_policy["Whitelist"])
        TOTAL += shadow_data_tbl_size * len(operation_policy["ExternalData"])
    return TOTAL


"""
    造成Flash Overhead的原因: 
    Hexbox_rt_code code size
    计算:
    (ACES_.text + ACES_.data + ACES_.rodata + ACES_.CODE_REGION + ACES_.DATA_REGION + ACES_.Hexbox_rt_code + ACES_.Hexbox_rt_ram) - (baseline_.text + baseline_.data + baseline_.rodata)
"""
def calc_flash_overhead(BASELINE_binary_section_data, ACES_binary_section_data):
    delta_text      = ACES_binary_section_data[".text"]["section_size"] - BASELINE_binary_section_data[".text"]["section_size"]
    delta_rodata    = ACES_binary_section_data[".rodata"]["section_size"] - BASELINE_binary_section_data[".rodata"]["section_size"]
    delta_data      = ACES_binary_section_data[".data"]["section_size"] - BASELINE_binary_section_data[".data"]["section_size"]

    delta_aces_code_sections = 0
    delta_aces_code_frags    = 0
    delta_aces_data_sections = 0
    delta_aces_data_frags    = 0

    for key in ACES_binary_section_data.keys():
        if ACES_code_section_prefix in key or ACES_hexbox_code in key:
            delta_aces_code_sections += ACES_binary_section_data[key]["section_size"]
            delta_aces_code_frags += ACES_binary_section_data[key]["frag_size"]

        elif ACES_data_section_prefix in key or ACES_hexbox_data in key:
            delta_aces_data_sections += ACES_binary_section_data[key]["section_size"]
            delta_aces_data_frags += ACES_binary_section_data[key]["frag_size"]

    total = delta_text + delta_rodata + delta_data + delta_aces_code_sections + delta_aces_code_frags + delta_aces_data_sections + delta_aces_data_frags
    result = {
        ".text": delta_text,
        ".rodata": delta_rodata,
        ".data": delta_data,
        ".aces_code_sections": delta_aces_code_sections + delta_aces_code_frags,
        ".aces_data_sections": delta_aces_data_sections + delta_aces_data_frags,
        "total": total
    }
    return result


"""
    SRAM overhead:
    ACES_Data_Sections
    hexbox data section
    .data
    .bss
    计算方式:
    (ACES_.data + ACES_.bss + ACES_.DATA_REGION + ACES_.Hexbox_rt_ram) - (baseline_.data + baseline_.bss + baseline_.rodata)
"""
def calc_sram_overhead(BASELINE_binary_section_data, ACES_binary_section_data):
    delta_data      = ACES_binary_section_data[".data"]["section_size"] + ACES_binary_section_data[".data"]["frag_size"] - BASELINE_binary_section_data[".data"]["section_size"]
    delta_bss       = ACES_binary_section_data[".bss"]["section_size"] - BASELINE_binary_section_data[".bss"]["section_size"]

    delta_aces_data_sections = 0
    delta_aces_data_frags    = 0

    for key in ACES_binary_section_data.keys():
        if ACES_data_section_prefix in key or ACES_hexbox_data in key:
            delta_aces_data_sections += ACES_binary_section_data[key]["section_size"]
            delta_aces_data_frags += ACES_binary_section_data[key]["frag_size"]

    total = delta_data + delta_bss + delta_aces_data_sections + delta_aces_data_frags
    result = {
        ".data": delta_data,
        ".bss": delta_bss,
        ".operation_data_sections": delta_aces_data_sections + delta_aces_data_frags,
        "total": total
    }
    return result


def calc_privileged_code_ratio(BASELINE_binary_section_data, ACES_binary_section_data):
    # OPEC-Monitor code size计算方式: 通过ghidra查看
    opec_monitor_section_size = 0x080269F4 - 0x08024A7A
    operation_cnt = 0
    for section, info in ACES_binary_section_data.items():
        if section.startswith(ACES_data_section_prefix) or section.startswith(heap_data_section_prefix):
            operation_cnt += 1
    initialized_code_size = 4 + (8+8+8+4)*operation_cnt + (8+4) + 4 + 4 + 4 + 4
    privileged_code_size = opec_monitor_section_size + initialized_code_size

    unprivileged_code_size = BASELINE_binary_section_data[".text"]["section_size"]

    ratio = privileged_code_size * 1.0 / (privileged_code_size + unprivileged_code_size)
    return privileged_code_size, ratio


def normalized_flash_1(size):
    return round(size*1.0/FLASH_SIZE_1*100, 3)


def normalized_flash_2(size):
    return round(size*1.0/FLASH_SIZE_2*100, 3)


def normalized_sram_1(size):
    return round(size*1.0/SRAM_SIZE_1*100, 3)


def normalized_sram_2(size):
    return round(size*1.0/SRAM_SIZE_2*100, 3)


def transfer_bytes_to_kb(overhead_info):
    updated_overhead_info = {}
    for app, app_info in overhead_info.items():
        updated_overhead_info[app] = {}
        for policy, policy_info in app_info.items():
            policy_info["size"] = round(policy_info["size"] / 1.0 / 1024, 2)
            updated_overhead_info[app][policy] = policy_info

    return updated_overhead_info


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--cur_dir', dest='CUR_DIR', default="", help='The path of current figure directory')
    args = parser.parse_args()
    dir_name = args.CUR_DIR

    apps = ["PinLock", "Animation", "FatFs-uSD", "LCD-uSD", "TCP-Echo"]
    app_name_mappings = {
        "PinLock": "PinLock", 
        "Animation": "LCD_AnimatedPictureFromSDCard", 
        "FatFs-uSD": "FatFs_uSD", 
        "LCD-uSD": "LCD_PicturesFromSDCard", 
        "TCP-Echo": "LwIP_TCP_Echo_Server", 
    }


    policies = ["ACES1", "ACES2", "ACES3"]
    policy_name_mappings = {
        "ACES1": "filename",
        "ACES2": "filename-no-opt",
        "ACES3": "peripheral"
    }

    Total_privileged_code = {}
    Total_Flash = {}
    Total_SRAM = {}
    for app in apps:
        app_complex = app_name_mappings[app]
        Total_Flash[app] = {}
        Total_SRAM[app] = {}
        logger.info("APP: {}".format(app))
        baseline_file = dir_name + "/bins/baseline/" + app_complex + "--baseline.elf"
        format_baseline_data = parse_readelf_data(readelf(baseline_file))
        store_json_to_file(format_baseline_data, dir_name + "/" + "BASELINE_" + app + "_bin_sections.json")
        for policy in policies:
            policy_complex = policy_name_mappings[policy]
            Total_Flash[app][policy] = {}
            Total_SRAM[app][policy] = {}
            aces_file = dir_name + "/bins/aces/" + app_complex + "--" + policy_complex + "--mpu-8--hexbox--final.elf"
            format_aces_data = parse_readelf_data(readelf(aces_file))
            store_json_to_file(format_aces_data, dir_name + "/" + "ACES_" + app + "_" + policy + "_bin_sections.json")

            # evaluate privileged code ratio
            # privileged_code_size, privileged_code_ratio = calc_privileged_code_ratio(format_baseline_data, format_aces_data)
            # Total_privileged_code[app] = {"size": privileged_code_size, "ratio": round(privileged_code_ratio*100, 3)}

            # evaluate flash overhead
            flash_overhead_ = calc_flash_overhead(format_baseline_data, format_aces_data)
            Total_Flash[app][policy] = {
                "size": flash_overhead_["total"]                 # Total overhead(Bytes)
            }
            if app == "Pinlock":
                Total_Flash[app][policy]["ratio"] = normalized_flash_1(Total_Flash[app][policy]["size"])
            else:
                Total_Flash[app][policy]["ratio"] = normalized_flash_2(Total_Flash[app][policy]["size"])

            # evaluate sram overhead
            sram_overhead_ = calc_sram_overhead(format_baseline_data, format_aces_data)
            Total_SRAM[app][policy] = {
                "size": sram_overhead_["total"]                  # Total overhead(Bytes)
                }
            if app == "Pinlock":
                Total_SRAM[app][policy]["ratio"] = normalized_sram_1(Total_SRAM[app][policy]["size"])
            else:
                Total_SRAM[app][policy]["ratio"] = normalized_sram_2(Total_SRAM[app][policy]["size"])

        # print("#"*20 + " Privileged Code Ratio " + "#"*20)
        # print(json.dumps(Total_privileged_code, indent=4, sort_keys=True))

        # print("#"*20 + " Flash Overhead " + "#"*20)
        # print(json.dumps(Total_Flash, indent=4, sort_keys=True))

        # print("#"*20 + " Sram Overhead " + "#"*20)
        # print(json.dumps(Total_SRAM, indent=4, sort_keys=True))


    store_json_to_file(transfer_bytes_to_kb(Total_privileged_code), dir_name + "/" + "privileged_code.json")
    store_json_to_file(transfer_bytes_to_kb(Total_Flash), dir_name + "/" + "ACES_flash_overhead.json")
    store_json_to_file(transfer_bytes_to_kb(Total_SRAM), dir_name + "/" + "ACES_sram_overhead.json")

if __name__ == '__main__':
    main()
