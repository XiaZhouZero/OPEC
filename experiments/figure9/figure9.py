#!/usr/bin/env python
# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import json
import argparse

labels = ["PinLock", "Camera", "Animation", "FatFs-uSD", "LCD-uSD", "TCP-Echo", "CoreMark", "Average"]

apps = ["PinLock", "Camera", "Animation", "FatFs-uSD", "LCD-uSD", "TCP-Echo", "CoreMark"]

app_name_mappings = {
    "PinLock": "PinLock", 
    "Animation": "LCD_AnimatedPictureFromSDCard", 
    "FatFs-uSD": "FatFs_uSD", 
    "LCD-uSD": "LCD_PicturesFromSDCard", 
    "TCP-Echo": "LwIP_TCP_Echo_Server", 
    "Camera": "Camera_To_USBDisk", 
    "CoreMark": "CoreMark"
}


def load_json_from_file(filename):
    with open(filename, "r") as f:
        json_to_load = json.load(f)
    return json_to_load


def get_runtime_overhead(runtime_overhead_file):
    result = load_json_from_file(runtime_overhead_file)
    runtime_overhead = []
    for app in apps:
        app_complex = app_name_mappings[app]
        runtime_overhead.append(result[app_complex]["ratio"])
    
    average = round(sum(runtime_overhead)*1.0/len(runtime_overhead), 2)
    runtime_overhead.append(average)
    
    return runtime_overhead

def get_sram_overhead(sram_overhead_file):
    results = load_json_from_file(sram_overhead_file)
    sram_overhead = []
    _sram_overhead = []
    for app in apps:
        app_complex = app_name_mappings[app]
        _sram_overhead.append(results[app_complex]["ratio"])

    average = sum(_sram_overhead) / len(_sram_overhead)
    _sram_overhead.append(average)

    for i in _sram_overhead:
        sram_overhead.append(round(i, 2))

    return sram_overhead


def get_flash_overhead(flash_overhead_file):
    results = load_json_from_file(flash_overhead_file)
    flash_overhead = []
    _flash_overhead = []
    for app in labels:
        if app == "Average":
            continue
        app_complex = app_name_mappings[app]
        _flash_overhead.append(results[app_complex]["ratio"])

    average = round(sum(_flash_overhead) / len(_flash_overhead), 2)
    _flash_overhead.append(average)

    for i in _flash_overhead:
        flash_overhead.append(round(i, 2))

    return flash_overhead


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--cur_dir', dest='CUR_DIR', default="", help='The path of current figure directory')
    args = parser.parse_args()
    dir_name = args.CUR_DIR

    runtime_overhead_file = dir_name + "/" + "OPEC_runtime_overhead_json"
    runtime_overhead = get_runtime_overhead(runtime_overhead_file)

    flash_overhead_file = dir_name + "/" + "flash_overhead.json"
    flash_overhead = get_flash_overhead(flash_overhead_file)

    sram_overhead_file = dir_name + "/" + "sram_overhead.json"
    sram_overhead = get_sram_overhead(sram_overhead_file)

    plt.style.use('seaborn-colorblind')
    # plt.style.use('seaborn-paper')

    mpl.rcParams["font.sans-serif"] = ["Times New Roman"]
    mpl.rcParams["axes.unicode_minus"] = False
    mpl.rcParams["font.size"] = 28
    mpl.rcParams['figure.figsize'] = (24.0, 9.0)
    mpl.rcParams["font.weight"] = "bold"

    x = np.arange(len(labels))  # the label locations
    width = 0.31  # the width of the bars

    fig, ax = plt.subplots()
    rects1 = ax.bar(x - width, runtime_overhead, width, label='Runtime Overhead')
    rects2 = ax.bar(x, flash_overhead, width, label='Flash Overhead')
    rects3 = ax.bar(x + width, sram_overhead, width, label='SRAM Overhead')

    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_ylabel('Overhead (%)', fontweight="bold")
    ax.set_title('')
    ax.set_xticks(x)
    ax.set_xticklabels(labels)
    ax.legend(loc="upper left", prop={'size': 24})
    # ax.grid(True)

    def autolabel(rects):
        """Attach a text label above each bar in *rects*, displaying its height."""
        for rect in rects:
            height = rect.get_height()
            ax.annotate('{}'.format(height),
                        xy=(rect.get_x() + rect.get_width() / 2, height-0.1),
                        xytext=(0, 3),  # 3 points vertical offset
                        textcoords="offset points",
                        ha='center', va='bottom')


    autolabel(rects1)
    autolabel(rects2)
    autolabel(rects3)

    fig.tight_layout()

    fig.savefig("Figure9_Performance_Overhead.pdf", dpi=600, format="pdf", bbox_inches='tight')
    # plt.show()

if __name__ == "__main__":
    main()