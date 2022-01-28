import json
import collections
import logging
import copy
import oi_utils

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

PERIPHERAL_MERGE_LABEL = "__"

def merge_dict(dict1, dict2):
    dict3 = {}
    if (len(dict1.keys()) == 0):
        return dict2
    if (len(dict2.keys()) == 0):
        return dict1
    for key in dict1.keys():
        dict3[key] = dict1[key]
    for key in dict2.keys():
        dict3[key] = dict2[key]
    return dict3


def merge_funcs_to_operation(func_memdep, Operation_Funcs):
    logger.info("Start to merge Functions to an operation...")
    # print(func_memdep)
    operation = {}
    # global operation_func_members
    """
    example operation:
        "key_init": {
            "ExternalData": [
                "key_in",
                "key",
                "PinRxBuffer",
                "pin",
                "K"
            ],
            "Peripherals": []
        }
    """
    for cur_op in Operation_Funcs.keys():
        operation[cur_op] = {}
        # operation_func_members[cur_op] = {}
        # operation_func_members[cur_op][cur_op] = {}
        #operation[cur_op]["ExternalData"] = set()
        #operation[cur_op]["Peripherals"] = set()
        # initialize
        operation[cur_op]["ExternalData"] = set(func_memdep[cur_op]["Variables"])
        operation[cur_op]["Peripherals"] = set(func_memdep[cur_op]["Peripherals"])
        operation[cur_op]["Whitelist"] = func_memdep[cur_op]["Whitelist"]
        operation[cur_op]["Whitelist_Namelist"] = set(func_memdep[cur_op]["Whitelist_Namelist"])
        # operation_func_members[cur_op][cur_op] = func_memdep[cur_op]["Variables"]
        # operation_func_members[cur_op][cur_op] = func_memdep[cur_op]["Peripherals"]
        # operation_func_members[cur_op][cur_op] = func_memdep[cur_op]["Whitelist"]
        # operation_func_members[cur_op][cur_op] = func_memdep[cur_op]["Whitelist_Namelist"]
        for func in Operation_Funcs[cur_op]:
            if func == "HAL_GPIO_WritePin":
                print(func)
            operation[cur_op]["ExternalData"] = operation[cur_op]["ExternalData"].union(set(func_memdep[func]["Variables"]))
            ## add caller arguments
            print(func_memdep[func].keys())
            print(func_memdep[func])
            if "caller" in func_memdep[func].keys():
                for caller in func_memdep[func]["caller"].keys():
                    if caller in Operation_Funcs[cur_op]:
                        # if caller also in Operation, add the peripherals to operation
                        operation[cur_op]["Peripherals"] = operation[cur_op]["Peripherals"].union(set(func_memdep[func]["caller"][caller]["Peripherals"]))
                        operation[cur_op]["Whitelist"] = merge_dict(func_memdep[func]["caller"][caller]["Whitelist"], operation[cur_op]["Whitelist"])
                        operation[cur_op]["Whitelist_Namelist"] = operation[cur_op]["Whitelist_Namelist"].union(set(func_memdep[func]["caller"][caller]["Whitelist_Namelist"]))
            operation[cur_op]["Peripherals"] = operation[cur_op]["Peripherals"].union(set(func_memdep[func]["Peripherals"]))
            operation[cur_op]["Whitelist"] = merge_dict(operation[cur_op]["Whitelist"], func_memdep[func]["Whitelist"])
            operation[cur_op]["Whitelist_Namelist"] = operation[cur_op]["Whitelist_Namelist"].union(set(func_memdep[func]["Whitelist_Namelist"]))
            # operation_func_members[cur_op][func] = {'ExternalData':func_memdep[func]["Variables"], 'Peripherals': func_memdep[func]["Peripherals"], 'Whitelist': func_memdep[func]["Whitelist"], 'Whitelist_Namelist':func_memdep[func]["Whitelist_Namelist"]}

    key_list = operation.keys()
    all_external_data = []
    operation_w_nonshareable = {}
    for cur_op in key_list:
        operation_w_nonshareable[cur_op] = {}
        operation_w_nonshareable[cur_op]["ExternalData"] = list(operation[cur_op]["ExternalData"])
        # operation_w_nonshareable[cur_op]["Peripherals"] = list(operation[cur_op]["Peripherals"])
        all_external_data += operation_w_nonshareable[cur_op]["ExternalData"]
    logger.info("Finish merging Functions to an operation!")

    # oi_utils.store_json_to_file(operation_w_nonshareable, app_policy_w_nonshareable_json)
    # logger.info("Finish dump operation with no-sharing Variables!")

    logger.info("Start to remove no-sharing Variables...")

    external_data_counter = collections.Counter(all_external_data)
    try:
        for cur_op in key_list:
            external_gv_set = set()
            internal_gv_set = set()
            for external_data in operation[cur_op]["ExternalData"]:
                if external_data_counter[external_data] > 1:
                    external_gv_set.add(external_data)
                elif external_data_counter[external_data] == 1:
                    internal_gv_set.add(external_data)
            operation[cur_op]["ExternalData"] = external_gv_set
            operation[cur_op]["InternalData"] = internal_gv_set
        logger.info("Finshing removing no-sharing Variables!")
    except Exception as e:
        logger.error(e)
        traceback.print_exc()

    return operation


def add_default_operation(operation):
    operation["main"] = {}
    operation["main"]["ExternalData"] = []
    operation["main"]["InternalData"] = []
    operation["main"]["Peripherals"] = []
    operation["main"]["Whitelist"] = {}
    operation["main"]["Whitelist"]["None"] = [0,0,0]
    operation["main"]["Whitelist_Namelist"] = ["None"]
    operation["main"]["ExternalDataFields"] = {}
    operation["main"]["ExternalDataSan"] = {}
    return operation


def add_none_whitelist(operation):
    for cur_op in operation.keys():
        if len(operation[cur_op]["Whitelist_Namelist"]) == 0 and len(operation[cur_op]["Whitelist"].keys()) == 0:
            operation[cur_op]["Whitelist_Namelist"].append("None")
            operation[cur_op]["Whitelist"]["None"] = [0,0,0]
    return operation


def extract_gv_remove_size(gv_list):
    new_gv_list = []
    for gv in gv_list:
        new_gv_list.append(gv[0])
    return new_gv_list


def generate_init_policy(entry_info, operation, memory_pools_info):
    init_policy = {
        "Operation": {},
        "MemoryPool": {}
        }
    operation_index = 0
    for cur_op in operation.keys():
        operation[cur_op]["Entry"] = cur_op
        operation[cur_op]["MPU_Config"] = {
            "Addr": [0,0,0,0,0,0,0,0],
            "Attr": [0,0,0,0,0,0,0,0]
            }
        if "main" in cur_op:
            operation[cur_op]["Stack"] = []
            operation[cur_op]["Sanitization"] = []
            init_policy["Operation"]["_default_"] = operation[cur_op]
        else:
            if entry_info[cur_op] == {}:
                operation[cur_op]["Stack"] = []
                operation[cur_op]["Sanitization"] = []
            else:
                operation[cur_op]["Stack"] = entry_info[cur_op]["Stack"]
                operation[cur_op]["Sanitization"] = entry_info[cur_op]["Sanitization"]
            init_policy["Operation"]["_Operation_"+str(operation_index)+"_"] = operation[cur_op]
            operation_index += 1
            

    for operation in init_policy["Operation"].keys():
        internal_data = init_policy["Operation"][operation]["InternalData"]
        for global_variable in internal_data:
            if global_variable.startswith("switch.table"):
                init_policy["Operation"][operation]["InternalData"].remove(global_variable)

        external_data = init_policy["Operation"][operation]["ExternalData"]
        for global_variable in external_data:
            if global_variable.startswith("switch.table"):
                init_policy["Operation"][operation]["ExternalData"].remove(global_variable)

    for memp_name, memp_info in memory_pools_info.items():
        init_policy["MemoryPool"][memp_name] = [
            "0xdeadbeef",       # temporary start address of the memory pool
            3,
            memp_name,
            memp_info["aligned_size"]   # aligned size of the memory pool
        ]

    return init_policy


def peripheral_merge_for_GPIOs(peripherals):
    namelist = []
    merged_GPIOs_peripherals = []
    GPIOA_H_start = "0x40020000"
    GPIOA_H_len   = "0x2000"
    GPIOA_H_name  = "GPIOA_H"
    GPIOA_H_attr  = 3
    A_H = "ABCDEFGH"
    SUM = ""
    for peri in peripherals:
        namelist.append(peri[2])
    merge_GPIOs = False
    for i in A_H:
        if "GPIO"+i in namelist:
            SUM = SUM + i
    if SUM == A_H:
        merge_GPIOs = True

    if merge_GPIOs:
        for peri in peripherals:
            if peri[2].startswith("GPIO") and peri[2][-1] in A_H:
                continue
            else:
                merged_GPIOs_peripherals.append(peri)
        merged_GPIOs_peripherals.append([GPIOA_H_start, GPIOA_H_attr, GPIOA_H_name, GPIOA_H_len])
        return merged_GPIOs_peripherals
    else:
        return peripherals


def merge_adjacent_peripherals(init_policy):
    """
    for each operation, merge the adjacent peripherals to save MPU regions
    if the start address does not align to the new merged peripheral, abandon the merge
    """
    for operation_name, policy in init_policy["Operation"].items():
        peripherals = policy["Peripherals"]
        peri_num = len(peripherals)
        if peri_num == 0:
            peripherals_merged = []

        elif peri_num == 1:
            peripherals_merged = peripherals

        else:
            peripherals.sort(key=oi_utils.take_first)
            peripherals = peripheral_merge_for_GPIOs(peripherals)
            peri_num = len(peripherals)     # update peripherals num
            peripherals_merged = []

            # initialize to find adjacent peripherals
            
            cur_addr = int(peripherals[0][0], 16)
            cur_attr = peripherals[0][1]
            cur_name = peripherals[0][2]
            cur_len = int(peripherals[0][3], 16)

            for i in range(1, peri_num):
                next_addr = int(peripherals[i][0], 16)
                next_attr = peripherals[i][1]
                next_name = peripherals[i][2]
                next_len = int(peripherals[i][3], 16)
                if (cur_addr + cur_len == next_addr) and ((cur_addr % (cur_len + next_len)) == 0):
                    # check whether the merged peripheral start address align to the size
                    # merge the two peripherals by directly add the len
                    cur_len += next_len
                    cur_name = cur_name + PERIPHERAL_MERGE_LABEL + next_name
                else:
                    # not adjacent or not aligned, save current peripheral to policy
                    peripherals_merged.append([hex(cur_addr), "rw", cur_name, hex(cur_len)])
                    cur_addr = next_addr
                    cur_attr = next_attr
                    cur_name = next_name
                    cur_len = next_len
                # add the final current peripheral
            peripherals_merged.append([hex(cur_addr), "rw", cur_name, hex(cur_len)])
        init_policy["Operation"][operation_name]["Peripherals"] = peripherals_merged
        
    return init_policy


def sort_external_global_data(init_policy, global_var_dict_wth_size_fields):
    for operation_name, policy in init_policy["Operation"].items():
        external_data = policy["ExternalData"]
        external_data_size = []
        external_data_fields = {}       # which fields of a struct global variable is pointer
        for data in external_data:
            external_data_size.append([data, global_var_dict_wth_size_fields[data]["size"]])
            external_data_fields[data] = global_var_dict_wth_size_fields[data]["PointerField"]
        external_data_size.sort(key=oi_utils.take_second, reverse=True)
        external_data = extract_gv_remove_size(external_data_size)
        # external_data = external_data_size.keys()
        init_policy["Operation"][operation_name]["ExternalData"] = external_data
        init_policy["Operation"][operation_name]["ExternalDataFields"] = external_data_fields
    return init_policy


def init_sanitization_info(init_policy):
    """
    Currently OPEC supports 32 bit value check, i.e., sanitization
    """
    for operation_name, policy in init_policy["Operation"].items():
        external_data_san = {}
        external_data = policy["ExternalData"]
        for data in external_data:
            external_data_san[data] = [0,   # if needs sanitization. 1 need, 0 not need
                                       0,   # offset
                                       0,   # min_value
                                       0]   # max_value
        init_policy["Operation"][operation_name]["ExternalDataSan"] = external_data_san
    return init_policy

def generate_peripheral_whitelist(final_policy):
    """
    for operations whose number of accessible peripehrals exceed four, move extra peripherals to the Peripheral Whitelist
    """
    for operation_name, policy in final_policy["Operation"].items():
        peripherals =  policy["Peripherals"]
        peri_num = len(peripherals)
        policy["Peripheral_Whitelist"] = {}
        policy["Peripheral_Whilelist_Namelist"] = []
        if peri_num <= 4:
            policy["Peripheral_Whitelist"]["None"] = [0,0,0]
            policy["Peripheral_Whilelist_Namelist"].append("None")
        else:
            _peripherals = copy.deepcopy(peripherals)
            for i in range(4, peri_num):
                peri_addr = int(_peripherals[i][0], 16)
                peri_attr = _peripherals[i][1]
                peri_name = _peripherals[i][2]
                peri_size = int(_peripherals[i][3], 16)
                policy["Peripheral_Whitelist"][peri_name] = [peri_addr, peri_size, 3]
                policy["Peripheral_Whilelist_Namelist"].append(peri_name)
                peripherals.remove(_peripherals[i])
            policy["Peripherals"] = peripherals
        final_policy["Operation"][operation_name] = policy
    return final_policy


def generate_peripheral_namelist(final_policy):
    """
    add peripheral namelist to final_policy
    """
    for operation_name, policy in final_policy["Operation"].items():
        peripherals = policy["Peripherals"]
        policy["Peripheral_Namelist"] = []
        peri_num = len(peripherals)
        if peri_num == 0:
            policy["Peripheral_Namelist"].append("NONE")
            policy["Peripherals"].append(["0", 3, "0", "0"])
            pass
        else:
            for i in range(len(peripherals)):
                peri_name = policy["Peripherals"][i][2]
                policy["Peripheral_Namelist"].append(peri_name)
                peri = list(policy["Peripherals"][i])
                peri[1] = 3
                policy["Peripherals"][i] = peri
        final_policy["Operation"][operation_name] = policy

    return final_policy


def initialize_memory_pool(operation):
    for operation_name, operation_info in operation.items():
        operation_info["MemoryPoolNames"] = []
        operation_info["MemoryPool"] = []
        operation_info["MemoryHeap"] = []
        operation[operation_name] = operation_info
    return operation 


"""
memp_memory_UDP_PCB_base
memp_tab_PBUF_POOL
ram_heap
"""
def get_memory_pool_name(gv_name):
    label_memory = "memp_memory_"
    label_base = "_base"
    label_tab = "memp_tab_"
    memory_pool_name = None
    if gv_name.startswith(label_memory) and gv_name.endswith(label_base):
        memory_pool_name = gv_name.split(label_memory)[1].split(label_base)[0].strip()
    elif gv_name.startswith(label_tab):
        memory_pool_name = gv_name.split(label_tab)[1].strip()
    elif gv_name in ["ram_heap", "ram", "ram_end", "lfree"]:
        memory_pool_name = "MEMHEAP"
    elif gv_name in ["DMARxDscrTab", "DMATxDscrTab", "Tx_Buff", "Rx_Buff"]:
        memory_pool_name = "ETHDMA"
    elif gv_name in ["results"]:
        memory_pool_name = "CoreMarkResults"
    elif gv_name in ["stack_memblock"]:
        memory_pool_name = "StackMemoryBlock"
    else:
        memory_pool_name = None

    return memory_pool_name


def add_memory_pool_info(operation_info, var_name):
    memory_pool_name = get_memory_pool_name(var_name)
    operation_info["MemoryPoolNames"].append(memory_pool_name)
    if memory_pool_name == None:
        print(var_name)
    return operation_info


def replace_memory_pool_vars(operation, memory_pool_vars_list, memory_pools_info):
    for operation_name, operation_info in operation.items():
        internal_data           = []
        external_data           = []
        # external_data_fields    = {}

        for i_data in operation_info["InternalData"]:
            if i_data in memory_pool_vars_list:
                # add memory_pool_info
                operation_info = add_memory_pool_info(operation_info, i_data)
                continue

            internal_data.append(i_data)
        operation_info["InternalData"] = internal_data

        for e_data in operation_info["ExternalData"]:
            if e_data in memory_pool_vars_list:
                # add memory_pool_info
                operation_info = add_memory_pool_info(operation_info, e_data)
                continue

            external_data.append(e_data)
            # external_data_fields[e_data] = operation_info["ExternalDataFields"][e_data]
        operation_info["ExternalData"] = external_data
        # operation_info["ExternalDataFields"] = external_data_fields

        # operation_info["MemoryPool"] = list(set(operation_info["MemoryPool"]))
        for i in range(len(operation_info["MemoryPoolNames"])):
            memory_pool_name = operation_info["MemoryPoolNames"][i]
            aligned_size = memory_pools_info[memory_pool_name]["aligned_size"]
            operation_info["MemoryPool"].append(["0xdeadbeef", 3, memory_pool_name, aligned_size])
        operation[operation_name] = operation_info

    return operation


"""
{
	"FRAG_PBUF" : 
	{
		"memp_memory_FRAG_PBUF_base" : 363,
		"memp_tab_FRAG_PBUF" : 4
	}
}
"""
def calc_aligned_size_for_memory_pool(memory_pool):
    for name, info in memory_pool.items():
        aligned_size = 0
        for var, var_size in info.items():
            aligned_size += var_size
        aligned_size = oi_utils.next_power_2(aligned_size)
        print(name, aligned_size)
        info["aligned_size"] = aligned_size

        memory_pool[name] = info
    # sort the memory pools based on the aligned_size they need
    # sorted(memory_pool.items(), key=lambda kv:(kv[1]["aligned_size"], kv[0]), reverse=True)

    return memory_pool


def get_memory_pool_vars_list(memory_pool):
    pool_vars_list = []
    for pool_name, vars_info in memory_pool.items():
        for var_name in vars_info.keys():
            if var_name != "aligned_size":
                pool_vars_list.append(var_name)
    return pool_vars_list