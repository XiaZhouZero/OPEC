#!/usr/bin/env python
# -*- coding: utf-8 -*-

from prettytable import PrettyTable
import argparse
import json

app_name_mappings = {
    "PinLock": "PinLock", 
    "Animation": "LCD_AnimatedPictureFromSDCard", 
    "FatFs-uSD": "FatFs_uSD", 
    "LCD-uSD": "LCD_PicturesFromSDCard", 
    "TCP-Echo": "LwIP_TCP_Echo_Server", 
}

apps = ["PinLock", "Animation", "FatFs-uSD", "LCD-uSD", "TCP-Echo"]

aces_policies = ["ACES1", "ACES2", "ACES3"]

def load_json_from_file(filename):
    with open(filename, "r") as f:
        json_to_load = json.load(f)
    return json_to_load

def print_table2(aces_runtime_overhead, aces_flash_overhead, aces_sram_overhead, aces_privileged_app_code, opec_runtime_overhead, opec_flash_overhead, opec_sram_overhead):
    table2_head = ["Application", "Policy", "Runtime Overhead(X)", "Flash Overhead(%)", "SRAM Overhead(%)", "Privileged Application Code(%)"]
    table = PrettyTable(table2_head)
    for app in apps:
        app_complex = app_name_mappings[app]
        table.add_row([app, "OPEC", round(opec_runtime_overhead[app_complex]["ratio"], 2), round(opec_flash_overhead[app_complex]["ratio"], 2), round(opec_sram_overhead[app_complex]["ratio"], 2), 0.00])
        for policy in aces_policies:
            table.add_row([app, policy, round(aces_runtime_overhead[app][policy]["ratio"], 2), round(aces_flash_overhead[app][policy]["ratio"], 2), round(aces_sram_overhead[app][policy]["ratio"], 2), round(aces_privileged_app_code[app][policy]["ratio"], 2)])
    
    print(table)


def main():
    # parse results
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--cur_dir', dest='CUR_DIR', default="", help='The path of current figure directory')
    args = parser.parse_args()
    dir_name = args.CUR_DIR

    opec_runtime_overhead_file = dir_name + "/" + "OPEC_runtime_overhead_json"
    opec_runtime_overhead = load_json_from_file(opec_runtime_overhead_file)

    opec_flash_overhead_file = dir_name + "/" + "OPEC_flash_overhead.json"
    opec_flash_overhead = load_json_from_file(opec_flash_overhead_file)

    opec_sram_overhead_file = dir_name + "/" + "OPEC_sram_overhead.json"
    opec_sram_overhead = load_json_from_file(opec_sram_overhead_file)

    # get ACES's runtime overhead is from paper: https://www.usenix.org/system/files/conference/usenixsecurity18/sec18-clements.pdf
    aces_runtime_overhead_file = dir_name + "/" + "ACES_runtime_overhead_json"
    aces_runtime_overhead = load_json_from_file(aces_runtime_overhead_file)

    aces_flash_overhead_file = dir_name + "/" + "ACES_flash_overhead.json"
    aces_flash_overhead = load_json_from_file(aces_flash_overhead_file)

    aces_sram_overhead_file = dir_name + "/" + "ACES_sram_overhead.json"
    aces_sram_overhead = load_json_from_file(aces_sram_overhead_file)

    aces_privileged_app_code_file = dir_name + "/" + "ACES_privileged_app_code.json"
    aces_privileged_app_code = load_json_from_file(aces_privileged_app_code_file)

    print_table2(aces_runtime_overhead, aces_flash_overhead, aces_sram_overhead, aces_privileged_app_code, opec_runtime_overhead, opec_flash_overhead, opec_sram_overhead)


if __name__ == "__main__":
    main()