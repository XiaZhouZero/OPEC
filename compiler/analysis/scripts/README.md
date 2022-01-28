  
# 外设分析程序PeripheralAnalysis.py的运行条件：
# 1.已获得程序的Callgraph.dot
## 获得CallGraph的方式为：
    1. 采用LLVM编译Pinlock baseline获得PinLock.elf.0.5.precodegen.bc
    2. ~~用LLVM 9.0原生的opt程序分析即可PinLock.elf.0.5.precodegen.bc可得到整个程序的callgraph。
        运行LLVM 9.0::opt工具：
```shell
        $opt -dot-callgraph PinLock.elf.0.5.precodegen.bc > /dev/null
```~~
       采用SVF的wpa工具生成callgraph_final.dot文件，然后对文件format得到最后的callgraph.dot文件
    3. 以上可得到dot格式的callgraph，需要用networkx(python)进行图的分析（代码FindFunctions.py）
        该图为多向图（multidigraph），我们需要从Operation对应的Entry_function节点开始层层搜索接下来所有可能调用到的函数。这里采用了深度优先搜索（DFS）
    4. 分析结果是该Operation中所有会调用到的函数集合 
    

        # "key_init": [
        #     "mbedtls_sha256_free",
        #     "mbedtls_sha256_starts",
        #     "mbedtls_sha256_process",
        #     "mbedtls_sha256_update",
        #     "mbedtls_zeroize",
        #     "memcpy",
        #     "memset",
        #     "mbedtls_sha256_init",
        #     "mbedtls_sha256_finish",
        #     "mbedtls_sha256"
        # ],
        # "handle_lock_input": [
        #     "HAL_UART_Receive_IT",
        #     "rx_from_uart",
        #     "BSP_LED_On",
        #     "HAL_GPIO_WritePin",
        #     "BSP_LED_Off",
        #     "lock",
        #     "Error_Handler"
        # ],
        # "Program_Init": [
        #     "SysTick_Config",
        #     "NVIC_EncodePriority",
        #     "HAL_RCC_GetPCLK2Freq",
        #     "HAL_RCC_GetSysClockFreq",
        #     "HAL_Init",
        #     "HAL_UART_Init",
        #   ........
# 2.已有手工标记的operation_entry function名称集合（它被放在FindFunction.py开头，以数组的形式保存）内容示例如下
        #   OperationNames = [
        #     '"{Program_Init}"',
        #     '"{key_init}"',
        #     '"{handle_unlock_input}"',
        #     '"{handle_lock_input}"'
        #     ]
# 3. 应用程序每个function中load和store指令的集合。oi.log（OperationIsolation/test_apps/pinlock/Decode/SW4STM32/STM32F4-DISCO/oi.log1）
        # Operation isolation Application Pass
        # ########################################ALL LOAD & STORE########################################
        # $FunctionName: lock
        # $FunctionName: unlock
        # $FunctionName: rx_from_uart
        #   store i32 %size, i32* %size.addr, align 4
        #   %0 = load i32, i32* %size.addr, align 4, !dbg !1884
        #   %1 = load i32, i32* %size.addr, align 4, !dbg !1891
        #   %2 = load volatile i8, i8* @UartReady, align 1, !dbg !1900
        #   store volatile i8 0, i8* @UartReady, align 1, !dbg !1908
        # $FunctionName: llvm.dbg.declare
        # $FunctionName: Error_Handler
        # $FunctionName: handle_unlock_input
        #   store i32 %one, i32* %one.addr, align 4
        #   store i32 %exp, i32* %exp.addr, align 4
        #   store i32 %ms, i32* %ms.addr, align 4
        #   store i32* %unlock_count, i32** %unlock_count.addr, align 4
        #   store i32 %unused, i32* %unused.addr, align 4
        #   store i32 0, i32* %failures, align 4, !dbg !1894
        #   store i32 0, i32* %i, align 4, !dbg !1902
        #   ....
# 4. 应用程序SUPA指针分析的结果SUPA.txt
        1.SUPA指针分析程序的安装：
        1.1 下载SVF指针分析套件
```shell
            $git clone https://github.com/SVF-tools/SVF.git
```
        1.2 设置编译SVF需采用的LLVM+CLANG编译程序环境
```shell       
            $export LLVM_DIR=PATH/TO/LLVM/BINARY/bin
            $export PATH=$LLVM_DIR:$PATH
```     1.3 指定生成SUPA工具，同类型的还有WPA，SABER...
            取消注释SVF_ROOT/tools/CmakeLists.txt中包含有“DDA”的这一行
            取消注释SVF_ROOT/lib//CmakeLists.txt中包所有含有“DDA”的行
        1.4 编译SVF/SUPA
```shell
            $cd SVF
            $mkdir Release-build
            $cd Release-build
            $cmake ../
            $make -j4
```
        1.5 对PinLock.elf.0.5.precodegen.bc进行指针分析
```shell
            $cd SVF
            $. ./setup.sh
            $dvf -cxt -query=all -maxcxt=3 -flowbg=10000 -cxtbg=10000 -print-query-pts  O2.PinLock.elf.0.5.precodegen.bc > SUPA.txt
```
        1.6 SUPA.txt就是指针分析的结果了
# 5.运行PeripheralAnalysis.py得到外设依赖结果 Pinlock_peripheral_result.json
        形式为：


        # {
        #     "handle_unlock_input": [ #用户指定的Entry Function
        #         [
        #             "0x40004400", #外设起始地点
        #             "rw",         #外设的访问权限，默认为rw
        #             "USART2",     #外设的名称
        #             "0x400"       #外设的地址范围
        #         ], #多个外设
        #         [
        #             "0x40020c00",
        #             "rw",
        #             "GPIOD",
        #             "0x400"
        #         ]
        #     ],
        #     "key_init": [
        #         [
        #             "0x40020c00",
        #             "rw",
        #             "GPIOD",
        #             "0x400"
        #         ]
        #     ],
        #     ..... 
        
        
![file](http://www.nuanyun.cloud/wp-content/uploads/2020/04/5ea64e6b353ab.png) 

![file](http://www.nuanyun.cloud/wp-content/uploads/2020/04/5ea64f0165b30.png) 