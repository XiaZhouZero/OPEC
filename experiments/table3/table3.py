#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import subprocess
import json
import logging
from prettytable import PrettyTable

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

# apps = ["PinLock", "Camera_To_USBDisk", "FatFs_RAMDisk", "FatFs_uSD", "LCD_AnimatedPictureFromSDCard", "LCD_PicturesFromSDCard", "LwIP_TCP_Echo_Server"]

apps = ["PinLock", "Camera_To_USBDisk", "FatFs_uSD", "LCD_AnimatedPictureFromSDCard", "LCD_PicturesFromSDCard", "LwIP_TCP_Echo_Server", "CoreMark"]

ASM_LABEL = "asm sideeffect"

def store_json_to_file(json_to_dump, filename):
    with open(filename, "w") as f:
        json.dump(json_to_dump, f, sort_keys=True, indent=4)


def load_json_from_file(filename):
    with open(filename, "r") as f:
        json_to_load = json.load(f)
    return json_to_load


def eval_point2(results, svf_solved, type_based_sovled):
    callinst_sum = 0
    targets_sum = 0
    targets_max = 0
    for function, callinsts in results.items():
        if callinsts != None:
            callinst_sum += len(callinsts)
            for callinst in callinsts:
                targets_num = 0
                if callinst["targets"] == None:
                    targets_num = 0
                else:
                    targets_num = len(callinst["targets"])
                targets_sum += targets_num
                if targets_num > targets_max:
                    targets_max = targets_num
    summary = {
        "targets_max": targets_max,
        "targets_sum": targets_sum,
        "callinst_sum": callinst_sum,
        "svf_solved": svf_solved,
        "svf_solved_ratio": round(svf_solved*100.0/callinst_sum, 2),
        "type_based_solved": type_based_sovled,
        "type_based_solved_ratio": round(type_based_sovled*100.0/callinst_sum, 2),
        "targets_avg": round(targets_sum*1.0/callinst_sum, 2)
    }
    return summary

def get_analysis_time(point2_stat_file):
    time_label = "TotalTime"
    total_time = 0.0
    with open(point2_stat_file, "r") as f:
        for line in f.readlines():
            if time_label in line:
                total_time += float(line.split(time_label)[1].strip())
    return round(total_time, 2)
    


def print_table3(Point2_Summary):
    table1_head = ["Application", "#Icall", "#SVF", "Time(s)", "#Type", "#Avg.", "#Max"]
    app_name_mappings = {
        "PinLock": "PinLock", 
        "Animation": "LCD_AnimatedPictureFromSDCard", 
        "FatFs-uSD": "FatFs_uSD", 
        "LCD-uSD": "LCD_PicturesFromSDCard", 
        "TCP-Echo": "LwIP_TCP_Echo_Server", 
        "Camera": "Camera_To_USBDisk", 
        "CoreMark": "CoreMark"
    }

    table = PrettyTable(table1_head)
    for app_breif, app in app_name_mappings.items():
         table.add_row([app_breif, Point2_Summary[app]["callinst_sum"], Point2_Summary[app]["svf_solved"], Point2_Summary[app]["total_time"], Point2_Summary[app]["type_based_solved"], Point2_Summary[app]["targets_avg"], Point2_Summary[app]["targets_max"]])
    
    print(table)


def main():
    logger.info("Point-to analysis evaluation")
    dir_root = os.path.abspath(os.curdir)
    Point2_Summary = {}
    for app in apps:
        logger.info("APP: {}".format(app))
        Point2_Results = {}
        svf_solved = 0
        type_based_sovled = 0
        ander_pointto_file = dir_root + "/raw_results/" + app + "/" + "ander_funcptr.json"
        type_based_file    = dir_root + "/raw_results/" + app + "/" + "type_based_analysis_results.json"
        point2_stat_file  = dir_root + "/raw_results/" + app + "/" + app + ".pts"
        ander_point2_results = load_json_from_file(ander_pointto_file)
        type_based_results = load_json_from_file(type_based_file)

        for func, callsites in ander_point2_results.items():
            Point2_Results[func] = []
            for callsite in callsites:
                if ASM_LABEL not in callsite:
                    if callsite["targets"]:
                        Point2_Results[func].append(
                            {
                                "instruction": callsite["callsite"],
                                "location": callsite["location"],
                                "targets": callsite["targets"]
                            }
                        )
                        svf_solved += 1
                    else:
                        if func in type_based_results.keys():
                            for callinst in type_based_results[func]:
                                if callsite["location"] == callinst["location"]:
                                    if callinst["targets"]:
                                        type_based_sovled += 1
                                    Point2_Results[func].append(
                                        {
                                            "instruction": callsite["callsite"],
                                            "location": callsite["location"],
                                            "targets": callinst["targets"]
                                        }
                                    )
        store_json_to_file(Point2_Results, dir_root + "/" + app + "_results.json")
        Point2_Summary[app] = eval_point2(Point2_Results, svf_solved, type_based_sovled)
        Point2_Summary[app]["total_time"] = get_analysis_time(point2_stat_file)

    store_json_to_file(Point2_Summary, dir_root + "/" + "Summary.json")

    print_table3(Point2_Summary)


if __name__ == "__main__":
    main()