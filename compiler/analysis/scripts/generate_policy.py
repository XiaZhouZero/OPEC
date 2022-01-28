#!/usr/bin python
# -*- coding: UTF-8 -*-

import os
import sys
import json
import subprocess
import argparse
import mpu_helpers as mpu
import operator
import oi_utils

STEP_ONE = "1"
STEP_TWO = "2"
BASIC_REGIONS = 4
REGIONS_NUM = 8

# ELF file example
# Section Headers:
#   [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
#   [ 0]                   NULL            00000000 000000 000000 00      0   0  0
#   [ 1] .isr_vector       PROGBITS        08000000 010000 000188 00   A  0   0  1
#   [ 2] .text             PROGBITS        080001c0 0101c0 016ff0 00  AX  0   0 64
#   [ 3] .rodata           PROGBITS        080171b0 0271b0 000760 00   A  0   0 16
#   [ 4] .ARM              ARM_EXIDX       08017910 027910 000008 00  AL  2   0  4
#   [ 5] .data             PROGBITS        20004000 034000 000a48 00  WA  0   0  8
#   [ 6] _Operation_2__dat PROGBITS        20004a48 034a48 000064 00  WA  0   0  4
#   [ 7] _Operation_1__dat PROGBITS        20004aac 034aac 000026 00  WA  0   0  1
#   [ 8] _Operation_0__dat PROGBITS        20004ad4 034ad4 000040 00  WA  0   0  4
#   [ 9] _Operation_3__dat PROGBITS        20004b14 034b14 000001 00  WA  0   0  1
#   [10] .stack            NOBITS          20000000 040000 004000 00  WA  0   0  1
#   [11] .ccmram           PROGBITS        10000000 034b15 000000 00   W  0   0  1
#   [12] .bss              NOBITS          20004b18 040000 00097c 00  WA  0   0  4
#   [13] ._user_heap_stack NOBITS          20005494 040000 004000 00  WA  0   0  1


# Program Headers:
#   Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
#   LOAD           0x010000 0x08000000 0x08000000 0x17918 0x17918 R E 0x10000
#   LOAD           0x034000 0x20004000 0x08017918 0x00ad2 0x00ad2 RW  0x10000
#   LOAD           0x034ad4 0x20004ad4 0x080183ea 0x00041 0x00041 RW  0x10000
#   LOAD           0x040000 0x20000000 0x20000000 0x00000 0x09494 RW  0x10000
#   GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x10

#  Section to Segment mapping:
#   Segment Sections...
#    00     .isr_vector .text .rodata .ARM 
#    01     .data _Operation_2__data _Operation_1__data 
#    02     _Operation_0__data _Operation_3__data 
#    03     .stack .bss ._user_heap_stack 
#    04     



DEFAULT_RAM_SECTIONS = ['.data', '.bss', '._user_heap_stack',
                        '.stack', '.oi_rt_ram','ccmram']
DEFAULT_FLASH_SECTIONS = ['.isr_vector', '.rodata', '.ARM.extab',
                          '.ARM', '.text', '.oi_rt_code']
DEFAULT_STACK_REGION = '.stack'

FLASH_ADDR = 0x08000000
FLASH_SIZE = 1*1024*1024
RAM_ADDR = 0x20000000
RAM_SIZE = 128*1024


def readelf(object_filename):
    print(os.path.abspath(os.curdir))
    cmd = ['arm-none-eabi-size', '-A','-d']
    cmd.append(object_filename)
    stdout = subprocess.check_output(cmd).decode() #byte2str
    section_data=stdout.split('\n')
    return section_data


def upper_bound_value(val):
    retval = 1
    while retval < val:
        retval = retval << 1
    return retval


def MPU_size(val):
    ret = upper_bound_value(val)
    if ret < 32:
        return 32
    else:
        return ret


def lower_align(base, size):
    return base & ~(size - 1)


def upper_align(base, size):
    return (base + size - 1) & ~(size - 1)


def get_section_info(src_policy, size_output_str):
    # get operation shadow data addr and size
    data = size_output_str
    shadow_sections = {}
    default_sections = {}
    isr_text_rodata_section = {}
    for line in data[2:]:       # Skip first two lines
        fields = line.split()
        if len(fields) != 3:    # skip final line
            break
        name = fields[0]
        size = int(fields[1])   # get the section size if it is a shadow_sections
        if size <= 0:
            continue
        if (len(name) > 5) and (src_policy['Operation'].get(name[0:-5])):    
                if name in shadow_sections:
                    shadow_sections[name]['size'] += size
                else:
                    shadow_sections[name] = {'size': size, 'addr':int(fields[2])}

        else:
            if name in default_sections.keys():
                default_sections[name]['size'] += size
            else:
                if name in DEFAULT_RAM_SECTIONS or name in DEFAULT_FLASH_SECTIONS or name in DEFAULT_STACK_REGION:
                    default_sections[name] = {'size':size,'addr':int(fields[2])}

        # if name == ".opec_monitor":
        #     isr_text_rodata_section[name] = {'size': size, 'addr':int(fields[2])}
        
        if name == ".text":
            isr_text_rodata_section[name] = {'size': size, 'addr':int(fields[2])}

        if name == ".isr_vector":
            isr_text_rodata_section[name] = {'size': size, 'addr':int(fields[2])}
        
        if name == ".rodata":
            isr_text_rodata_section[name] = {'size': size, 'addr':int(fields[2])}



    # flag = 1  
    # for name in shadow_sections:
    #     if flag:
    #         shadow_sections[name]['addr'] = (shadow_sections[name]['addr']+31)&0xffffffe0
    #         current_addr = shadow_sections[name]['addr']+shadow_sections[name]['size']
    #         flag = 0
    #     else:
    #         shadow_sections[name]['addr'] = (current_addr+31)&0xffffffe0
    #         current_addr = shadow_sections[name]['addr']+shadow_sections[name]['size']
        # print("%x %x"%(shadow_sections[name]['addr'],shadow_sections[name]['addr']+shadow_sections[name]['size']))
    # Update shadow_setions addr here
    current_addr = default_sections['.data']['addr'] + default_sections['.data']['size']
    for name in sorted(shadow_sections):
        print("%x %x"%(current_addr, shadow_sections[name]['size']))
        current_addr = upper_align(current_addr, MPU_size(shadow_sections[name]['size']))
        print("inter addr : %x %x"%(current_addr, MPU_size(shadow_sections[name]['size'])))
        shadow_sections[name]['addr'] = current_addr
        current_addr = current_addr + MPU_size(shadow_sections[name]['size'])
        print("new addr : %x" % current_addr)
    return (default_sections, shadow_sections, isr_text_rodata_section)


def get_default_data_info(size_output_str):
    data = size_output_str
    default_sections ={}
    for line in data[2:]:# Skip first two lines
        fields = line.split()
        if len(fields) != 3:
            break
        name = fields[0]
        size = int(fields[1])
        if size >= 0:
            if size == 0:
                size = 0
            elif size < 32:
                size = 32
            if name in default_sections:
                default_sections[name]['size'] += size
            else:
                if name in DEFAULT_RAM_SECTIONS or name in DEFAULT_FLASH_SECTIONS or name in DEFAULT_STACK_REGION:
                    default_sections[name] = {'size':size, 'addr':int(fields[2])}
    return default_sections


# def write_linker(src_ld_script_filename, shadow_ld, monitor_ld, text_ld=None):
def write_linker(src_ld_script_filename, shadow_ld, text_ld=None, rodata_ld=None):
    filename = src_ld_script_filename.split('.ld')
    if args.ld_script_num == "2":
        out_ld_script_filename = filename[0] + "_final.ld"
    if args.ld_script_num == "1":
        out_ld_script_filename = filename[0] + "_inter.ld" 
    with open(src_ld_script_filename) as inldfile:
        with open(out_ld_script_filename, 'w') as outldfile:
            for line in inldfile.readlines():
                if 'OLD_TEXT' in line and text_ld:
                    continue
                elif "ALIGNED_TEXT_SECTION" in line and text_ld:
                    outldfile.write(text_ld)
                # elif "OPEC_MONITOR_SECTION" in line:
                #     outldfile.write(monitor_ld)
                elif 'OI_DATA_SECTIONS' in line:
                    outldfile.write(shadow_ld)
                elif "OLD_RODATA" in line and rodata_ld:
                    continue
                elif "ALIGNED_RODATA_SECTION" in line and rodata_ld:
                    outldfile.write(rodata_ld)
                else:
                    outldfile.write(line)


"""
  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(8);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(8);
  } >FLASH
"""

"""
  _Operation_3__data_vma_start = LOADADDR(_Operation_3__data);
  _Operation_3__data 0x2001fb80 :
  {
    _Operation_3__data_start = .;
    *(_Operation_3__data);
    *(_Operation_3__data*);
    . = ALIGN(4);
    _Operation_3__data_end = .;
  } >RAM AT>FLASH
"""


def generate_shadow_section_ld(data_section_name, addr,size):
    data_section = []
    start_var = data_section_name+"_vma_start"
    declare_str =  "%s = LOADADDR(%s);" % (start_var, data_section_name)
    data_section.append("  %s" % declare_str)
    data_section.append("  %s 0x%x :" % (data_section_name, addr))
    data_section.append("  {")
    # data_section.append("\t%s_data_start = .;" % data_section_name)
    data_section.append("    %s_start = .;" % data_section_name)
    # data_section.append("\t. = ALIGN(%i);" %32)
    data_section.append("    *(%s);" % data_section_name)
    data_section.append("    *(%s*);" % data_section_name)
    data_section.append("    . = ALIGN(%i);" %4)
    # data_section_end_var = "%s_data_end" % data_section_name
    data_section_end_var = "%s_end" % data_section_name
    data_section.append("    %s = .;" % data_section_end_var)
    data_section.append("  } >RAM AT>FLASH\n")
    return "\n".join(data_section)


def set_monitor_section(monitor_section_name, addr, size):
    monitor_section = []
    start_var = monitor_section_name + "_vma_start"
    declare_str =  "%s = LOADADDR(%s);" % (start_var, monitor_section_name)
    monitor_section.append("  %s" % declare_str)
    monitor_section.append("  %s 0x%x :" % (monitor_section_name, addr))
    monitor_section.append("  {")
    # monitor_section.append("\t%s_data_start = .;" % monitor_section_name)
    monitor_section.append("    %s_start = .;" % monitor_section_name)
    # monitor_section.append("\t. = ALIGN(%i);" %32)
    monitor_section.append("    *(%s);" % monitor_section_name)
    monitor_section.append("    *(%s*);" % monitor_section_name)
    monitor_section.append("    . = ALIGN(%i);" %4)
    # monitor_section_end_var = "%s_data_end" % monitor_section_name
    monitor_section_end_var = "%s_end" % monitor_section_name
    monitor_section.append("    %s = .;" % monitor_section_end_var)
    monitor_section.append("  } >FLASH\n")
    return "\n".join(monitor_section)


"""
  .text :
  {
    . = ALIGN(8);
    _stext = .;
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(8);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH
"""
def generate_text_section_ld(text_section_name, addr, size):
    text_section = []
    start_var = text_section_name + "_vma_start"
    declare_str =  "%s = LOADADDR(%s);" % (start_var, text_section_name)
    text_section.append("  %s" % declare_str)
    text_section.append("  %s 0x%x :" % (text_section_name, addr))
    text_section.append("  {")
    text_section.append("    . = ALIGN(%i);" % 8)
    # text_section.append("\t%s_data_start = .;" % text_section_name)
    text_section.append("    %s_start = .;" % text_section_name)
    # text_section.append("\t. = ALIGN(%i);" %32)
    text_section.append("    *(%s)" % text_section_name)
    text_section.append("    *(%s*)" % text_section_name)
    text_section.append("    *(.glue_7)")
    text_section.append("    *(.glue_7*)")
    text_section.append("    *(.eh_frame)")
    text_section.append("\n")
    text_section.append("    KEEP (*(.init))")
    text_section.append("    KEEP (*(.fini))")
    text_section.append("\n")
    text_section.append("    . = ALIGN(%i);" % 8)
    text_section.append("    _etext = .;")
    text_section_end_var = "%s_end" % text_section_name
    text_section.append("    %s = .;" % text_section_end_var)
    text_section.append("  } >FLASH\n")
    return "\n".join(text_section)


"""
  .rodata :
  {
    . = ALIGN(8);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(8);
  } >FLASH
"""
def generate_rodata_section_ld(rodata_section_name, rodata_start_addr, rodata_region_size):
    rodata = []
    start_var = rodata_section_name + "_vma_start"
    declare_str =  "%s = LOADADDR(%s);" % (start_var, rodata_section_name)
    rodata.append("  %s" % declare_str)
    rodata.append("  %s 0x%x :" % (rodata_section_name, rodata_start_addr))
    rodata.append("  {")
    rodata.append("    . = ALIGN(%i);" % 8)
    rodata.append("    %s_start = .;" % rodata_section_name)
    rodata.append("    *(%s)" % rodata_section_name)
    rodata.append("    *(%s*)" % rodata_section_name)
    rodata.append("\n")
    rodata.append("    . = ALIGN(%i);" % 8)
    rodata.append("    _endrodata = .;")
    rodata_section_end_var = "%s_end" % rodata_section_name
    rodata.append("    %s = .;" % rodata_section_end_var)
    # rodata.append("    . = ALIGN(%i);" % rodata_region_size)
    rodata.append("  } >FLASH\n")
    return "\n".join(rodata)


def gen_shadow_section_memory_pool_ld(shadow_memory_pool_sections):
    shadow_sections_w_addr = {}
    shadow_str = "\n"
    last_ram_addr = RAM_ADDR + RAM_SIZE
    for section in shadow_memory_pool_sections:
    # for name in sorted(shadow_sections):
        name = section[0]
        size = section[1]
        last_ram_addr = last_ram_addr - size
        addr = last_ram_addr
        shadow_str += generate_shadow_section_ld(name, addr, size) 
        shadow_str += "\n"
        shadow_sections_w_addr[name] = {'addr':addr, 'size':size}
    return shadow_str, shadow_sections_w_addr


def gen_text_rodata_section_ld(isr_text_rodata_section):
    text_ld = "\n"
    text_sec_size = isr_text_rodata_section[".isr_vector"]["size"] + isr_text_rodata_section[".text"]["size"]     # only .text + .isr_vector is executable.为什么.isr_vector也是executable?因为避免MPU对其导致的fragments
    aligned_size = oi_utils.next_power_2(text_sec_size)
    text_ld += generate_text_section_ld(".text", isr_text_rodata_section[".text"]["addr"], aligned_size)
    text_ld += "\n"
    isr_text_rodata_section[".text"]["size"] = aligned_size

    rodata_ld = "\n"
    # rodata_region_size = MPU_size(isr_text_rodata_section[".rodata"]["size"])
    rodata_start_addr = 0x8000000 + aligned_size
    # if rodata_start_addr % rodata_region_size:
    #     rodata_start_addr = upper_align(rodata_start_addr, rodata_region_size)
    rodata_ld += generate_rodata_section_ld(".rodata", rodata_start_addr, isr_text_rodata_section[".rodata"]["size"])
    rodata_ld += "\n"
    
    isr_text_rodata_section[".rodata"]["addr"] = rodata_start_addr
    return text_ld, rodata_ld, isr_text_rodata_section


def get_mpu_rbar(peripherals, peripheral_index, region_id):
    if peripheral_index in range(0, len(peripherals)):
        try:
            peripheral_addr = int(peripherals[peripheral_index][0], 16)
        except TypeError:
            peripheral_addr = peripherals[peripheral_index][0]
        if region_id < REGIONS_NUM:
            return mpu.encode_addr(peripheral_addr, region_id, 1)
        else:
            return mpu.encode_addr(peripheral_addr, 0, 1)
    else:
        return 0


def get_mpu_rasr(peripherals, peripheral_index):
    if peripheral_index in range(0, len(peripherals)):
        try:
            peripheral_size = int(peripherals[peripheral_index][3], 16)
        except TypeError:
            peripheral_size = peripherals[peripheral_index][3]
        return mpu.encode_mpu_attributes(1, 1, 1, 1, 1, 'PERIPHERALS', peripheral_size, 1)
    else:
        return 0


def generate_mpu_configs(src_policy, mpu_config_filename, default_sections, isr_text_rodata_section, new_shadow_sections):
    # 设置aligned text section addr, size
    aligned_text_size = oi_utils.next_power_2(default_sections['.isr_vector']['size']+default_sections['.text']['size'])
    executable_isr_vector_text_addr = 0x8000000
    mpu_config = {}
    #mpu_config["_default_"] = {}
    for name, info in src_policy['Operation'].items():
        #op_index = int(name.split("_")[2])
        # def encode_mpu_attributes(XN,PR,PW,UR,UW,mem_type,size,enable,dis_sub=[]):
        text_base = lower_align(executable_isr_vector_text_addr, upper_bound_value(default_sections['.text']['addr']+default_sections['.text']['size']))
        # monitor_base = lower_align(monitor_aligned_addr, upper_bound_value(monitor_aligned_size))

        # region_0(Flash+SRAM)              :Unpri. RO, Pri. RW, XN
        # region_1(text)                    :Unpri. RO, Pri. RO, X
        # region_2(Stack)                   :Unpri. RW, Pri. RW, XN
        # region_3(Operation Data Region)   :Unpri. RW, Pri. RW, XN

        mpu_config[name] =  {
            # All memory address, No Access
            '_region_0_': {
                'Addrs': mpu.encode_addr(0, 0, 1),
                # 'Attrs': 285605947  # 0x1106003b, unprivileged all address space NA
                'Attrs': 302383163  # 0x1206003b, all address space RO
            },
            # .text
            '_region_1_': { 
                'Addrs': mpu.encode_addr(executable_isr_vector_text_addr, 1, 1),
                'Attrs': mpu.encode_mpu_attributes(0, 1, 0, 1, 0, 'FLASH', isr_text_rodata_section['.text']['size'], 1)
            },
            # # .rodata
            # '_region_2_': { 
            #     'Addrs': mpu.encode_addr(isr_text_rodata_section[".rodata"]["addr"], 2, 1),
            #     'Attrs': mpu.encode_mpu_attributes(1, 1, 0, 1, 0, 'FLASH', isr_text_rodata_section['.rodata']['size'], 1)
            # },
            # .stack
            '_region_2_': {
                'Addrs': mpu.encode_addr(default_sections['.stack']['addr'], 2, 1),
                'Attrs': mpu.encode_mpu_attributes(1, 1, 1, 1, 1, 'INTERNAL_RAM', default_sections['.stack']['size'], 1)
            },
        }

        region_index = 3
        operation_data_region_name = name + "_data"
        if operation_data_region_name in new_shadow_sections.keys():
            mpu_config[name]["_region_%d_"%region_index] = {
                'Addrs': mpu.encode_addr(new_shadow_sections[name+"_data"]['addr'], region_index, 1),
                'Attrs': mpu.encode_mpu_attributes(1, 1, 1, 1, 1,'INTERNAL_RAM', new_shadow_sections[name+"_data"]['size'], 1)
            }
        else:
            # even if there is an operation that does not contain a shadow data region, we currently do not use this spare region
            mpu_config[name]["_region_%d_"%region_index] = {
                'Addrs': 0,
                'Attrs': 0
            }
        region_index += 1


        if len(info["Peripheral_Namelist"]) == 1 and info["Peripheral_Namelist"][0] == "NONE":
            for i in range(region_index, 8):
                mpu_config[name]["_region_%d_"%i] = {
                    'Addrs': 0,
                    'Attrs': 0
                }
        else:
            for i in range(region_index, 8):
                mpu_config[name]["_region_%d_"%i] = {
                    'Addrs': get_mpu_rbar(info["Peripherals"], i-region_index, i),
                    'Attrs': get_mpu_rasr(info["Peripherals"], i-region_index)
                }

    mpu_configs_json = json.dumps(mpu_config, sort_keys=True, indent=4)
    f = open(mpu_config_filename, 'w')
    f.write(mpu_configs_json)
    f.close()
    return mpu_config


def update_policy_mpu_region_info(inter_policy, mpu_config, sections_info):
    """
    Now the operation data sections' addresses are determined
    1) Update the MPU configurations in the policy file.
    2) Generate the operation data sections infomation
    3) Update the memory pool information
    """
    for name in inter_policy['Operation'].keys():
        for region_index in range(0, 8):
            region_name = "_region_%d_" % region_index
            inter_policy['Operation'][name]['MPU_Config']['Addr'][region_index] = mpu_config[name][region_name]['Addrs']
            inter_policy['Operation'][name]['MPU_Config']['Attr'][region_index] = mpu_config[name][region_name]['Attrs']

    inter_policy["OperationDataSectionInfo"] = []
    for name, section_info in sections_info.items():            # name: operation name or memory_pool name, section_info: operation_data section or memory pool section information
        try:
            operation_id = int(name.split("_")[2], 10)          # "_Operation_1__data".split("_") = ['', 'Operation', '1', '', 'data']
            cur_sec_info = [section_info["addr"], section_info["size"], operation_id]
            inter_policy["OperationDataSectionInfo"].append(cur_sec_info)
        except (IndexError, ValueError):                        # "memp_PBUF".split("_") = ['memp', 'PBUF']
                                                                # "memp_PBUF_POOL".split("_") = ['memp', 'PBUF', 'POOL']
            if name.startswith("memp_"):
                inter_policy["MemoryPool"][name.split("memp_")[1]][0] = section_info["addr"]
            else:
                continue

    # add default operation's data section info, default operation's id is ('t'-'0')=
    inter_policy["OperationDataSectionInfo"].append([0, 0, 0x44])
    
    for operation_name, operation_info in inter_policy['Operation'].items():
        # for peripheral in operation_info["Peripherals"]:
        for i in range(len(operation_info["Peripherals"])):
            peripheral = operation_info["Peripherals"][i]
            # Memory_Pool information is appended to operation's peripherals' information
            peripheral_memory_pool_or_name = peripheral[2]
            if peripheral_memory_pool_or_name in inter_policy["MemoryPool"]:
                print(inter_policy["MemoryPool"])
                memory_pool_addr = inter_policy["MemoryPool"][peripheral_memory_pool_or_name][0]
                memory_pool_size = inter_policy["MemoryPool"][peripheral_memory_pool_or_name][3]
                memory_pool_rbar = mpu.encode_addr(memory_pool_addr, 0, 1)
                memory_pool_rasr = mpu.encode_mpu_attributes(1, 1, 1, 1, 1, 'INTERNAL_RAM', memory_pool_size, 1)
                inter_policy['Operation'][operation_name]["Peripherals"][i][0] = memory_pool_addr
                inter_policy['Operation'][operation_name]["Peripherals"][i][4] = memory_pool_rbar
                inter_policy['Operation'][operation_name]["Peripherals"][i][5] = memory_pool_rasr

    final_policy = json.dumps(inter_policy, sort_keys=True, indent=4)

    return final_policy


def write_inter_linker(src_ld_script, policy):
    # monitor_str = "\n"
    # monitor_section = []
    # monitor_section_name = ".opec_monitor"
    # start_var = monitor_section_name + "_vma_start"
    # declare_str = "%s = LOADADDR(%s);" % (start_var, monitor_section_name)
    # monitor_section.append("  %s" %declare_str)
    # monitor_section.append("  %s :" % (monitor_section_name))
    # monitor_section.append("  {")
    # monitor_section.append("    %s_start = .;" % monitor_section_name)
    # monitor_section.append("    *(%s);" % monitor_section_name)
    # monitor_section.append("    *(%s*);" % monitor_section_name)
    # monitor_section.append("    . = ALIGN(%i);" % 4)
    # monitor_section_end_var = "%s_end" % monitor_section_name
    # monitor_section.append("    %s = .;" % monitor_section_end_var)
    # monitor_section.append("  } > FLASH\n\n")
    # monitor_str += "\n".join(monitor_section)

    shadow_str = "\n"
    memory_pool_str = "\n"

    for name in policy["MemoryPool"]:
        memory_pool_section = []
        memory_pool_section_name = "memp_" + name
        start_var = memory_pool_section_name + "_vma_start"
        declare_str = "%s = LOADADDR(%s);" % (start_var, memory_pool_section_name)
        memory_pool_section.append("  %s" % declare_str)
        memory_pool_section.append("  %s :" % (memory_pool_section_name))
        memory_pool_section.append("  {")
        memory_pool_section.append("    %s_start = .;" % memory_pool_section_name)
        memory_pool_section.append("    *(%s);" % memory_pool_section_name)
        memory_pool_section.append("    *(%s*);" % memory_pool_section_name)
        memory_pool_section.append("    . = ALIGN(%i);" %4)
        memory_pool_section_end_var = "%s_end" % memory_pool_section_name
        memory_pool_section.append("    %s = .;" % memory_pool_section_end_var)
        memory_pool_section.append("  } >RAM AT >FLASH\n\n")
        memory_pool_str += "\n".join(memory_pool_section)

    for name in policy['Operation']:
        if len(policy['Operation'][name]["InternalData"]) == 0 and len(policy['Operation'][name]["ExternalData"]) == 0:
            print(name+'has no data section')   
        else:
            data_section = []
            data_section_name = name + '_data'
            start_var = data_section_name + "_vma_start"
            declare_str = "%s = LOADADDR(%s);" % (start_var, data_section_name)
            data_section.append("  %s" % declare_str)
            data_section.append("  %s :" % (data_section_name))
            data_section.append("  {")
            # data_section.append("\t%s_data_start = .;" % data_section_name)
            data_section.append("    %s_start = .;" % data_section_name)
            # data_section.append("\t. = ALIGN(%i);" %32)
            data_section.append("    *(%s);" % data_section_name)
            data_section.append("    *(%s*);" % data_section_name)
            data_section.append("    . = ALIGN(%i);" %4)
            # data_section_end_var = "%s_data_end" % data_section_name
            data_section_end_var = "%s_end" % data_section_name
            data_section.append("    %s = .;" % data_section_end_var)
            data_section.append("  } >RAM AT >FLASH\n\n")
            shadow_str += "\n".join(data_section)
    # print(shadow_str)

    write_linker(args.src_ld_script, memory_pool_str+shadow_str, None, rodata_ld=None)


def add_peripehral_mpu_rbar_rasr(policy):
    for name, info in policy['Operation'].items():
        peripheral_num = len(info["Peripherals"])
        for i in range(peripheral_num):
            if peripheral_num == 1 and info["Peripheral_Namelist"][0] == "NONE":
                info["Peripherals"][i].append(0)
                info["Peripherals"][i].append(0)
                info["Peripherals"][i][0] = int(info["Peripherals"][i][0], 16)
                info["Peripherals"][i][3] = int(info["Peripherals"][i][3], 16)
            else:
                rbar = get_mpu_rbar(peripherals=info["Peripherals"], peripheral_index=i, region_id=i+BASIC_REGIONS)
                rasr = get_mpu_rasr(peripherals=info["Peripherals"], peripheral_index=i)
                info["Peripherals"][i].append(rbar)
                info["Peripherals"][i].append(rasr)
                info["Peripherals"][i][0] = int(info["Peripherals"][i][0], 16)
                info["Peripherals"][i][3] = int(info["Peripherals"][i][3], 16)

        policy['Operation'][name] = info
    return policy


def append_memory_pool_info_to_peripherals(policy):
    for operation_name, info in policy['Operation'].items():
        for memp_ele in info["MemoryPool"]:
            info["Peripherals"].append([
                0,  # start_address
                3,  # read & write
                memp_ele[2],  # name
                memp_ele[3],  # size
                0,  # rbar
                0   # rasr
            ])
            info["Peripheral_Namelist"].append(memp_ele[2])
        policy['Operation'][operation_name] = info
    return policy


def get_aligned_memory_pool_sections(memory_pools):
    aligned_memory_pool_sections = {}
    for memory_pool_name, info in memory_pools.items():
        aligned_memory_pool_sections["memp_"+memory_pool_name] = info[3]
    
    return aligned_memory_pool_sections


def get_sorted_operation_data_sections_and_memory_pool_sections(size_data, aligned_memory_pool_sections):
    operation_sections = {}
    sorted_operation_data_sections = {}
    total_size = 0
    for name, data in size_data.items():
        addr = data['addr']
        aligned_size = oi_utils.next_power_2(data['size'])
        # if addr >= RAM_ADDR  and addr < (RAM_ADDR + RAM_SIZE):
        #     if name not in DEFAULT_RAM_SECTIONS:
        operation_sections[name] = aligned_size
        total_size += aligned_size
        if total_size >= RAM_SIZE:
            print("[!] I'm running out of SRAM! {}".format(name))
    # flash_sorted_sections = sorted(hexbox_flash_sections.items(), key=operator.itemgetter(1),reverse=True)
    for memory_pool_name, aligned_size in aligned_memory_pool_sections.items():
        operation_sections[memory_pool_name] = aligned_size
    sorted_operation_data_sections = sorted(operation_sections.items(), key=operator.itemgetter(1), reverse=True)
    return sorted_operation_data_sections


def move_unactivated_peripheral_region(src_policy):
    for name, info in src_policy['Operation'].items():
        peripheral_num = len(info["Peripheral_Namelist"])
        info["Peripheral_Regions"] = []
        # if peripheral_num == 1 and info["Peripheral_Namelist"][0] == "NONE":
            # info["Peripheral_Regions"].append([0, 3, "0", 0, 0, 0, 0])
        if peripheral_num <= 4:
            info["Peripheral_Regions"].append([0, 3, "0", 0, 0, 0, 0])
        else:
            for i in range(4, peripheral_num):
                info["Peripheral_Regions"].append(info["Peripherals"][i])
        src_policy['Operation'][name] = info
    return src_policy
                

if __name__ == "__main__":
    """ policy generation steps
    0) baseline
    1) intermediate: have shadow sections, not aligned, 获得这些section的size
    2) final: aligned shadow sections, mpu configs
    """
    parser = argparse.ArgumentParser()
    parser.add_argument('-o','--object',dest='object_filename',required=False, help='Object File')
    parser.add_argument('-sp','--srcpolicy',dest='src_policy_file', required = True, help = "Policy file used to derive the permissions for each region")
    parser.add_argument('-m', '--mpuconfig', dest='mpu_config_file', required=False, help = "MPU config output file")
    parser.add_argument('-ld_src','--linker_script',dest='src_ld_script', required=True, help='NULL')
    parser.add_argument('-ld_num','--ld_num',dest='ld_script_num', required=True, help='NULL')
    args = parser.parse_args()

    if args.ld_script_num == STEP_ONE:
        policy_before_insert_rbar_rasr_file = args.src_policy_file.split(".json")[0] + "_before_add_rbar_rasr.json"
        src_policy = json.load(open(args.src_policy_file, 'r+'))
        oi_utils.store_json_to_file(src_policy, policy_before_insert_rbar_rasr_file)
        write_inter_linker(args.src_ld_script, src_policy)
        src_policy = add_peripehral_mpu_rbar_rasr(src_policy)
        src_policy = append_memory_pool_info_to_peripherals(src_policy)
        policy_before_step_two_file = args.src_policy_file.split(".json")[0] + "_before_step_two.json"
        oi_utils.store_json_to_file(src_policy, policy_before_step_two_file)
        # src_policy = move_unactivated_peripheral_region(src_policy)
        oi_utils.store_json_to_file(src_policy, args.src_policy_file)

    if args.ld_script_num == STEP_TWO:
        src_policy = json.load(open(args.src_policy_file, 'r+'))
        (default_sections, shadow_sections, isr_text_rodata_section) = get_section_info(src_policy, readelf(args.object_filename))
        # default_sections = get_default_data_info(readelf(args.object_filename))
        
        ## sort operation shadow data sections & memory pool sections by size, increasing order
        aligned_memory_pool_sections = get_aligned_memory_pool_sections(src_policy["MemoryPool"])
        shadow_and_memorypool_ssections = get_sorted_operation_data_sections_and_memory_pool_sections(shadow_sections, aligned_memory_pool_sections)
        ## 
        shadow_ld, shadow_sections_w_addr = gen_shadow_section_memory_pool_ld(shadow_and_memorypool_ssections)     # shadow_sections: [("operation_name", size)]
        # print(shadow_sections_w_addr)
        text_ld, rodata_ld, isr_text_rodata_section = gen_text_rodata_section_ld(isr_text_rodata_section)              # monitor sections contains .text section to calculate the start addr of monitor section
        write_linker(args.src_ld_script, shadow_ld, text_ld, rodata_ld)
        mpu_config = generate_mpu_configs(src_policy, args.mpu_config_file, default_sections, isr_text_rodata_section, shadow_sections_w_addr)
        src_policy = update_policy_mpu_region_info(src_policy, mpu_config, shadow_sections_w_addr)
        f = open(args.src_policy_file, 'w')
        f.write(src_policy)
        f.close()