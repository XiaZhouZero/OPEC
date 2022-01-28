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

# apps = ["PinLock", "Camera_To_USBDisk", "FatFs_RAMDisk", "FatFs_uSD", "LCD_AnimatedPictureFromSDCard", "LCD_PicturesFromSDCard", "LwIP_TCP_Echo_Server"]

apps = ["PinLock", "Camera_To_USBDisk", "FatFs_uSD", "LCD_AnimatedPictureFromSDCard", "LCD_PicturesFromSDCard", "LwIP_TCP_Echo_Server", "CoreMark"]

FLASH_SIZE_1 = 1 * 1024 * 1024
FLASH_SIZE_2 = 2 * 1024 * 1024
SRAM_SIZE_1 = 192 * 1024
SRAM_SIZE_2 = 288 * 1024

Default_Flash_Sections = [".text", ".rodata", ".data"]
Default_SRAM_Sections = [".data", ".bss"]
operation_data_section_prefix = "_Operation"
heap_data_section_prefix = "memp_"


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
 $ readelf -S opec/LwIP_TCP_Echo_Server--oi--final.elf
There are 44 section headers, starting at offset 0x1e18cc:

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .isr_vector       PROGBITS        08000000 010000 0001b4 00   A  0   0  1
  [ 2] .text             PROGBITS        080001c0 0101c0 026df0 00  AX  0   0 64
  [ 3] .rodata           PROGBITS        08040000 040000 005850 00   A  0   0 16
  [ 4] .ARM              ARM_EXIDX       08045850 045850 000008 00  AL  2   0  4
  [ 5] .data             PROGBITS        20001000 051000 0005f0 00  WA  0   0  8
  [ 6] .stack            NOBITS          20000000 180000 001000 00  WA  0   0  1
  [ 7] .ccmram           PROGBITS        10000000 173764 000000 00   W  0   0  1
  [ 8] .bss              NOBITS          200015f0 180000 000810 00  WA  0   0  4
  [ 9] ._user_heap_stack NOBITS          20001e00 180000 001200 00  WA  0   0  1
  [10] memp_ETHDMA       PROGBITS        2001c000 05c000 0030a0 00  WA  0   0  4
  [11] memp_MEMHEAP      PROGBITS        20018000 068000 002820 00  WA  0   0  4
  [12] memp_PBUF_POOL    PROGBITS        20016000 076000 0018c4 00  WA  0   0  4
  [13] memp_TCP_PCB      PROGBITS        20015800 085800 000678 00  WA  0   0  4
  [14] _Operation_0__dat PROGBITS        20015400 095400 00028c 00  WA  0   0  4
  [15] _Operation_1__dat PROGBITS        20015000 0a5000 0002b8 00  WA  0   0  4
  [16] _Operation_2__dat PROGBITS        20014c00 0b4c00 000288 00  WA  0   0  4
  [17] _Operation_3__dat PROGBITS        20014800 0c4800 000288 00  WA  0   0  4
  [18] _Operation_7__dat PROGBITS        20014400 0d4400 000288 00  WA  0   0  4
  [19] _Operation_8__dat PROGBITS        20014000 0e4000 000290 00  WA  0   0  4
  [20] memp_FRAG_PBUF    PROGBITS        20013e00 0f3e00 00016c 00  WA  0   0  4
  [21] memp_PBUF         PROGBITS        20013d00 103d00 0000a4 00  WA  0   0  4
  [22] memp_REASSDATA    PROGBITS        20013c00 113c00 0000a4 00  WA  0   0  4
  [23] memp_SYS_TIMEOUT  PROGBITS        20013b00 123b00 0000a4 00  WA  0   0  4
  [24] memp_TCP_PCB_LIST PROGBITS        20013a00 173764 000000 00   W  0   0  1
  [25] memp_TCP_SEG      PROGBITS        20013900 133900 0000f4 00  WA  0   0  4
  [26] memp_UDP_PCB      PROGBITS        20013800 143800 0000c4 00  WA  0   0  4
  [27] _Operation_4__dat PROGBITS        200137c0 1537c0 000038 00  WA  0   0  4
  [28] _Operation_5__dat PROGBITS        20013780 163780 000030 00  WA  0   0  4
  [29] _Operation_6__dat PROGBITS        20013760 173760 000004 00  WA  0   0  4
  [30] .ARM.attributes   ARM_ATTRIBUTES  00000000 173764 000032 00      0   0  1
  [31] .debug_str        PROGBITS        00000000 173796 00a068 01  MS  0   0  1
  [32] .debug_abbrev     PROGBITS        00000000 17d7fe 00052a 00      0   0  1
  [33] .debug_info       PROGBITS        00000000 17dd28 01da9c 00      0   0  1
  [34] .debug_ranges     PROGBITS        00000000 19b7c4 000018 00      0   0  1
  [35] .debug_macinfo    PROGBITS        00000000 19b7dc 000001 00      0   0  1
  [36] .debug_pubnames   PROGBITS        00000000 19b7dd 005d8f 00      0   0  1
  [37] .debug_pubtypes   PROGBITS        00000000 1a156c 0025e2 00      0   0  1
  [38] .comment          PROGBITS        00000000 1a3b4e 000015 01  MS  0   0  1
  [39] .debug_frame      PROGBITS        00000000 1a3b64 005908 00      0   0  4
  [40] .debug_line       PROGBITS        00000000 1a946c 01ddec 00      0   0  1
  [41] .symtab           SYMTAB          00000000 1c7258 0126d0 10     42 4430  4
  [42] .strtab           STRTAB          00000000 1d9928 007d61 00      0   0  1
  [43] .shstrtab         STRTAB          00000000 1e1689 000241 00      0   0  1
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


"""
struct Pointer_Field {
    uint32_t first_index;
    uint32_t second_index;
};


struct Shadow_Data_TBL {
	uint32_t attr;      // deprecated
	char **ptr_addr;
	char *sha_addr;
	char *org_addr;
	uint32_t size;      // size of shadow variable
    uint32_t need_sanitize;
    uint32_t offset_of_the_var_to_check;
    uint32_t min_value;
    uint32_t max_value;
    uint32_t has_pointer_fields;
    uint32_t pointer_fields_num;
    struct Pointer_Field *pointer_fields_ptr;
};


struct Shadow_Stack_TBL {
	uint32_t total_size;
	uint32_t ptr_num;
	struct {
		int type;		// type为正表示为offset，为负表示寄存器-1到-4表示寄存器r0-r3
		int count;		// count表示个数，负数表示跟其他参数关联，绝对值为索引
		int size;		// size表示每个的大小，负数表示和其他参数关联，绝对值为索引
		char *org_addr;
		char *new_addr;
	}arg[0];
};


struct Sanitization_TBL {
    char **ptr_addr;
    uint32_t min;
    uint32_t max;
};


/**
 * @brief Peripheral_Regionlist is used for switching MPU regions when encountered a Memory Management Fault
 * start_addr: 	start address of the private peripheral
 * end_addr: 	end address of the private peripheral
 * attr:		attribute for uprivileged code, and the encoding rules are below:
 * 					  wr
 * 					0b00: 0: No Access
 * 					0b01: 1: Read-Only
 * 					0b10: 2: Write-only
 * 					0b11: 3: Write&Read
 * mpu_rbar:    MPU RBAR register value, the region number is set to 0
 * mpu_rasr:    MPU RASR register value
 */
struct Peripheral_MPU_Region {
    uint32_t start_addr;
    uint32_t attr;
    uint32_t size;
    uint32_t mpu_rbar;
    uint32_t mpu_rasr;
};


/**
 * @brief PPB_Register_Whitelist is used for checked in PPB access
 * start_addr: 	start address of the private peripheral
 * end_addr: 	end address of the private peripheral
 * attr:		attribute for uprivileged code, and the encoding rules are below:
 * 					  wr
 * 					0b00: 0: No Access
 * 					0b01: 1: Read-Only
 * 					0b10: 2: Write-only
 * 					0b11: 3: Write&Read
 * 
 */
struct PPB_Register_Whitelist {
	uint32_t start_addr;
	uint32_t size;
	uint32_t attr;
};


struct Operation_Policy {
	struct MPU_Region region[8];	// 8 MPU regions for Cortex-M4 MCU
    uint32_t current_operation_id;
    struct Operations_Data_Section_Info *OpeDataSecInfo_ptr;
	uint32_t stack_copy_size;
	struct Shadow_Stack_TBL *stbl;
    uint32_t peripehral_region_num;
    struct Peripheral_MPU_Region *regions_ptr;
	uint32_t ppb_whitelist_num;
	struct PPB_Register_Whitelist *ppb_ptr;
	uint32_t shadowdata_tbl_size;
	struct Shadow_Data_TBL shadowdata_tbl[0];
};


struct Operation_Metadata {
	struct Operation_Policy *policy;
	uint32_t stack_pointer;
};
"""
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
    OPEC-Monitor code size
    initialization code
    instrumented SVCs
    Metadata
    Operation data section
    计算:
    (OPEC_.text + OPEC_.data + OEPC_.rodata + OPEC_.memp + OPEC_._Operation) - (baseline_.text + baseline_.data + baseline_.rodata)
"""
def calc_flash_overhead(BASELINE_binary_section_data, OPEC_binary_section_data):
    delta_text      = OPEC_binary_section_data[".text"]["section_size"] - BASELINE_binary_section_data[".text"]["section_size"]
    delta_rodata    = OPEC_binary_section_data[".rodata"]["section_size"] - BASELINE_binary_section_data[".rodata"]["section_size"]
    delta_data      = OPEC_binary_section_data[".data"]["section_size"] + OPEC_binary_section_data[".data"]["frag_size"] - BASELINE_binary_section_data[".data"]["section_size"]

    delta_extra_sections = 0
    delta_extra_frags    = 0
    delta_memp_sections  = 0
    delta_memp_frags     = 0
    operation_cnt = 0
    memp_cnt = 0

    for section, info in OPEC_binary_section_data.items():
        if section.startswith(operation_data_section_prefix):
            operation_cnt += 1
            delta_extra_sections += info["section_size"]
            delta_extra_frags += info["frag_size"]

        elif section.startswith(heap_data_section_prefix):
            memp_cnt += 1
            delta_memp_sections += info["section_size"]
            delta_memp_frags += info["frag_size"]
    # initialization instructions
    # operation_initdata
    # operation_start
    # environment_init
    # bl to old main
    initialized_code_size = 4 + (8+8+8+4)*(operation_cnt+memp_cnt) + (8+4) + 4 + 4 + 4 + 4
    instrumented_svc_size = (4+4) * operation_cnt
    delta_text = delta_text + instrumented_svc_size + initialized_code_size
    total = delta_text + delta_rodata + delta_data + delta_extra_sections + delta_extra_frags + delta_memp_sections + delta_memp_frags
    result = {
        ".text": delta_text,
        ".rodata": delta_rodata,
        ".data": delta_data,
        ".operation_data_sections": delta_extra_sections + delta_extra_frags,
        ".memp_data_sections": delta_memp_sections + delta_memp_frags,
        "total": total
    }
    return result


"""
    SRAM overhead:
    operation_data_sections
    .data
    .bss
    memp_sections
    计算方式:
    (OPEC_.data + OEPC_.bss + OPEC_.memp + operation_data_sections) - (baseline_.data + baseline_.bss + baseline_.rodata)
"""
def calc_sram_overhead(BASELINE_binary_section_data, OPEC_binary_section_data):
    delta_data      = OPEC_binary_section_data[".data"]["section_size"] + OPEC_binary_section_data[".data"]["frag_size"] - BASELINE_binary_section_data[".data"]["section_size"]
    delta_bss       = OPEC_binary_section_data[".bss"]["section_size"] - BASELINE_binary_section_data[".bss"]["section_size"]

    delta_extra_sections = 0
    delta_extra_frags    = 0
    delta_memp_sections  = 0
    delta_memp_frags     = 0
    operation_cnt = 0
    memp_cnt = 0

    for section, info in OPEC_binary_section_data.items():
        if section.startswith(operation_data_section_prefix):
            operation_cnt += 1
            delta_extra_sections += info["section_size"]
            delta_extra_frags += info["frag_size"]

        elif section.startswith(heap_data_section_prefix):
            memp_cnt += 1
            delta_memp_sections += info["section_size"]
            delta_memp_frags += info["frag_size"]

    total = delta_data + delta_bss + delta_extra_sections + delta_extra_frags + delta_memp_sections + delta_memp_frags
    result = {
        ".data": delta_data,
        ".bss": delta_bss,
        ".operation_data_sections": delta_extra_sections + delta_extra_frags,
        ".memp_data_sections": delta_memp_sections + delta_memp_frags,
        "total": total
    }
    return result


def calc_privileged_code_ratio(BASELINE_binary_section_data, OPEC_binary_section_data):
    # OPEC-Monitor code size计算方式: 通过ghidra查看
    opec_monitor_section_size = 0x080269F4 - 0x08024A7A
    operation_cnt = 0
    for section, info in OPEC_binary_section_data.items():
        if section.startswith(operation_data_section_prefix) or section.startswith(heap_data_section_prefix):
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
    for operation_name, info in overhead_info.items():
        info["size"] = round(info["size"] / 1.0 / 1024, 2)
        updated_overhead_info[operation_name] = info
    return updated_overhead_info


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--cur_dir', dest='CUR_DIR', default="", help='The path of current figure directory')
    args = parser.parse_args()
    dir_name = args.CUR_DIR

    # Total_privileged_code = {}
    Total_Flash = {}
    Total_SRAM = {}
    for app in apps:
        logger.info("APP: {}".format(app))
        baseline_file = dir_name + "/bins/baseline/" + app + "--baseline.elf"
        format_baseline_data = parse_readelf_data(readelf(baseline_file))
        store_json_to_file(format_baseline_data, "BASELINE_"+app+"_bin_sections.json")
        opec_file = dir_name + "/bins/opec/" + app + "--oi--final.elf"
        format_opec_data = parse_readelf_data(readelf(opec_file))
        store_json_to_file(format_opec_data, "OPEC_" + app + "_bin_sections.json")

        # evaluate privileged code ratio
        # privileged_code_size, privileged_code_ratio = calc_privileged_code_ratio(format_baseline_data, format_opec_data)
        # Total_privileged_code[app] = {"size": privileged_code_size, "ratio": round(privileged_code_ratio*100, 3)}

        # evaluate flash overhead
        flash_overhead_ = calc_flash_overhead(format_baseline_data, format_opec_data)
        Total_Flash[app] = {
            "size": flash_overhead_["total"]                 # Total overhead(Bytes)
        }
        if app == "Pinlock":
            Total_Flash[app]["ratio"] = normalized_flash_1(Total_Flash[app]["size"])
        else:
            Total_Flash[app]["ratio"] = normalized_flash_2(Total_Flash[app]["size"])

        # evaluate sram overhead
        sram_overhead_ = calc_sram_overhead(format_baseline_data, format_opec_data)
        Total_SRAM[app] = {
            "size": sram_overhead_["total"]                  # Total overhead(Bytes)
            }
        if app == "Pinlock":
            Total_SRAM[app]["ratio"] = normalized_sram_1(Total_SRAM[app]["size"])
        else:
            Total_SRAM[app]["ratio"] = normalized_sram_2(Total_SRAM[app]["size"])

    # print("#"*20 + " Privileged Code Ratio " + "#"*20)
    # print(json.dumps(Total_privileged_code, indent=4, sort_keys=True))

    # print("#"*20 + " Flash Overhead " + "#"*20)
    # print(json.dumps(Total_Flash, indent=4, sort_keys=True))

    # print("#"*20 + " Sram Overhead " + "#"*20)
    # print(json.dumps(Total_SRAM, indent=4, sort_keys=True))

    # already calculated in table1
    # store_json_to_file(transfer_bytes_to_kb(Total_privileged_code), "privileged_code.json")
    store_json_to_file(transfer_bytes_to_kb(Total_Flash), dir_name+"/flash_overhead.json")
    store_json_to_file(transfer_bytes_to_kb(Total_SRAM), dir_name+"/sram_overhead.json")

if __name__ == '__main__':
    main()
