# OPEC

This package includes the source codes and evaluatoin scritpts in the paper "[OPEC: Operation-based Security Isolation for Bare-metal Embedded Systems](#)" presented at EuroSys 2022.

```
.
├── experiments						# scripts and raw data for experiments
├── STM32Cube_FW_F4_V1.14.0			# Source code of projects and libraries
├── STM32Cube_FW_F4_V1.25.0			# Source code of projects and libraries
├── app_tools
│   ├── pinlock_serial.py
│   └── tcp_connect.py
├── compiler
│   ├── project_init_scripts		# Scritpts used to build the OPEC
│   ├── 3rd_party					# Source code of third party tools including gcc-arm-none-eabi, LLVM4, and LLVM9
│   ├── analysis					# LLVM9 patch files for operation resource dependency analysis and operation policy generation
│   ├── llvm						# LLVM4 patch files for instruction instrumentation
│   ├── operation-rt				# OPEC-Monitor source code
│   └── svf_patch					# SVF patch files used to format the point-to analysis output and add additional output
├── stm32f407						# Tested applications (PinLock) runs on the STM32F407 board
│   ├── Drivers -> ../STM32Cube_FW_F4_V1.25.0/Drivers
│   ├── Middlewares -> ../STM32Cube_FW_F4_V1.25.0/Middlewares
│   ├── Utilities -> ../STM32Cube_FW_F4_V1.25.0/Utilities
│   ├── PinLock
│   ├── build_app.sh				# Script for building tested applications
│   └── clean_app.sh				# Script for cleaning tested applications
└── stm32f469I						# Tested applications (PinLock) runs on the STM32469I-EVAL board
    ├── Drivers -> ../STM32Cube_FW_F4_V1.25.0/Drivers
    ├── Middlewares -> ../STM32Cube_FW_F4_V1.25.0/Middlewares
    ├── Utilities -> ../STM32Cube_FW_F4_V1.25.0/Utilities
    ├── Camera_To_USBDisk
    ├── FatFs_uSD
    ├── LCD_AnimatedPictureFromSDCard
    ├── LCD_PicturesFromSDCard -> ../STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/Display/LCD_PicturesFromSDCard
    ├── LwIP_TCP_Echo_Server -> ../STM32Cube_FW_F4_V1.14.0/Projects/STM32469I_EVAL/Applications/LwIP/LwIP_TCP_Echo_Server
    ├── CoreMark
    ├── build_app.sh				# Script for building tested applications
    └── clean_app.sh				# Script for cleaning tested applications
```



## 0x01 Enviroments

### OS & HW

+ Ubuntu 18.04
+ Hardware disk: 150G
+ CPU cores: 6

### Set up the Ubuntu Environment

Change the Ubuntu source mirror if needed

```
deb http://mirrors.zju.edu.cn/ubuntu bionic main universe restricted multiverse
deb http://mirrors.zju.edu.cn/ubuntu bionic-security main universe restricted multiverse
deb http://mirrors.zju.edu.cn/ubuntu bionic-updates main universe restricted multiverse
deb http://mirrors.zju.edu.cn/ubuntu bionic-backports main universe restricted multiverse
```

Install necessary packages

```bash
sudo apt install zlib1g-dev git build-essential gcc-arm-none-eabi make texinfo bison flex cmake ninja-build libusb-dev python3-pip texlive-full clang libusb-dev openocd
```

### Set up the Python Environment

Set python virtual environment if needed

```bash
$ sudo apt install virtualenv
$ virtualenv -p python3.6.9 py3
$ source py3/bin/active
$ pip3 install pip -U
$ pip3 config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple
```

Install python libriries

```bash
$ pip3 install networkx==2.5 pydot matplotlib serial pydotplus prettytable
```



## 0x02 Build OPEC

### Get OPEC Source

+ It takes about 25 minutes to clone OPEC

```bash
$ git clone https://github.com/XiaZhouZero/OPEC
```

Set the environment variable `OI_DIR` to the absolute path of OPEC repo.

*OI* means *Operation Isolation*.

```bash
$ export OI_DIR=/home/eurosys22/OPEC/		# note that the path must have the last `/`
```

### Build GCC

+ It takes about 3 hours to build GCC

```bash
$ cd OPEC/compiler
OPEC/compiler$ ./project_init_scripts/init_gcc.sh
```

### Build LLVM&SVF

+ It takes about 1 hour to build LLVM and SVF

```bash
OPEC/compiler$ ./project_init_scripts/init_llvm9.sh
OPEC/compiler$ ./project_init_scripts/init_svf.sh
OPEC/compiler$ ./project_init_scripts/init_llvm4.sh
```

### Build OPEC-Monitor

+  It takes less than 1 minute to build OPEC-Monitor

```bash
OPEC/compiler$ cd operation-rt
OPEC/compiler/operation-rt$ make all
```

## 0x03 Build Applications

+ It takes less than 5 minutes to build each application

### STM32F407

Build tested applications that run on the STM32F407 development board.

```bash
$ cd OPEC/stm32f407
OPEC/stm32f407$ vim ./build_app.sh				# change OI_DIR to OPEC repo path, e.g., OI_DIR=/home/eurosys22/OPEC
OPEC/stm32f407$ ./build_app.sh <app_name>		# get the app list by running `./build_app.sh help`

OPEC/stm32f407$ vim ./clean_app.sh				# change OI_DIR to OPEC repo path, e.g., OI_DIR=/home/eurosys22/OPEC
OPEC/stm32f407$ ./clean_app.sh <app_name>		# get the app list by running `./clean_app.sh help`
```

For example, to build PinLock

```bash
OPEC/stm32f407$ ./build_app.sh PinLock
```

### STM32469I-Eval

Build tested applications that run on the STM32469I-Eval board.

```bash
$ cd OPEC/stm32469I
OPEC/stm32469I$ vim ./build_app.sh				# change OI_DIR to OPEC repo path, e.g., OI_DIR=/home/eurosys22/OPEC
OPEC/stm32469I$ ./build_app.sh <app_name>		# get the app list by running `./build_app.sh help`

OPEC/stm32469I$ vim ./clean_app.sh				# change OI_DIR to your OI path
OPEC/stm32469I$ ./clean_app.sh <app_name>		# get the app list by running `./build_app.sh help`
```

For exmaple, to build Camera

```bash
OPEC/stm32469I$ ./build_app.sh Camera_To_USBDisk
```

## 0x04 Run Applications
Open a terminal to runs the gdb-server
```bash
$ openocd -f /usr/share/openocd/scripts/interface/stlink-v2-1.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg
```

### Run Baseline Applications
The baseline binary of each application is in the `build9/` directory. Take Camera as an example:
```bash
OPEC/stm32469I$ ./arm-none-eabi-gdb-py -iex "target remote :3333" -iex "monitor reset halt" Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build9/Camera_To_USBDisk.elf
...
(gdb) load
```

To test the used clock cycles of each baseline appliction:
+ First, add the marco `DTIMER_BASELINE` to the `C_DEFS` variable in Makefile9.mk file of each application.
+ Second, rebuild the tested application.
+ Third, run the baseline binary, the used clock cycles of each baseline operation are recorded in the `ExeTime` array. Read this value in gdb.


### Run Applications Built with OPEC
The binary built with OPEC of each application is in the `build4/` directory. Take Camera as an example
```bash
OPEC/stm32469I$ ./arm-none-eabi-gdb-py -iex "target remote :3333" -iex "monitor reset halt" Camera_To_USBDisk/SW4STM32/STM32469I_EVAL/build4/Camera_To_USBDisk--oi--final.elf
...
(gdb) load
```

To test the used clock cycles of each appliction built with OPEC:
+ First, add the marco `DTIMER_OPEC` to the `CFLAGS` variable in the Makefile file in `compiler/operation-rt` directory.
+ Second, rebuild the tested application.
+ Third, run the agumented binary, the used clock cycles of each operation are recorded in the `ExeTime` array. Read this value in gdb and caculate the runtime overhead incurred by OPEC.


### Use GDB to Record Executed Functions
Run this command to launch gdb. Take PinLock as example:
```bash
$ ./arm-none-eabi-gdb-py -x commands/PinLock_Commands.txt stm32f407/PinLock/Decode/SW4STM32/STM32F4-DISCO/build9/PinLock.elf
```

File `PinLock_Commands.txt` contains the commands gdb runs. For example:
```txt
load
set logging file ./logs/PinLock.log
set pagination off
set logging on

b main.c:274

c
while 1
step
```
File `PinLock.log` records the executed source code lines.