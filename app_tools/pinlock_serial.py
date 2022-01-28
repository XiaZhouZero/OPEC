#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Functionality: to interact with pinlock at serial port automatically
Reference:
    https://github.com/embedded-sec/ACES/blob/master/compiler/tools/pinlock_stimulus.py
"""

import sys
import time
import argparse
import logging

import serial
from serial import Serial

LOG_FORMAT = "%(asctime)s - %(levelname)s - %(message)s"
DATE_FORMAT = "%Y/%m/%d %H:%M:%S %p"

logging.basicConfig(filename='pinlock_debug.log', level=logging.INFO, format=LOG_FORMAT, datefmt=DATE_FORMAT)

unlock_flag = False
lock_flag = False
wrong_pin_cnt = 0

board = "Discovery-Board"

correct_unlock_pin = "1995\n"
correct_lock_pin = "0\n"
wrong_unlock_pin = "1234\n"

def welcome():
    """
    wellcome message
    """
    print("Script to automate pinlock application")
    print("Press Ctrl+C to quit")


def setup_serial(ser, port, baudrate):
    # print(id(ser))
    ser.port = "/dev/" + port
    ser.baudrate = baudrate
    ser.timeout = 1
    ser.write_timeout = 1
    ser.stopbits = serial.STOPBITS_ONE
    ser.open()
    if(not ser.is_open):
        logging.error("Fail to open device {}.".format(ser.port), exc_info=True)
    else:
        logging.info("Open device {} successfully.".format(ser.port))


def interactive(board_name):
    global unlock_flag
    global lock_flag
    global wrong_pin_cnt

    while True:
        if ser.in_waiting > 0:
            messages = ser.readlines()
            # print(messages)
            for msg in messages:
                if msg == "\x00":
                    continue
                print("[{}]: {}".format(board, msg.strip()))
                logging.info("Received message from {}: {}".format(ser.name, msg))
                if "Enter Pin" in msg:
                    pin_flag = True
                elif "Incorrect Pin" in msg:
                    correct_flag = True
                elif "Unlocked" in msg:
                    lock_flag = True
                elif "FUNC_FLAG" in msg:
                    print("executed function {}".format(msg.split(":")[1].strip()))

            if pin_flag:
                if wrong_pin_cnt == 0:
                    ser.write(wrong_unlock_pin)
                    print("[Input]: {}".format(wrong_unlock_pin.strip()))
                else:
                    ser.write(correct_unlock_pin)
                    print("[Input]: {}".format(correct_unlock_pin.strip()))
                pin_flag = False
                wrong_pin_cnt ^= 1;

            if lock_flag:
                ser.write(correct_lock_pin)
                print("[Input]: {}".format(correct_lock_pin.strip()))
                lock_flag = False
            

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-p', '--port', dest='port', required=False, help="Device port name, e.g., ttyUSB0", default='ttyUSB0')
    parser.add_argument('-b', '--baudrate', dest='baudrate', required=False, help="Device baudrate, e.g., 115200", default='115200')
    args = parser.parse_args()

    welcome()
    ser = Serial()
    setup_serial(ser, args.port, args.baudrate)
    # print(id(ser))
    # global board
    interactive(board)