/*
*****************************************************************************
**
**  File        : LinkerScript.ld
**
**  Author      : Ac6
**
**  Abstract    : Linker script for STM32F407VG Device with
**                1024KByte FLASH, 192KByte RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Environment : System Workbench for MCU
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
*****************************************************************************
**
** <h2><center>&copy; COPYRIGHT(c) 2014 Ac6</center></h2>
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**   1. Redistributions of source code must retain the above copyright notice,
**      this list of conditions and the following disclaimer.
**   2. Redistributions in binary form must reproduce the above copyright notice,
**      this list of conditions and the following disclaimer in the documentation
**      and/or other materials provided with the distribution.
**   3. Neither the name of Ac6 nor the names of its contributors
**      may be used to endorse or promote products derived from this software
**      without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
** OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
/* top of stack 64K from bottom of RAM on AHB bus*/
/*_estack = DEFINED(estack) ? estack : 0x20010004;*/
/*_estack = 0x2000FFFC;*/

/*Default bottom of unsafe stack (grows up) 64K from top of ram */
/*_eunsafe_stack = DEFINED(eunsafe_stack) ? eunsafe_stack : 0x20020000; */
/*_eunsafe_stack = 0x20020000;*/

/*Ram really starts at 0x20000000  Giveing 64K for stack (Grows Down)*/
/*_start_data = DEFINED(start_data) ? start_data : 0x20010000;  */
/*_start_data = 0x20010000;*


/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x4000;      /* required amount of heap  */
_Min_Stack_Size = 0x4000; /* required amount of stack */

/* Specify the memory areas */

MEMORY
{
  FLASH (RX)      : ORIGIN = 0x08000000 , LENGTH = 1024K
  RAM (XRW)       : ORIGIN = 0x20000000, LENGTH = 128K
  MEMORY_B1 (RX)  : ORIGIN = 0x60000000, LENGTH = 0K
  CCMRAM (rw)      : ORIGIN = 0x10000000, LENGTH = 64K
}

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(8);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(8);
  } >FLASH

  /* <OPEC_MONITOR_SECTION> */

  /* The program code and other data goes into FLASH */
  .text :       /* OLD_TEXT */
  {       /* OLD_TEXT */
    . = ALIGN(8);       /* OLD_TEXT */
    _stext = .;       /* OLD_TEXT */
    *(.text)           /* .text sections (code) */        /* OLD_TEXT */
    *(.text*)          /* .text* sections (code) */       /* OLD_TEXT */
    *(.glue_7)         /* glue arm to thumb code */       /* OLD_TEXT */
    *(.glue_7t)        /* glue thumb to arm code */       /* OLD_TEXT */
    *(.eh_frame)        /* OLD_TEXT */
        /* OLD_TEXT */
    KEEP (*(.init))       /* OLD_TEXT */
    KEEP (*(.fini))       /* OLD_TEXT */
        /* OLD_TEXT */
    . = ALIGN(8);       /* OLD_TEXT */
    _etext = .;        /* define a global symbols at end of code */       /* OLD_TEXT */
  } >FLASH        /* OLD_TEXT */

  /* <ALIGNED_TEXT_SECTION> */

  /* Constant data goes into FLASH */
  .rodata :         /* OLD_RODATA */
  {                 /* OLD_RODATA */
    . = ALIGN(8);   /* OLD_RODATA */
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */    /* OLD_RODATA */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */   /* OLD_RODATA */
    . = ALIGN(8);   /* OLD_RODATA */
  } >FLASH          /* OLD_RODATA */

  /* <ALIGNED_RODATA_SECTION> */

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .stack :
  {
    _sstack = .;
    . = . + _Min_Stack_Size; 
    _estack = .;
  } >RAM

  .data : 
  {
    . = ALIGN(8);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(8);
    _edata = .;        /* define a global symbol at data end */
  } >RAM AT> FLASH



  _siccmram = LOADADDR(.ccmram);

  /* CCM-RAM section 
  * 
  * IMPORTANT NOTE! 
  * If initialized variables will be placed in this section,
  * the startup code needs to be modified to copy the init-values.  
  */
  .ccmram :
  {
    . = ALIGN(4);
    _sccmram = .;       /* create a global symbol at ccmram start */
    *(.ccmram)
    *(.ccmram*)
    
    . = ALIGN(4);
    _eccmram = .;       /* create a global symbol at ccmram end */
  } >CCMRAM AT> FLASH

  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM



  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(4);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
/*     . = . + _Min_Stack_Size; */
    . = ALIGN(4);
  } >RAM

  

  /* Remove information from the standard libraries */
  /* OI_DATA_SECTIONS */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
