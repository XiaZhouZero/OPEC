import traceback
import logging
import PeripheralDef
import json

store_inst_pattern = " store "
load_inst_pattern = " load "
getelementptr_inst_pattern = "getelementptr"
function_pattern = "$FunctionName:"
primary_peripheral_pattern = "inttoptr"
function_call_pattern = "call"


llvm_peripheral_start = "LLVM PERIPHERAL ANALYSIS"
llvm_peripheral_func_start = "[*] Function"
llvm_peripheral_callee_start = "[*] Call function"
llvm_peripheral_callee_end = "[!] Callee end"
llvm_peripheral_addr_start = "[+] Int"

llvm_def_use_pattern = "LLVM DEF-USE ANALYSIS"
global_variable_records_start = "ALL GLOBALVARIABLES"

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

def extract_ppb_reg_list(ppb_reg_list):
    # [hex(ph['Base_Addr']),"rw",ph['NAME_KEY'],hex(ph_range)] 
    ppb_reg_attr = []
    ppb_reg_attr.append(int(ppb_reg_list[0], 16))  # base_addr
    ppb_reg_attr.append(int(ppb_reg_list[3], 16))  # size
    ppb_reg_attr.append(3)                              # attr. 0 for No Access, 1 for Read-Only, 2 for Write-Only, 3 for RW
    ppb_reg_name = ppb_reg_list[2]
    return ppb_reg_name, ppb_reg_attr


def add_funcmemdep(func_memdep, func_name):
    if func_name not in func_memdep.keys():
        func_memdep[func_name] = {}
    if "Variables" not in func_memdep[func_name].keys():
        func_memdep[func_name]["Variables"] = set()
    if "Peripherals" not in func_memdep[func_name].keys():
        func_memdep[func_name]["Peripherals"] = set()
    if "Whitelist" not in func_memdep[func_name].keys():
        func_memdep[func_name]["Whitelist"] = {}
    if "Whitelist_Namelist" not in func_memdep[func_name].keys():
        func_memdep[func_name]["Whitelist_Namelist"] = []
    return func_memdep


def add_peripheral_to_func(func_memdep, func_name, device_addr_int):
    func_memdep = add_funcmemdep(func_memdep, func_name)
    peripherals_name = PeripheralDef.get_peripherals_name(device_addr_int)
    ppb_reg_list = PeripheralDef.get_ppb_name(device_addr_int)
    if peripherals_name:
        func_memdep[func_name]["Peripherals"].add(peripherals_name)
    elif ppb_reg_list:
        ppb_reg_name, ppb_reg_attr = extract_ppb_reg_list(ppb_reg_list)
        func_memdep[func_name]["Whitelist"][ppb_reg_name] = ppb_reg_attr
        func_memdep[func_name]["Whitelist_Namelist"].append(ppb_reg_name)
    return func_memdep


def parse_defuse(func_memdep, defuse_records_file):
    Start_LLVM_DEF_USE = False
    cur_func = None
    with open(defuse_records_file, "r") as defuse_file:
        for line in defuse_file.readlines():
            if llvm_def_use_pattern in line:
                Start_LLVM_DEF_USE = not Start_LLVM_DEF_USE
                continue
            
            if Start_LLVM_DEF_USE:
                cur_func = line.split(":")[1].strip()
                gv = line.split(":")[0].split("]")[1].strip()
                if cur_func not in func_memdep.keys():
                    add_funcmemdep(func_memdep, cur_func)   # add function key in func_memedep dict
                func_memdep[cur_func]["Variables"].add(gv)
    return func_memdep


def parse_peripheral(peripheral_records_file, func_memdep={}):
    """
    parse the app_def_use.output file to extract the peripherals each func access
    return:
    {
        "func_name": 
        {
            "peripheral" = set()
            "ppb" = set()
        },
    }
    """
    Start_Peripheral = False
    cur_func = None
    callee_func = None
    is_in_a_callee = False
    with open(peripheral_records_file, "r") as pf:
        lines = pf.readlines()
        line_len = len(lines)

        for line_num in range(line_len):
            line = lines[line_num]
            if llvm_peripheral_start in line:
                Start_Peripheral = not Start_Peripheral

            if not Start_Peripheral:
                continue

            if line.startswith(llvm_peripheral_func_start):
                cur_func = line.split(":")[1].strip()
                func_memdep = add_funcmemdep(func_memdep, cur_func) # init cur_func in func_memdep

            if line.startswith(llvm_peripheral_callee_end):
                is_in_a_callee = False
                continue

            if line.startswith(llvm_peripheral_callee_start):
                """
                [*] Function: HAL_UART_MspInit      <== line_prev
                [*] Call function: HAL_GPIO_Init    <== line
                [+] Int: 1073872896                 <== line_next_1
                [+] Type: %struct.GPIO_TypeDef*
                [+] Addr: 1073872896
                """
                is_in_a_callee = True
                callee_func = line.split(":")[1].strip()
                continue

            if is_in_a_callee:
                line_next_1 = line
                if line_next_1.startswith(llvm_peripheral_addr_start):
                    peripheral_address_int = int(line_next_1.split(":")[1].strip())
                    func_memdep = add_funcmemdep(func_memdep, callee_func)  # init callee in func_memdep
                    func_memdep = add_peripheral_to_func(func_memdep, callee_func, peripheral_address_int)
                else:
                    # [*] Function: handle_unlock_input   <== line
                    # [*] Call function: print            <== line_next_1
                    # [*] Call function: print            <== line_next_2
                    # [*] Call function: mbedtls_sha256
                    # [*] Call function: mbedtls_sha256
                    # [*] Call function: print
                    # [*] Call function: print
                    # [*] Call function: print
                    # [*] Call function: print
                    continue

            if not is_in_a_callee and line.startswith(llvm_peripheral_addr_start):
                peripheral_address_int = int(line.split(":")[1].strip())
                func_memdep = add_peripheral_to_func(func_memdep, cur_func, peripheral_address_int)

    return func_memdep


def add_peripehral_to_func(func_memdep, func, peripherals=None):
    if peripherals == None:
        func_memdep = add_funcmemdep(func_memdep, func)
    else:
        for ph in peripherals:
            func_memdep = add_peripheral_to_func(func_memdep, func, int(ph))
    return func_memdep


def add_peripheral_to_func_condition(func_memdep, caller_name, callee_name, device_addr_int):
    """
    used when generating operations. if the caller is also in the operation sub functions, the peripheral will be added to the operation dependency.
    """
    func_memdep = add_funcmemdep(func_memdep, callee_name)
    peripherals_name = PeripheralDef.get_peripherals_name(device_addr_int)
    ppb_reg_list = PeripheralDef.get_ppb_name(device_addr_int)
    if "caller" not in func_memdep[callee_name].keys():
        func_memdep[callee_name]["caller"] = {}

    if caller_name not in func_memdep[callee_name]["caller"].keys():
        func_memdep[callee_name]["caller"][caller_name] = {}
        func_memdep[callee_name]["caller"][caller_name]["Peripherals"] = []
        func_memdep[callee_name]["caller"][caller_name]["Whitelist"] = {}
        func_memdep[callee_name]["caller"][caller_name]["Whitelist_Namelist"] = []

    if peripherals_name:
        func_memdep[callee_name]["caller"][caller_name]["Peripherals"].append(peripherals_name)

    elif ppb_reg_list:
        ppb_reg_name, ppb_reg_attr = extract_ppb_reg_list(ppb_reg_list)
        func_memdep[callee_name]["caller"][caller_name]["Whitelist"][ppb_reg_name] = ppb_reg_attr
        func_memdep[callee_name]["caller"][caller_name]["Whitelist_Namelist"].append(ppb_reg_name)
    
    return func_memdep


def add_couple_peripheral(init_policy, couple_peripheral):
    for operation_name, dependency in init_policy["Operation"].items():
        peripherals = dependency["Peripherals"]
        couple_peripherals = []
        for peri in peripherals:
            peri_name = peri[2]
            if peri_name in couple_peripheral.keys():
                couple_peri_start_addr = couple_peripheral[peri_name]
                couple_peripherals.append(PeripheralDef.get_peripherals_name(couple_peri_start_addr))
        peripherals.extend(couple_peripherals)
        init_policy["Operation"][operation_name]["Peripherals"] = peripherals
    return init_policy


def add_peripheral_dependency(peripheral_json, func_memdep):
    for func in peripheral_json.keys():
        dependency = peripheral_json[func]
        if len(dependency.keys()) > 1:
            for callee in dependency.keys():
                if callee == "addr":
                    # target peripherals that directly used by func, otherwise, the targets are arguments of callee
                    func_memdep = add_peripehral_to_func(func_memdep, func, dependency["addr"])
                else:
                    peripherals = peripheral_json[func][callee]
                    for ph in peripherals:
                        func_memdep = add_peripheral_to_func_condition(func_memdep, func, callee, int(ph))

        if len(dependency.keys()) == 1:
            peripherals = peripheral_json[func]["addr"]
            if peripherals == None:
                func_memdep = add_funcmemdep(func_memdep, func)
                continue

            for ph in peripherals:
                func_memdep = add_peripheral_to_func(func_memdep, func, int(ph))

    return func_memdep


def parse_load_store_inst(load_store_filename):
    """
    Parse load/store IR instructions. Need to handle the offset in getelementptr instructions
    """
    func_memdep = {}
    Start_LLVM_DEF_USE = False
    with open(load_store_filename, "r") as load_store_reader:
        lines = load_store_reader.readlines()
        for line in lines:
            try:
                if function_pattern in line:
                    # a new function
                    cur_func = line.split(" ")[1].strip()
                    if cur_func == "NVIC_SetPriority":
                       print("TARGET FOUND")
                    func_memdep[cur_func] = {}
                    func_memdep[cur_func]["Variables"] = set()
                    func_memdep[cur_func]["Peripherals"] = set()
                    func_memdep[cur_func]["Whitelist"] = {}
                    func_memdep[cur_func]["Whitelist_Namelist"] = []

                elif (load_inst_pattern in line) and (getelementptr_inst_pattern not in line):
                    """
                    %0 = load i8*, i8** %str.addr, align 4, !dbg !2056
                    %58 = load void (%struct.__DMA_HandleTypeDef*)*, void (%struct.__DMA_HandleTypeDef*)** %XferAbortCallback88, align 4, !dbg !2052
                    """
                    if primary_peripheral_pattern in line:
                        """
                        %0 = load volatile i8, i8* inttoptr (i32 1073888276 to i8*), align 1, !dbg !1885
                        """
                        target = line.split(primary_peripheral_pattern)[1].strip()[1:-1].split(" ")[1]
                        if "add" not in target:
                            peripheral_address_int = int(target, 10)
                            peripherals_name = PeripheralDef.get_peripherals_name(peripheral_address_int)
                            ppb_reg_list = PeripheralDef.get_ppb_name(peripheral_address_int)
                            if (peripherals_name):
                                func_memdep[cur_func]["Peripherals"].add(peripherals_name)
                            elif(ppb_reg_list):
                                ppb_reg_name, ppb_reg_attr = extract_ppb_reg_list(ppb_reg_list)
                                func_memdep[cur_func]["Whitelist"][ppb_reg_name] = ppb_reg_attr
                                func_memdep[cur_func]["Whitelist_Namelist"].append(ppb_reg_name)
                    else:
                        target = line.split(", ")[1].split(" ")[-1].strip()
                        #func_memdep[cur_func].add(target)
                        func_memdep[cur_func]["Variables"].add(target)

                elif (load_inst_pattern in line) and (getelementptr_inst_pattern in line):
                    if primary_peripheral_pattern in line:
                        """
                        %0 = load volatile i32, i32* getelementptr inbounds (%struct.RCC_TypeDef, %struct.RCC_TypeDef* inttoptr (i32 1073887232 to %struct.RCC_TypeDef*), i32 0, i32 2), align 4, !dbg !3892
                        """
                        # peripheral access
                        target = line.split(primary_peripheral_pattern)[1].strip()[1:-1].split(" ")[1]
                        peripheral_address_int = int(target, 10)
                        peripherals_name = PeripheralDef.get_peripherals_name(peripheral_address_int)
                        ppb_reg_list = PeripheralDef.get_ppb_name(peripheral_address_int)
                        if (peripherals_name):
                            func_memdep[cur_func]["Peripherals"].add(peripherals_name)
                        elif(ppb_reg_list):
                            ppb_reg_name, ppb_reg_attr = extract_ppb_reg_list(ppb_reg_list)
                            func_memdep[cur_func]["Whitelist"][ppb_reg_name] = ppb_reg_attr
                            func_memdep[cur_func]["Whitelist_Namelist"].append(ppb_reg_name)
                    else:
                        """
                        %0 = load i8*, i8** getelementptr inbounds ([38 x i8*], [38 x i8*]* @print_info, i32 0, i32 1), align 4, !dbg !4503
                        %4 = load i32, i32* getelementptr inbounds (%struct.cmb_hard_fault_regs, %struct.cmb_hard_fault_regs* @regs, i32 0, i32 0, i32 6), align 4, !dbg !4533
                        """
                        target = line.split(", ")[2].strip().split(" ")[-1]
                        func_memdep[cur_func]["Variables"].add(target)

                elif (store_inst_pattern in line) and (getelementptr_inst_pattern not in line):
                    """
                    store i32 %c, i32* %c.addr, align 4
                    store void (%struct.__SPI_HandleTypeDef*)* @SPI_RxISR_8BIT, void (%struct.__SPI_HandleTypeDef*)** %RxISR27, align 4, !dbg !1993
                    """
                    if primary_peripheral_pattern in line:
                        """
                        store volatile i8 %1, i8* inttoptr (i32 1073888277 to i8*), align 1, !dbg !1895
                        """
                        target = line.split(primary_peripheral_pattern)[1].strip()[1:-1].split(" ")[1]
                        peripheral_address_int = int(target, 10)
                        # peripheral_address_hex = hex(int(target.split(peripheral_pattern)[1].strip()[1:-1].split(" ")[1], 10))
                        peripherals_name = PeripheralDef.get_peripherals_name(peripheral_address_int)
                        ppb_reg_list = PeripheralDef.get_ppb_name(peripheral_address_int)
                        if (peripherals_name):
                            func_memdep[cur_func]["Peripherals"].add(peripherals_name)
                        elif(ppb_reg_list):
                            ppb_reg_name, ppb_reg_attr = extract_ppb_reg_list(ppb_reg_list)
                            func_memdep[cur_func]["Whitelist"][ppb_reg_name] = ppb_reg_attr
                            func_memdep[cur_func]["Whitelist_Namelist"].append(ppb_reg_name)
                    else:
                        target = line.split(", ")[1].split(" ")[-1].strip()
                        func_memdep[cur_func]["Variables"].add(target)

                elif (store_inst_pattern in line) and (getelementptr_inst_pattern in line):
                    """
                    store volatile i32 %or29, i32* getelementptr inbounds (%struct.RCC_TypeDef, %struct.RCC_TypeDef* inttoptr (i32 1073887232 to %struct.RCC_TypeDef*), i32 0, i32 11), align 4, !dbg !4021
                    store i32 %24, i32* getelementptr inbounds (%struct.cmb_hard_fault_regs, %struct.cmb_hard_fault_regs* @regs, i32 0, i32 0, i32 0), align 4, !dbg !4730
                    """
                    if primary_peripheral_pattern in line:
                        # try:
                        #     _target = line.split(", ")[2].strip()
                        #     if not primary_peripheral_pattern in _target:
                        #         """
                        #         exclude this case:
                        #         "store %struct.USART_TypeDef* inttoptr (i32 1073759232 to %struct.USART_TypeDef*), %struct.USART_TypeDef** getelementptr inbounds (%struct.UART_HandleTypeDef, %struct.UART_HandleTypeDef* @UartHandle, i32 0, i32 0), align 4, !dbg !1898"
                        #         value `1073759232` which is %struct.USART_TypeDef* type is stored at a field in @UartHandle
                        #         """
                        #         continue
                        #     peripheral_address = _target.split(primary_peripheral_pattern)[1].strip()[1:-1].split(" ")[1]
                        #     if re.findall(r"\-\d", peripheral_address):
                        #         print(hex(int(peripheral_address, 10)))
                        #     else:
                        #         print(peripheral_address)
                        # except IndexError:
                        #     traceback.print_exc()
                        target = line.split(primary_peripheral_pattern)[1].strip()[1:-1].split(" ")[1]
                        peripheral_address_int = int(target, 10)
                        peripherals_name = PeripheralDef.get_peripherals_name(peripheral_address_int)
                        ppb_reg_list = PeripheralDef.get_ppb_name(peripheral_address_int)
                        if (peripherals_name):
                            func_memdep[cur_func]["Peripherals"].add(peripherals_name)
                        elif(ppb_reg_list):
                            ppb_reg_name, ppb_reg_attr = extract_ppb_reg_list(ppb_reg_list)
                            func_memdep[cur_func]["Whitelist"][ppb_reg_name] = ppb_reg_attr
                            func_memdep[cur_func]["Whitelist_Namelist"].append(ppb_reg_name)
                    else:
                        target = line.split(", ")[2].strip().split(" ")[1].strip()
                        func_memdep[cur_func]["Variables"].add(target)

                elif (getelementptr_inst_pattern in line) and (primary_peripheral_pattern in line) and ((load_inst_pattern not in line) and (store_inst_pattern not in line)):
                    """
                    getelementptr only calculates the target address without running load/store the value to this address
                    %arrayidx = getelementptr inbounds [16 x i8], [16 x i8]* @APBAHBPrescTable, i32 0, i32 %shr, !dbg !3965
                    """
                    target = line.split(primary_peripheral_pattern)[1].split()
                    if target[0][1:] == 'i32':
                        target = target[1]
                        peripheral_address_int = int(target, 10)
                        peripherals_name = PeripheralDef.get_peripherals_name(peripheral_address_int)
                        ppb_reg_list = PeripheralDef.get_ppb_name(peripheral_address_int)
                        if (peripherals_name):
                            func_memdep[cur_func]["Peripherals"].add(peripherals_name)
                        elif(ppb_reg_list):
                            ppb_reg_name, ppb_reg_attr = extract_ppb_reg_list(ppb_reg_list)
                            func_memdep[cur_func]["Whitelist"][ppb_reg_name] = ppb_reg_attr
                            func_memdep[cur_func]["Whitelist_Namelist"].append(ppb_reg_name)
                    else:
                        logger.warning("Unknown cases: {}".format(line))


                elif (primary_peripheral_pattern in line) and (function_call_pattern in line):
                    target = line.split(primary_peripheral_pattern)[1].split()
                    if target[0][1:] == 'i32':
                        target = target[1]
                        peripheral_address_int = int(target, 10)
                        peripherals_name = PeripheralDef.get_peripherals_name(peripheral_address_int)
                        ppb_reg_list = PeripheralDef.get_ppb_name(peripheral_address_int)
                        # if (peripherals_name):
                            # func_memdep[cur_func]["Peripherals"].add(peripherals_name)
                        if(ppb_reg_list):
                            ppb_reg_name, ppb_reg_attr = extract_ppb_reg_list(ppb_reg_list)
                            func_memdep[cur_func]["Whitelist"][ppb_reg_name] = ppb_reg_attr
                            func_memdep[cur_func]["Whitelist_Namelist"].append(ppb_reg_name)
                    else:
                        logger.warning("Unknown cases: {}".format(line))

                if llvm_def_use_pattern in line:
                    Start_LLVM_DEF_USE = not Start_LLVM_DEF_USE
                    continue

                if Start_LLVM_DEF_USE:
                    """
                    [1] @UartHandle is used in this function: USART2_IRQHandler
                    [2] @UartHandle[3] @ is used in this function: config_uarthandle
                    [87] @sha256_padding[88] @[89] @ is used in this function: mbedtls_sha256_finish
                    """
                    # gv = line.split()[1].strip()
                    gv = line.split()[1].split("[")[0]
                    if gv == "@":   #empty
                        continue
                    func = line.split("function:")[1].strip()
                    func_memdep[func]["Variables"].add(gv)
                    # print("{} {}".format(gv, func))
            except Exception:
                traceback.print_exc()
    return func_memdep


def parse_global_variables(def_use_file):
    START_ALL_GLOBALVARIABLES = False
    global_var_set = set()

    with open(def_use_file, "r") as def_use:
        for line in def_use:
            if global_variable_records_start in line:
                START_ALL_GLOBALVARIABLES = not START_ALL_GLOBALVARIABLES
                continue
            if START_ALL_GLOBALVARIABLES:
                global_var_set.add(line.strip())

    global_var_list = list(global_var_set)
    return global_var_list


def check_variable(func_memdep, gv_size_json):
    logger.info("Start to Check whether a Local Vairable pointing to a Global Variable...")
    _func_memdep = {}
    for func in func_memdep.keys():
        #if func not in pts_data.keys():
        #    continue
        gv_set_old = func_memdep[func]["Variables"]
        gv_set_new = set()
        # try:
        #     point_to_set = pts_data[func]
        # except KeyError:
        #     point_to_set = {}
        """
        point-to_set example: func=BSP_LED_Init:
            "arrayidx38": {
                "GPIO_PORT": "GepObjNode"
            },
            "arrayidx40": {
                "GPIO_PORT": "GepObjNode"
            }
        """
        for gv_candidate in gv_set_old:
            if gv_candidate in gv_size_json.keys():
            # if gv_candidate.startswith("@"):
                # global variable startswith "@"
                gv_set_new.add(gv_candidate)
                continue

            # elif gv_candidate.startswith("%"):
            #     tmp = gv_candidate[1:]
            #     if tmp in point_to_set.keys():
            #         # merge 2 sets
            #         gv_set_new = gv_set_new.union(set(point_to_set[tmp].keys()))
            # else:
            #     logger.error("Unknown condition {}".format(gv_candidate))

        _func_memdep[func] = {}
        _func_memdep[func]["Variables"] = list(gv_set_new)
        _func_memdep[func]["Peripherals"] = list(func_memdep[func]["Peripherals"])
        _func_memdep[func]["Whitelist"] = func_memdep[func]["Whitelist"]
        _func_memdep[func]["Whitelist_Namelist"] = list(func_memdep[func]["Whitelist_Namelist"])
        if "caller" in func_memdep[func].keys():
            _func_memdep[func]["caller"] = func_memdep[func]["caller"]
        del gv_set_old

    
    # for func_name in pts_data.keys():
    #     for ptr in pts_data[func_name].keys():
    #         for gv in pts_data[func_name][ptr].keys():
    #             _func_memdep[func_name]["Variables"].add(gv)
    #     _func_memdep[func_name]["Variables"] = list(_func_memdep[func_name]["Variables"])
    #print(func_memdep)
    logger.info("Finish checking!")
    # print(_func_memdep)
    return _func_memdep


def remove_constant_data(func_memdep, ro_data_json):
    """
    remove constant global variables from function dependency because they are read only
    """
    logger.info("Start to Remove constant global variables from function dependency...")
    _func_memdep = {}
    for func, dependency in func_memdep.items():
        gv_set_old = dependency["Variables"]
        gv_set_new = set()
        for gv in gv_set_old:
            if gv in ro_data_json:
                continue
            else:
                gv_set_new.add(gv)
        _func_memdep[func] = {}
        _func_memdep[func]["Variables"] = list(gv_set_new)
        _func_memdep[func]["Peripherals"] = list(func_memdep[func]["Peripherals"])
        _func_memdep[func]["Whitelist"] = func_memdep[func]["Whitelist"]
        _func_memdep[func]["Whitelist_Namelist"] = list(func_memdep[func]["Whitelist_Namelist"])
        if "caller" in func_memdep[func].keys():
            _func_memdep[func]["caller"] = func_memdep[func]["caller"]
        del gv_set_old
    logger.info("Finish checking!")
    return _func_memdep


def add_data_pointer_results(func_memdep, data_pointer_json):
    """
    add svf data pointer analysis results to function dependency
    """
    for function, pointers in data_pointer_json.items():
        for pointer in pointers:
            if pointer["targets"] == None:
                # empty targets
                continue
            for target in pointer["targets"]:
                if target["name"] == pointer["ptr_name"]:
                    # if a pointer name equals to target name, it is a global variable
                    continue
                else:
                    try:
                        func_memdep[function]["Variables"].add(target["name"])
                    except KeyError:
                        add_funcmemdep(func_memdep, function)
                        func_memdep[function]["Variables"].add(target["name"])
    return func_memdep