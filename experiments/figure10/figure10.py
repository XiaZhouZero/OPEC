#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json
import matplotlib.pyplot as plt
from matplotlib.pyplot import MultipleLocator
import matplotlib as mpl
import numpy as np
import argparse


app_name_mappings = {
    "PinLock": "PinLock", 
    "LCD_AnimatedPictureFromSDCard": "Animation", 
    "FatFs_uSD": "FatFs-uSD", 
    "LCD_PicturesFromSDCard": "LCD-uSD", 
    "LwIP_TCP_Echo_Server": "TCP-Echo", 
    "Camera_To_USBDisk": "Camera", 
    "CoreMark": "CoreMark"
}

policy_name_mappings = {
    "filename": "ACES1",
    "filename-no-opt": "ACES2",
    "peripheral": "ACES3"
}

def store_json_to_file(json_to_dump, filename):
    with open(filename, "w") as f:
        json.dump(json_to_dump, f, sort_keys=True, indent=4)

def load_json_from_file(filename):
    with open(filename, "r") as f:
        json_to_load = json.load(f)
    return json_to_load

def get_results(results_file):
    results_summary = {}
    results = load_json_from_file(results_file)
    for app, content in results.items():
        app_breif = app_name_mappings[app]
        results_summary[app_breif] = {}
        for policy, results in content.items():
            print(policy)
            policy_breif = policy_name_mappings[policy]
            results_summary[app_breif][policy_breif] = results

    return results_summary

    
def main():
    # parse results
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--cur_dir', dest='CUR_DIR', default="", help='The path of current figure directory')
    args = parser.parse_args()
    dir_name = args.CUR_DIR
    results_file = dir_name + "/Results.json"
    results_summary = get_results(results_file)

    # figure style configuration
    plt.style.use('seaborn-poster')
    mpl.rcParams["font.sans-serif"] = ["Times New Roman"]
    mpl.rcParams["axes.unicode_minus"] = False
    mpl.rcParams["font.size"] = 22
    mpl.rcParams["font.weight"] = "bold"
    mpl.rcParams['figure.figsize'] = (32, 4)

    plt.xlim(0, 1)
    plt.ylim(0, 1)
    n_bins = 5

    # fig = plt.figure()
    fig, axs = plt.subplots(1, len(results_summary.keys()))
    # fig, axs = plt.subplots(2, 3)

    # fig, ax = plt.subplots(figsize=(8, 4))

    # plot the cumulative histogram
    # n, bins, patches = ax.hist(x, n_bins, density=True, histtype='step', cumulative=True, label='Empirical')

    index = "abcdefg"
    app_id = 0
    for app_name, app_info in results_summary.items():
        policy_id = 0
        for policy_name, partition_overprivilege_ratio in app_info.items():
            axs[app_id].hist(partition_overprivilege_ratio, n_bins, density=True, histtype='step', cumulative=True, label=policy_name, linewidth=3)
            x_major_locator=MultipleLocator(0.2)
            y_major_locator=MultipleLocator(0.1)
            axs[app_id].xaxis.set_major_locator(x_major_locator)
            axs[app_id].yaxis.set_major_locator(y_major_locator)
            # axs[app_id].grid(True)
            if app_id == 0:
                axs[app_id].legend(loc='center', prop={'size': 18})
                axs[app_id].set_ylabel("CDF", fontsize=20, fontweight="bold")
            axs[app_id].set_xlabel("PT", fontsize=20, fontweight="bold")
            axs[app_id].set_title("("+index[app_id]+") " + app_name, fontsize=20, fontweight="bold")
            # policy_id += 1
        app_id += 1

    plt.savefig("Figure10_Partition_Overprivilege.pdf", dpi=600, format="pdf", bbox_inches='tight')
    # plt.show()


if __name__ == "__main__":
    main()