#ifndef OPERATION_RT_H
#define OPERATION_RT_H

#include "mpu_entry.h"
#include "basic_types.h"

// #define OPEC_MONITOR_CODE __attribute__((section(".opec_monitor")))
#define UPPER_ALIGNMENT(num, size) (((num) + (size) - 1) & ~((size)-1))
#define LOWER_ALIGNMENT(num, size) ((num) & ~((size)-1))


#define SHCSR *((volatile uint32_t *)0xE000ED24)
#define SCB_SHCSR_BUSFAULTENA_SHIFT         17U                                            /*!< SCB SHCSR: BUSFAULTENA Position */
#define SCB_SHCSR_BUSFAULTENA_MSK           (1UL << SCB_SHCSR_BUSFAULTENA_SHIFT)           /*!< SCB SHCSR: BUSFAULTENA Mask */
#define SCB_SHCSR_MEMFAULTENA_SHIFT         16U                                            /*!< SCB SHCSR: MEMFAULTENA Position */
#define SCB_SHCSR_MEMFAULTENA_MSK           (1UL << SCB_SHCSR_MEMFAULTENA_SHIFT)           /*!< SCB SHCSR: MEMFAULTENA Mask */


#define DEMCR *((volatile uint32_t *)0xE000EDFC)

#define SCS_CCR_STKALIGN_SHIFT              9U
#define SCS_CCR_STKALIGN_MSK                1U


//#define DROP_PRIVILEGES __asm("mrs %[reg], CONTROL" "orr %[reg], %[reg], #1" "msr CONTROL, %[reg]" : : [reg]"r": "memory")
#define DROP_PRIVILEGES __asm volatile("MRS r0, CONTROL\n" "ORR r0, r0, #1\n" "MSR CONTROL, r0\n" :::"r0","memory")
//#define SET_PRIVILEGES __asm("mrs %[reg], CONTROL" 	"orr %[reg], %[reg], #0" "msr CONTROL, %[reg]" : : [reg]"r": "memory")
#define SET_PRIVILEGES __asm volatile("MRS r0, CONTROL\n" "AND r0, r0, #0xFFFFFFFE\n" "msr CONTROL, r0\n" :::"r0","memory")
#define CHANGE_TO_PSP __asm volatile("MRS r0, CONTROL\n" "ORR r0, r0, #2\n" "MSR CONTROL, r0\n":::"r0", "memory")
#define CHANGE_TO_MSP __asm volatile("MRS r0, CONTROL\n" "AND r0, r0, #0xFFFFFFFD\n" "msr CONTROL, r0\n" :::"r0","memory")

#define OPEC_FAULT while(1){}

#define REG_R0  0
#define REG_R1  1
#define REG_R2  2
#define REG_R3  3
#define REG_R4  4
#define REG_R5  5
#define REG_R6  6
#define REG_R7  7
#define REG_R8  8
#define REG_R9  9
#define REG_R10  10
#define REG_R11  11
#define REG_R12  12
#define REG_SP  13
#define REG_LR  14
#define REG_PC  15
#define REG_PSR 16

#define IS_STORE    0
#define IS_LOAD     1

#define IS_UNSIGNED 0
#define IS_SIGNED   1

#define ONE_BYTE        1
#define ONE_HALFWORD    2
#define ONE_WORD        4

#define PC_2_STEP       2
#define PC_4_STEP       4

#define BOOL_TRUE   1
#define BOOL_FALSE  0

#define MPU_REGIONS 8
#define BASIC_REGIONS   4
#define PERIPHERAL_REGIONS  (MPU_REGIONS-BASIC_REGIONS)

#define SRAM_MEMORY_MASK    0xF0000000
#define SRAM_MEMORY_LABEL   0x20000000
#define PERIPHERAL_MEMORY_MASK
#define PERIPHERAL_MEMORY_LABEL 0x40000000

#define BITMAP_SIZE 20      /* 支持每个全局变20/2=10个指针的标记 */
#define BYTE_LEN     8      /* ARMv7m一个字节度为8 bit */
#define POINTER_SIZE 4      /* ARMv7m一个指针长度为4 bytes */

#define OPERATION_MAX   32


#define STACK_ALIGNMENT_8   8
#define STACK_ALIGNMENT_4   4

struct Operation_Data_Section {         // From policy file
    uint32_t addr;
    uint32_t size;
    uint32_t operation_id;
};


struct Operations_Data_Section_Info {
    uint32_t operation_data_section_num;
    struct Operation_Data_Section *operation_data_section_ptr;
};


struct Operation_Policy_Info {
    struct Operation_Policy *policy_addr;
    uint32_t operation_id;
};


struct Operation_Policies {
    uint32_t operation_policy_num;
    struct Operation_Policy_Info *operation_policy_info_ptr;
};

struct MPU_Region {
	uint32_t addr;
	uint32_t attr;
};


/* index2 > index1 && index2-index1+1=POINTER_SIZE才是有效的 */
struct Pointer_Field {
    uint32_t first_index;
    uint32_t second_index;
};


struct Shadow_Data_TBL {
	uint32_t attr;      // deprecated
	char **ptr_addr;
	char *sha_addr;
	char *org_addr;
	uint32_t size;      // size of shadow variable
    uint32_t need_sanitize;
    uint32_t offset_of_the_var_to_check;
    uint32_t min_value;
    uint32_t max_value;
    uint32_t has_pointer_fields;
    uint32_t pointer_fields_num;
    struct Pointer_Field *pointer_fields_ptr;
};


struct Shadow_Stack_TBL {
	uint32_t total_size;
	uint32_t ptr_num;
	struct {
		int type;		// type>=0, means that args are on stack, type is the arg's offset to psp; type<0, means that args are in regs, -1~-4, r0~r3
		int count;		// number of elements
		int size;		// size of each element
		char *org_addr;
		char *new_addr;
	}arg[0];
};


struct Sanitization_TBL {
    char **ptr_addr;
    uint32_t min;
    uint32_t max;
};


/**
 * @brief Peripheral_Regionlist is used for switching MPU regions when encountered a Memory Management Fault
 * start_addr: 	start address of the private peripheral
 * end_addr: 	end address of the private peripheral
 * attr:		attribute for uprivileged code, and the encoding rules are below:
 * 					  wr
 * 					0b00: 0: No Access
 * 					0b01: 1: Read-Only
 * 					0b10: 2: Write-only
 * 					0b11: 3: Write&Read
 * mpu_rbar:    MPU RBAR register value, the region number is set to 0
 * mpu_rasr:    MPU RASR register value
 */
struct Peripheral_MPU_Region {
    uint32_t start_addr;
    uint32_t attr;
    uint32_t size;
    uint32_t mpu_rbar;
    uint32_t mpu_rasr;
};


/**
 * @brief PPB_Register_Whitelist is used for checked in PPB access
 * start_addr: 	start address of the private peripheral
 * end_addr: 	end address of the private peripheral
 * attr:		attribute for uprivileged code, and the encoding rules are below:
 * 					  wr
 * 					0b00: 0: No Access
 * 					0b01: 1: Read-Only
 * 					0b10: 2: Write-only
 * 					0b11: 3: Write&Read
 * 
 */
struct PPB_Register_Whitelist {
	uint32_t start_addr;
	uint32_t size;
	uint32_t attr;
};


struct Operation_Policy {
	struct MPU_Region region[8];	// 8 MPU regions for Cortex-M4 MCU
    uint32_t current_operation_id;
    struct Operations_Data_Section_Info *OpeDataSecInfo_ptr;
	uint32_t stack_copy_size;
	struct Shadow_Stack_TBL *stbl;
    uint32_t peripehral_region_num;
    struct Peripheral_MPU_Region *regions_ptr;
	uint32_t ppb_whitelist_num;
	struct PPB_Register_Whitelist *ppb_ptr;
	uint32_t shadowdata_tbl_size;
	struct Shadow_Data_TBL shadowdata_tbl[0];
};


struct Operation_Metadata {
	struct Operation_Policy *policy;
	uint32_t stack_pointer;
};


/**
 * @brief Exception_Frame holds the value of exception entry saved registers
 * 
 */
struct Exception_Frame{
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t *lr;
    uint32_t *pc;
    uint32_t xPSR;
};


/**
 * @brief Reg_Frame holds all the registers value of registers before entering the exception handler
 * 
 */
struct Reg_Frame{
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t sp;    //r13
    uint32_t *lr;   //r14
    uint32_t *pc;   //r15
    uint32_t xPSR;
};


struct Instruction {
    uint32_t inst;              // instruction content
    uint32_t store_0_load_1;    // 0: store, 1: load
    uint32_t addr;              // store: addr to write to, load: addr to read from
    uint32_t reg;               // store: reg to read from, load: reg to write to
    uint32_t bytes_num;         // number of bytes to write or read
    uint32_t is_signed;         // 1: signed extened
    uint32_t pc_step;           // value added to pc
};


// extern uint32_t _estack;
// extern uint32_t _Min_Stack_Size;

// struct Operation_Metadata Operation_Stack;

void __operation_init_data_section(uint32_t * vma_start, uint32_t* ram_start,uint32_t *ram_end);

void construct_thumb2_inst(struct Instruction *thumb2_inst_struct, uint32_t store_0_load_1, uint32_t addr, uint32_t reg, uint32_t bytes_num, uint32_t is_signed, uint32_t pc_step);

#endif