#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import re

pattern_circle = "shape=record,shape=circle,label="
pattern_mrecord = "shape=record,shape=Mrecord,label="

def format_cg(indot, outdot):
    fin = open(indot, "r")
    fout = open(outdot, "w")
    lines = fin.readlines()
    label_pattern = re.compile('\"{(.*?)}\"')
    for line in lines:
        if pattern_circle in line:
            """
            Node0x557ae39418e0 [shape=record,shape=circle,label="{CallGraphNode ID: 4 \{fun: main\}|{<s0>1|<s1>2|<s2>3|<s3>4|<s4>5|<s5>6|<s6>7|<s7>8|<s8>9|<s9>10|<s10>11|<s11>12}}"];
            """
            line = line.replace("shape=circle,", "")
            func_name = line.split("\{")[1].split("\}")[0]
            func_name = func_name.split(":")[1].strip()
            func_name = '"{' + func_name + '}"'
            line = label_pattern.sub(func_name, line)

        elif pattern_mrecord in line:
            line = line.replace("shape=Mrecord,", "")
            func_name = line.split("\{")[1].split("\}")[0]
            func_name = func_name.split(":")[1].strip()
            func_name = '"{' + func_name + '}"'
            line = label_pattern.sub(func_name, line)

        elif ":" in line and "->" in line:
            node_name, target = line.split("->")
            node_name = node_name.split(":")[0] + " "
            line = node_name + "->" + target

        fout.write(line)
    fin.close()
    fout.close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-in', '--indot', dest='IN_DOT', default="/home/zhouxia/OI_project/stm32f469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/callgraph_final.dot", help='Original dot file')
    parser.add_argument('-out', '--outdot', dest='OUT_DOT', default="/home/zhouxia/OI_project/stm32f469I/Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/callgraph_format.dot", help="Formatted dot file")
    args = parser.parse_args()
    format_cg(args.IN_DOT, args.OUT_DOT)