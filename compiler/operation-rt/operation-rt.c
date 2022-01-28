#include <stdint.h>
#include <stddef.h>
#include "operation-rt.h"
#include "string.h"
#include "thumb.h"
#include "thumb2.h"

extern uint32_t _estack;
extern uint32_t _Min_Stack_Size;

struct Operation_Metadata Operation_Stack;
struct Operations_Data_Section_Info OpeDataSecInfo;
struct Operation_Policies *PoliciesInfo_ptr;
uint32_t __opec_syscall_registers[17];
uint32_t __opec_syscall_stack[200];
uint32_t stack_top = (uint32_t)&_estack - 4;
uint32_t stack_size = (uint32_t)&_Min_Stack_Size;
uint32_t next_region_id = BASIC_REGIONS;


#ifdef TIMER_OPEC
//timer
unsigned int TS_0, TS_1;			//timestamp
volatile unsigned int *DWT_CYCCNT  ;
volatile unsigned int *DWT_CONTROL ;
volatile unsigned int *SCB_DEMCR   ;

void _reset_timer(){
    DWT_CYCCNT   = (volatile unsigned int *)0xE0001004; //address of the register
    DWT_CONTROL  = (volatile unsigned int *)0xE0001000; //address of the register
    SCB_DEMCR    = (volatile unsigned int *)0xE000EDFC; //address of the register
    *SCB_DEMCR   = *SCB_DEMCR | 0x01000000;
    *DWT_CYCCNT  = 0; // reset the counter
    *DWT_CONTROL = 0; 
}

void _start_timer(){
    *DWT_CONTROL = *DWT_CONTROL | 1 ; // enable the counter
    TS_0 = *DWT_CYCCNT;
}

void _stop_timer(){
    TS_1 = *DWT_CYCCNT;
    *DWT_CONTROL = *DWT_CONTROL | 0 ; // disable the counter    
}

unsigned int _get_cycles(){
    return TS_1 - TS_0;
}

void _info_brkpt(){ 
    printf("\n");
}
#endif


#ifdef TIMER_OPEC
/* Initialization */
uint32_t __operation_init_data_section_EXE_time[32] = {0};
uint32_t __operation_init_data_section_CALL_count = 0;
uint32_t __clear_memh_memp_EXE_time[32] = {0};
uint32_t __clear_memh_memp_CALL_count = 0;
uint32_t __operation_start_EXE_time[32] = {0};
uint32_t __operation_start_CALL_count = 0;

/* SVC */
uint32_t SVC_Handler_Main_EXE_time[256] = {0};
uint32_t SVC_Handler_Main_CALL_count = 0;
uint32_t mpu_switch_EXE_time[32] = {0};
uint32_t mpu_switch_CALL_count = 0;
uint32_t shadow_data_writeback_EXE_time[32] = {0};
uint32_t shadow_data_writeback_CALL_count = 0;
uint32_t shadow_data_copy_EXE_time[32] = {0};
uint32_t shadow_data_copy_CALL_count = 0;
uint32_t do_value_sanitize_EXE_time[32] = {0};
uint32_t do_value_sanitize_CALL_count = 0;

/* Bus Fault */
uint32_t Busfault_Handler_EXE_time[256] = {0};
uint32_t Busfault_Handler_CALL_count = 0;

/* MemManage Fault */
uint32_t MemManage_Handler_EXE_time[512] = {0};
uint32_t MemManage_Handler_CALL_count = 0;
#endif


static inline uint32_t __get_PSP(void)
{
	uint32_t result;

	__asm volatile ("MRS %0, psp" : "=r" (result) );
	return(result);
}


static inline void __set_PSP(uint32_t psp)
{
	__asm volatile ("MSR psp, %0" : : "r" (psp) );
	return;
}


int is_SCB_reg(uint32_t addr)
{

	if (SCB_BASE <= addr && addr < SCB_BASE + sizeof(SCB_Type))
		return 0;
	else
		return -1;
}


#if 0
/*
 * @brief: Encode addr to mpu rbar register value
 * addr: 			peripheral start addr
 * mpu_region_id: 	the region to be re-configured
 * enable: 			enable or disable the MPU region
 */
uint32_t encode_mpu_rbar(uint32_t addr, uint32_t mpu_region_id, uint32_t enable) {
	addr = addr & 0xFFFFFFE0;
	mpu_region_id = mpu_region_id & 0xF;
	addr = addr | mpu_region_id;
	addr = addr | ((enable & 1) << 4);
	return addr;
}


uint32_t extract_region_start_addr(uint32_t rbar) {
	if (rbar == 0)
		return 0;
	else
		return (rbar & 0xFFFFFFE0);
}


uint32_t extract_region_size(uint32_t rasr) {
	if (rasr == 0)
		return 0;
	uint32_t size_bit = rasr & 0x3F >> 1;
	if ((size_bit >= 0b100) && (size_bit <= 0b1111)) {
		if (size_bit == 31) {
			// return 4*1024*1024*1024;
			OPEC_FAULT
		}
		else
			return 1<<(size_bit+1);		// may overflow
	}
	else {
		OPEC_FAULT
	}
}
#endif


/**
 * @brief do peripheral region switch
 * 
 * @param regions_ptr peripheral regionlist pointer
 * @param switch_in_region the peripheral region to be switched into MPU
 * @return int 0: successful, -1: fail
 */
int do_region_switch(struct Peripheral_MPU_Region* regions_ptr, struct Peripheral_MPU_Region* switch_in_region) {
	MPU_Type* MPU_regs = ((MPU_Type*)MPU_BASE);
	uint32_t selected_rbar;
	uint32_t success = 0;

	/* clear the used bit of switched out region */
#if 0
	selected_rbar = MPU_regs->RBAR & 0xFFFFFFE0;
	for (uint32_t i=0; i<Operation_Stack.policy->peripehral_region_num; i++) {	// exclude peripheral regions not in use
		if (selected_rbar == (regions_ptr+i)->mpu_rbar)
			success = 1;
			break;
	}
	if (success == 0)
		return -1;
#endif

	/*关中断*/
	__asm volatile ("cpsid i" : : : "memory");
	__asm volatile ("cpsid f" : : : "memory");

	/* switch MPU region */
	HAL_MPU_Disable();
	MPU_regs->RNR = next_region_id & 0x7;
	MPU_regs->RASR = MPU_regs->RASR & 0xFFFFFFFE;	// disable MPU region before updating
	MPU_regs->RBAR = switch_in_region->mpu_rbar | next_region_id | (1<<4);
	MPU_regs->RASR = switch_in_region->mpu_rasr;
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
	/* update next_region_id, range 4-7, round-robin */
	next_region_id = (next_region_id - BASIC_REGIONS + 1) % PERIPHERAL_REGIONS + BASIC_REGIONS;

	/*开中断*/
	__asm volatile ("cpsie f" : : : "memory");
	__asm volatile ("cpsie i" : : : "memory");
	return 0;
}


/**
 * @brief Check if the peripheral is allowed to access
 * 
 * @param peripheral_addr peripheral to access
 * @return int 0: allowed, -1: forbidden
 */
int check_and_do_region_switch(uint32_t peripheral_addr) {
	struct Peripheral_MPU_Region *regions_ptr, *cur_region;
	regions_ptr = Operation_Stack.policy->regions_ptr;
	uint32_t peripehral_region_num, i;
	peripehral_region_num = Operation_Stack.policy->peripehral_region_num;
	for (i=0; i<peripehral_region_num; i++) {
		cur_region = regions_ptr+i;
		if ((cur_region->start_addr <= peripheral_addr) && (peripheral_addr < (cur_region->start_addr + cur_region->size))) {
			if (do_region_switch(regions_ptr, cur_region) == 0)
				return 0;
		}
	}
	return -1;
}


/**
 * @brief check if the ppb is allowed to access
 * 
 * @param ppb_addr private peripheral bus register addr
 * @return int 0: allowed, -1: forbidden
 */
int check_valid_ppb(uint32_t ppb_addr) {
	struct PPB_Register_Whitelist* ppb_ptr, *cur_ppb;
	uint32_t ppb_num, allowed;
	ppb_ptr = Operation_Stack.policy->ppb_ptr;
	ppb_num = Operation_Stack.policy->ppb_whitelist_num;
	// check if the SCB is in range
	for (uint32_t i=0; i<ppb_num; i++) {
		cur_ppb = ppb_ptr+i;
		uint32_t attr = cur_ppb->attr;
		if ((cur_ppb->start_addr <= ppb_addr) && (ppb_addr < (cur_ppb->start_addr + cur_ppb->size))) {
			/* peripheral_addr not in addr range, return -1 */
			allowed = 1;
			break;
		}
	}
	if (allowed == 0)
		return -1;
	return 0;

#if 0
	/* store_0_load_1
	 *   wr
	 * 0b01: load
	 * 0b10: store
	 */
	// (2) check if the store/load access is allowed
	if (store_0_load_1 == 0) {
		/*
		* store_0_load_1=0, write to ppb register
		* transfer store_0_load_1 to 0b10
		*/
		store_0_load_1 = 2;
	}
		/*
		* store_0_load_1=1, read from ppb register
		* no change to store_0_load_1
		*/
	if ((store_0_load_1&attr) == 0) {
		// attribute not match
		return -1;
	}
	return 0;
#endif
}


/**
 * @brief do_peripheral_rw emulate the load/store instructions which accessed PPB
 * 
 * @param inst_struct 
 * @param Regs Register array
 * @return int 0: successful, -1: fail
 */
int do_peripheral_rw(struct Instruction *inst_struct, uint32_t *Regs) {
	uint32_t *ptr_32;
	uint16_t *ptr_16;
	uint8_t *ptr_8;
	if (inst_struct->store_0_load_1 == IS_STORE) {
		switch (inst_struct->bytes_num) {
			case 4:
				*(uint32_t *)(inst_struct->addr) = Regs[inst_struct->reg];
				break;

			case 2:
				*(uint16_t *)(inst_struct->addr) = (uint16_t)Regs[inst_struct->reg] & 0xFFFF;
				break;

			case 1:
				*(uint8_t *)(inst_struct->addr) = (uint8_t)Regs[inst_struct->reg] & 0xFF;
				break;

			default:
				OPEC_FAULT
		}
	}
	else if ((inst_struct->store_0_load_1 == IS_LOAD) && (inst_struct->is_signed == IS_UNSIGNED)) {	// load, not sign extended
		switch (inst_struct->bytes_num) {
			case 4:
				Regs[inst_struct->reg] = *(uint32_t *)(inst_struct->addr);
				break;

			case 2:
				Regs[inst_struct->reg] = *(uint32_t *)(inst_struct->addr) & 0xFFFF;
				break;

			case 1:
				Regs[inst_struct->reg] = *(uint32_t *)(inst_struct->addr) & 0xFF;
				break;

			default:
				OPEC_FAULT
				return -1;
		}
	}
	else if ((inst_struct->store_0_load_1 == IS_LOAD) && (inst_struct->is_signed == IS_SIGNED)) {	// load, and sign extended
		switch (inst_struct->bytes_num) {
			case 4:
				OPEC_FAULT
				return -1;

			case 2:
				if (*(uint32_t *)(inst_struct->addr) & (1 << (HALFWDLEN - 1)))
					Regs[inst_struct->reg] = *(uint32_t *)(inst_struct->addr) | (~((1 << HALFWDLEN) - 1));
				else
					Regs[inst_struct->reg] = *(uint32_t *)(inst_struct->addr) & 0xFFFF;

				break;

			case 1:
				if (*(uint32_t *)(inst_struct->addr) & (1 << (BTLEN - 1)))
					Regs[inst_struct->reg] = *(uint32_t *)(inst_struct->addr) | (~((1 << BTLEN) - 1));
				else
					Regs[inst_struct->reg] = *(uint32_t *)(inst_struct->addr) & 0xFF;

				break;

			default:
				OPEC_FAULT
				return -1;
		}
	}
	else {
		OPEC_FAULT
		return -1;
	}
	return 0;
}


void construct_thumb2_inst(struct Instruction *inst_struct, uint32_t store_0_load_1, uint32_t addr, uint32_t reg, uint32_t bytes_num, uint32_t is_signed, uint32_t pc_step) {
	inst_struct->store_0_load_1 = store_0_load_1;
	inst_struct->addr = addr;
	inst_struct->reg = reg;
	inst_struct->bytes_num = bytes_num;
	inst_struct->is_signed = is_signed;
	inst_struct->pc_step = pc_step;
	return;
}


uint32_t parse_inst(struct Instruction *inst_struct, uint32_t *Regs) {
	/* Armv7m ARch reference Section A5 */
	uint16_t thumb_inst, opcode, first_hw, second_hw;
	first_hw = inst_struct->inst & 0xFFFF;
	second_hw = inst_struct->inst >> 16 & 0xFFFF;

	switch (first_hw>>11) {
		case 0b11101:
		case 0b11110:
		case 0b11111:
			thumb2_disasm(inst_struct, Regs);
			// OPEC_FAULT
			break;
		
		default:
			thumb_disasm(inst_struct, Regs);
			break;
	}
	return 0;
}


void read_stack_frame(struct Exception_Frame *Stack, struct Reg_Frame *Regs) {
	Regs->r0 = Stack->r0;
	Regs->r1 = Stack->r1;
	Regs->r2 = Stack->r2;
	Regs->r3 = Stack->r3;
	Regs->r12 = Stack->r12;
	Regs->pc = Stack->pc;
	Regs->lr = Stack->lr;
	Regs->xPSR = Stack->xPSR;
}


void write_stack_frame(struct Exception_Frame *Stack, struct Reg_Frame *Regs) {
	Stack->r0 = Regs->r0;
	Stack->r1 = Regs->r1;
	Stack->r2 = Regs->r2;
	Stack->r3 = Regs->r3;
	Stack->r12 = Regs->r12;
	Stack->pc = Regs->pc;
	Stack->lr = Regs->lr;
	Stack->xPSR = Regs->xPSR;
}


uint32_t do_value_sanitize(struct Shadow_Data_TBL shadowdata_tbl)
{
	if ((shadowdata_tbl.min_value > shadowdata_tbl.sha_addr[shadowdata_tbl.offset_of_the_var_to_check]) || (shadowdata_tbl.sha_addr[shadowdata_tbl.offset_of_the_var_to_check] > shadowdata_tbl.max_value)) {
		return -1;
	}
	return 0;
}


void shadow_data_writeback(unsigned int shadowdata_tbl_size, struct Shadow_Data_TBL *shadowdata_tbl)
{
// #ifdef TIMER_OPEC
// 	_reset_timer();
// 	_start_timer();
// #endif
	unsigned int i, j;
	for (i=0; i<shadowdata_tbl_size; i++) {
		/* update the pointer */
		*(shadowdata_tbl[i].ptr_addr) = shadowdata_tbl[i].org_addr;

		/* sanitize the value */
		if (shadowdata_tbl[i].need_sanitize) {
			if (do_value_sanitize(shadowdata_tbl[i]) != 0) {
				OPEC_FAULT
			}
		}

		/* copy back the value from operation data section to .data section */
		for (j=0; j<shadowdata_tbl[i].size; j++) {
			shadowdata_tbl[i].org_addr[j] = shadowdata_tbl[i].sha_addr[j];
		}
	}
// #ifdef TIMER_OPEC
// 	_stop_timer();
// 	shadow_data_writeback_EXE_time[shadow_data_writeback_CALL_count++] = _get_cycles();
// #endif
	return;
}


/**
 * @brief given a pointer to a certain field of a shadow variable of an operation, return the address of that field of the original variable
 * 
 * @param operation_id the id of the operation that the variable pointer_to_check points to
 * @param pointer_to_check points to the shadow variable of an operation
 * @return uint32_t* 
 */
uint32_t *get_data_original(uint32_t operation_id, uint32_t *pointer_to_check) {
	for (int i=0; i<PoliciesInfo_ptr->operation_policy_num; i++) {
		if (operation_id == (PoliciesInfo_ptr->operation_policy_info_ptr+i)->operation_id) {
			struct Operation_Policy *policy_ptr;
			struct Shadow_Data_TBL *shadowdata_ptr;
			policy_ptr = (PoliciesInfo_ptr->operation_policy_info_ptr+i)->policy_addr;
			shadowdata_ptr = policy_ptr->shadowdata_tbl;
			uint32_t shadowdata_tbl_num = policy_ptr->shadowdata_tbl_size;
			for (int j=0; j<shadowdata_tbl_num; j++) {
				uint32_t shadow_addr = (uint32_t)(shadowdata_ptr+j)->sha_addr;
				uint32_t shadow_size = (shadowdata_ptr+j)->size;
				if ((shadow_addr <= (uint32_t)pointer_to_check) && ((uint32_t)pointer_to_check < (shadow_addr+shadow_size))) {
					uint32_t offset = (uint32_t)pointer_to_check - shadow_addr;
					uint32_t original_addr = (uint32_t)(shadowdata_ptr+j)->org_addr + offset;
					return (uint32_t *)original_addr;
				}
			}
		}
	}
	return 0;
}


/**
 * @brief Given a pointer to a certain field of a original variable of an operation, return the address of that field of the shadow variable
 * 
 * @param operation_id the id of the current operation
 * @param data_original points to the original
 * @return uint32_t* 
 */
uint32_t *get_data_shadow_current(uint32_t operation_id, uint32_t *data_original) {
	for (int i=0; i<PoliciesInfo_ptr->operation_policy_num; i++) {
		if (operation_id == (PoliciesInfo_ptr->operation_policy_info_ptr+i)->operation_id) {
			struct Operation_Policy *policy_ptr;
			struct Shadow_Data_TBL *shadowdata_ptr;
			policy_ptr = (PoliciesInfo_ptr->operation_policy_info_ptr+i)->policy_addr;
			shadowdata_ptr = policy_ptr->shadowdata_tbl;
			uint32_t shadowdata_tbl_num = policy_ptr->shadowdata_tbl_size;
			for (int j=0; j<shadowdata_tbl_num; j++) {
				uint32_t original_addr = (uint32_t)(shadowdata_ptr+j)->org_addr;
				uint32_t original_size = (shadowdata_ptr+j)->size;
				if ((original_addr <= (uint32_t)data_original) && ((uint32_t)data_original < original_addr+original_size)) {
					uint32_t shadow_addr = (uint32_t)(shadowdata_ptr+j)->sha_addr;
					uint32_t offset = (uint32_t)data_original - original_addr;
					return (uint32_t *)(shadow_addr+offset);
				}
			}
		}
	}
	return 0;
}


/**
 * @brief check wether the global variable has pointer fields and point to other operation data section
 * 
 * @param shadow_variable 
 * @param first_index 
 * @param second_index 
 */
void check_and_update_pointer(struct Shadow_Data_TBL shadow_variable, uint16_t first_index, uint16_t second_index) {
	uint32_t *data_original, *data_shadow;
	if (second_index-first_index != 3)
		OPEC_FAULT
	/* pointer_to_shadow: value of the global variable pointer field */
	uint32_t **pointer_to_field = (uint32_t **)(shadow_variable.org_addr+first_index);		// points to the fields of shadow variable
	uint32_t *pointer_to_shadow = *pointer_to_field;										// points to the shadow variable
	/* we only need to fix pointers that point to other operation's data section */
	if ((SRAM_MEMORY_MASK & (uint32_t)pointer_to_shadow) != SRAM_MEMORY_LABEL) {
		return;
	}
	/* find out the operation data section that shadow_variable resides */
	for (int i=0; i<OpeDataSecInfo.operation_data_section_num; i++) {
		uint32_t addr = (OpeDataSecInfo.operation_data_section_ptr+i)->addr;
		uint32_t size = (OpeDataSecInfo.operation_data_section_ptr+i)->size;
		uint32_t opid = (OpeDataSecInfo.operation_data_section_ptr+i)->operation_id;
		if ((addr <= (uint32_t)pointer_to_shadow) && ((uint32_t)pointer_to_shadow <= (addr+size))) {
			if (opid != Operation_Stack.policy->current_operation_id) {
				/* get the data original */
				if ((data_original = get_data_original(opid, pointer_to_shadow)) > 0) {
					if ((data_shadow = get_data_shadow_current(Operation_Stack.policy->current_operation_id, data_original)) > 0) {
						*pointer_to_field = data_shadow;
						break;
					}
				}
			}
		}
	}
	return;
}


void shadow_data_copy(unsigned int shadowdata_tbl_size, struct Shadow_Data_TBL *shadowdata_tbl)
{
// #ifdef TIMER_OPEC
// 	_reset_timer();
// 	_start_timer();
// #endif
	unsigned int i, j;
	int result;
	for(i=0; i<shadowdata_tbl_size; i++) {
		*(shadowdata_tbl[i].ptr_addr) = shadowdata_tbl[i].sha_addr;		/* modify pointer */
		if(shadowdata_tbl[i].has_pointer_fields) {						/* fix pointer field */
			for(j=0; j<shadowdata_tbl[i].pointer_fields_num; j++) {
				struct Pointer_Field *pointer_field = shadowdata_tbl[i].pointer_fields_ptr+j;
				check_and_update_pointer(shadowdata_tbl[i], pointer_field->first_index, pointer_field->second_index);
			}
		}
		for(j=0; j<shadowdata_tbl[i].size; j++) {						/* copy value */
			shadowdata_tbl[i].sha_addr[j] = shadowdata_tbl[i].org_addr[j];
		}
	}
// #ifdef TIMER_OPEC
// 	_stop_timer();
// 	shadow_data_copy_EXE_time[shadow_data_copy_CALL_count++] = _get_cycles();
// #endif
	return;
}


/* i is the start mpu region */
void mpu_simple_apply(uint8_t i, struct MPU_Region region[REGION_NUM])
{
	//HAL_MPU_Disable();
	for(; i<REGION_NUM; i++){
		MPU->RNR = i;
		MPU->RASR = MPU->RASR & 0xFFFFFFFE;		// disable MPU region before updating
		MPU->RBAR = region[i].addr;
		MPU->RASR = region[i].attr;
	}
	//HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}


void mpu_config_subregion(uint8_t num, uint8_t mask)
{
	uint32_t attr;
	MPU->RNR = num;
	attr = MPU->RASR;
	*((uint8_t *)&attr + 1) = mask;
	MPU->RASR = attr;
}


void mpu_switch(int type, uint8_t sub_mask, struct MPU_Region region[REGION_NUM])
{
// #ifdef TIMER_OPEC
// 	_reset_timer();
// 	_start_timer();
// #endif
	uint8_t sub_number;
	if (type) { //no-default operation need to calculate the stack
		HAL_MPU_Disable();
		mpu_config_subregion(2, sub_mask);
		mpu_simple_apply(3, region);
		HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
	} else { //default operation
		HAL_MPU_Disable();
		mpu_simple_apply(2, region);
		HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
	}
// #ifdef TIMER_OPEC
// 	_stop_timer();
// 	mpu_switch_EXE_time[mpu_switch_CALL_count++] = _get_cycles();
// #endif
	return;
}


void modify_stack_pointer(uint32_t psp) {
	/*这时候我们还没有修改栈指针，栈指针的修改只能在线程模式下，完成*/
	/*开中断*/
	__asm volatile ("cpsie f" : : : "memory");
	__asm volatile ("cpsie i" : : : "memory");

	/*修改PSP的栈内容和指针，用于返回*/
	__asm volatile (
		//copy psp stack to new position
		"push {r4-r7, r11, r12}\n"	//保存其寄存器
		"Mov r11, %0\n"
		"MRS r12, psp\n"
		"ldmia r12, {r0-r7}\n"		//读取原来压入栈中的上下文	pop
		"stmdb r11!, {r0-r7}\n"		//将原来stacking的数据移动压入新的栈中 push
		"MSR psp, r11\n"			//修改psp的值		
		"pop{r4-r7, r11, r12}\n"
		:
		:"r"(psp)
		:"r0","memory"
	);
}


void HardFault_Handler(void) {
	__asm volatile (
		".global HardFault_Handler_Main\n\t"
		"ldr r12, =__opec_syscall_registers \n\t"
		"stmia r12!, {r0-r11} \n\t"					// __opec_syscall_registers保存了r0-r11
		"mrs r0, psp \n\t"							// r0 point to stack frame
		"ldr r1, =__opec_syscall_registers \n\t"	// r1 point to reg frame
		"push {lr} \n\t"
		"bl HardFault_Handler_Main\n\t"
		"pop {lr} \n\t"
		"ldr r12, =__opec_syscall_registers+(4*4)\n\t"		// recover r4-r11. r0-r3, r12, pc, lr, xPSR already synced to stack_frame
		"ldmia r12, {r4-r11} \n\t"
		"bx lr\n\t"
	);
}


void HardFault_Handler_Main(struct Exception_Frame *Stack, struct Reg_Frame *Regs) {
	SCB_Type* SCB_regs = ((SCB_Type*)SCB_BASE);
	OPEC_FAULT
}


void BusFault_Handler(void)
{
#if 0
	__asm volatile (
		"mrs sp, psp\n\t"
		"pop {r0-r3}\n\t" 							//restore IRQ stack frame r0-r3
		"ldr r12, =__opec_syscall_registers \n\t"
		"stmia r12!, {r0-r11} \n\t"   				// __opec_syscall_registers保存了r0-r11
		"pop {r0,r2,r3,r4} \n\t"  					// r0=hard_args[r12], r2=hard_args[LR], r3=hard_args[pc], r4=hard_args[xPSR]
		"mrs r1, msp \n\t"							// r1=hard_args[sp]=msp
		"stmia r12!, {r0-r4} \n\t"  				// r0=hard_args[r12], r1=hard_args[sp]=msp, r2=hard_args[LR], r3=hard_args[pc], r4=hard_args[xPSR], where 现在的r12保存的__opec_syscall_registers, 一共Push了17 regs
		"mov r0, r3 \n\t"							// r3 points to pc i.e., instruction to be executed, stored during exception entry
		"ldr r1, =__opec_syscall_registers \n\t"			// r1保存的是stack frame值等同于R12
		"ldr sp, =__opec_syscall_stack+(100*4) \n\t"		// sp保存的是exception stack的位置
		"push {lr} \n\t"
		"bl BusFault_Handler_Main\n\t"
		"pop {lr} \n\t"
		"ldr r12, =__opec_syscall_registers+(13*4)\n\t"		// regs top
		"ldmia r12, {r0-r3} \n\t"
		"mov sp, r0 \n\t"									// recover sp
		"push {r1-r3}\n\t"									// 将xPSR, PC, LR压入hard_args的栈上
		"ldr r12, =__opec_syscall_registers \n\t"
		"ldmia r12!, {r0-r11} \n\t"
		"ldr r12, [r12] \n\t"
		"push {r0-r3,r12} \n\t"								// 将r0-r3, r12压入hard_args的栈上
		"bx lr\n\t"
	);
#endif

	__asm volatile (
		"ldr r12, =__opec_syscall_registers \n\t"
		"stmia r12!, {r0-r11} \n\t"					// __opec_syscall_registers保存了r0-r11
		"mrs r0, psp \n\t"							// r0 point to stack frame
		"ldr r1, =__opec_syscall_registers \n\t"	// r1 point to reg frame
		"push {lr} \n\t"
		"bl BusFault_Handler_Main\n\t"
		"pop {lr} \n\t"
		"ldr r12, =__opec_syscall_registers+(4*4)\n\t"		// recover r4-r11. r0-r3, r12, pc, lr, xPSR already synced to stack_frame
		"ldmia r12, {r4-r11} \n\t"
		"bx lr\n\t"
	);
}


void BusFault_Handler_Main(struct Exception_Frame *Stack, struct Reg_Frame *Regs)
{
#ifdef TIMER_OPEC
	_reset_timer();
	_start_timer();
#endif
	SCB_Type* SCB_regs = ((SCB_Type*)SCB_BASE);
	struct Instruction inst_struct;
	read_stack_frame(Stack, Regs);
	inst_struct.inst = *(uint32_t *)(Regs->pc);

	/*
	 * Bit 15 BFARVALID: Bus Fault Address Register (BFAR) valid flag.
	 * Bit 9 PRECISERR: Precise data bus error. When the processor sets this bit is 1, it writes the faulting address to the BFAR.
	 * BFSR == 0x82
	 */
	if (((SCB_regs->CFSR >> 0x8) & 0xFF) != 0x82) {
		OPEC_FAULT
	}

	if (parse_inst(&inst_struct, (uint32_t *)Regs) != 0) {
		OPEC_FAULT
	}

	// check whther the target peripheral is allowed to access
	if (check_valid_ppb(SCB_regs->BFAR) != 0) {
		OPEC_FAULT
	}

	if (do_peripheral_rw(&inst_struct, (uint32_t *)Regs) != 0) {
		OPEC_FAULT
	}
	
	/* clear the CFSR by writing 1 to the bits set by MCU */
	SCB_regs->CFSR = 0x82 << 8;
	Regs->pc = (uint32_t *)((uint8_t *)Regs->pc + inst_struct.pc_step);
	write_stack_frame(Stack, Regs);
#ifdef TIMER_OPEC
	_stop_timer();
	Busfault_Handler_EXE_time[Busfault_Handler_CALL_count++] = _get_cycles();
#endif
}


void MemManage_Handler(void)
{
	__asm volatile (
		".global MemManage_Handler_Main\n\t"
		"ldr r12, =__opec_syscall_registers \n\t"
		"stmia r12!, {r0-r11} \n\t"					// __opec_syscall_registers保存了r0-r11
		"mrs r0, psp \n\t"							// r0 point to stack frame
		"ldr r1, =__opec_syscall_registers \n\t"	// r1 point to reg frame
		"push {lr} \n\t"
		"bl MemManage_Handler_Main\n\t"
		"pop {lr} \n\t"
		"ldr r12, =__opec_syscall_registers+(4*4)\n\t"		// recover r4-r11. r0-r3, r12, pc, lr, xPSR already synced to stack_frame
		"ldmia r12, {r4-r11} \n\t"
		"bx lr\n\t"
	);
}


void MemManage_Handler_Main(struct Exception_Frame *Stack, struct Reg_Frame *Regs) {
#ifdef TIMER_OPEC
	_reset_timer();
	_start_timer();
#endif
	SCB_Type* SCB_regs = ((SCB_Type*)SCB_BASE);
	MPU_Type* MPU_regs = ((MPU_Type*)MPU_BASE);
	read_stack_frame(Stack, Regs);

	/* 
	 * Bit 7 MMARVALID: Memory Management Fault Address Register (MMAR) valid flag.
	 * Bit 1 DACCVIOL: Data access violation flag.
	 * MFSR == 0x82
	 */
	if ((SCB_regs->CFSR & 0x82) != 0x82) {
		OPEC_FAULT
	}

	if (check_and_do_region_switch(SCB_regs->MMFAR) != 0) {
		OPEC_FAULT
	}

	/* clear the CFSR by writing 1 to the bits set by MCU */
	SCB_regs->CFSR = 0x82;
#ifdef TIMER_OPEC
	_stop_timer();
	MemManage_Handler_EXE_time[MemManage_Handler_CALL_count++] = _get_cycles();
#endif
}


/**
 * @brief Clear registers(except for sp, pc, lr, xPSR) before exiting an operation
 * 
 * @param Regs 
 */
void clear_regs(struct Reg_Frame *Regs) {
	Regs->r0 = 0;
	Regs->r1 = 0;
	Regs->r2 = 0;
	Regs->r3 = 0;
	Regs->r4 = 0;
	Regs->r5 = 0;
	Regs->r6 = 0;
	Regs->r7 = 0;
	Regs->r8 = 0;
	Regs->r9 = 0;
	Regs->r10 = 0;
	Regs->r11 = 0;
	Regs->r12 = 0;
}


/**
 * @brief Sets up the first operation and enters it.  Call function automatically
added by compiler before main executes by compiler.
 */
void SVC_Handler(void)
{
	#if 0
	__asm volatile (
		".global SVC_Handler_Main\n"
		"tst lr, #4\n"
		"ite EQ\n"
		"mrseq r0, MSP\n"
		"mrsne r0, PSP\n"
		"mov r1, r0\n"
		"b SVC_Handler_Main\n"
	);
	#else
	__asm volatile (
		".global SVC_Handler_Main\n\t"
		"ldr r12, =__opec_syscall_registers \n\t"
		"stmia r12!, {r0-r11} \n\t"					// __opec_syscall_registers保存了r0-r11
		"mrs r0, psp \n\t"							// r0 point to stack frame
		"ldr r1, =__opec_syscall_registers \n\t"	// r1 point to reg frame
		"push {lr} \n\t"
		"bl SVC_Handler_Main\n\t"
		"pop {lr} \n\t"
		"ldr r12, =__opec_syscall_registers+(4*4)\n\t"		// recover r4-r11. r0-r3, r12, pc, lr, xPSR already synced to stack_frame
		"ldmia r12, {r4-r11} \n\t"
		"bx lr\n\t"
	);
	#endif
}


/**
 * @brief  Stack contains:
 * 				r0, r1, r2, r3, r12, r14, the return address and xPSR
 * 				First argument (r0) is svc_args[0]
 * 
 * @param Stack Exception stacked registers
 */
void SVC_Handler_Main(struct Exception_Frame *Stack, struct Reg_Frame *Regs) {
#ifdef TIMER_OPEC
	_reset_timer();
	_start_timer();
#endif
	SCB_Type* SCB_regs = ((SCB_Type*)SCB_BASE);
	MPU_Type* MPU_regs = ((MPU_Type*)MPU_BASE);
	struct Operation_Policy *policy;
	struct Shadow_Stack_TBL *stack_table;
	uint32_t psp, sub_number, sub_mask = 0xff;
	int i, j;
	char *org_sp, *new_sp;
	uint32_t *arg_org_pos;
	uint8_t svc_number = *(((uint8_t *)Stack->pc)-2);
	//svc_number = (( char * )svc_args[ 6 ])[-2];
	/*关中断*/
	__asm volatile ("cpsid i" : : : "memory");
	__asm volatile ("cpsid f" : : : "memory");

	switch(svc_number) {
	/* Operation Entry */
	case 100:
		/* step 1: 保存unprivileged mode的stack pointer(psp) */
		Operation_Stack.policy = policy = (struct Operation_Policy *)(*(Stack->pc));
		if (Stack->xPSR & 0x200)
			psp = __get_PSP() + 36;
		else
			psp = __get_PSP() + 32;
		Operation_Stack.stack_pointer = psp;
		OpeDataSecInfo = *Operation_Stack.policy->OpeDataSecInfo_ptr;
		PoliciesInfo_ptr = (struct Operation_Policies *)(*(Stack->pc+1));
		org_sp = (char *)psp;
		/* 因为在svc指令之后插入了2个指针, 所以pc长度需要加4*2 */
		Stack->pc += 2;

		/* step 2: 同步shadow data, 修改ptr指针. TODO: copy on write */
		shadow_data_copy(policy->shadowdata_tbl_size, policy->shadowdata_tbl);

		/* step 3: 计算新的栈指针 */
		sub_number = (stack_top - psp + ((stack_size >> 3) - 1)) / (stack_size >> 3);	//总共8个sub region，计算消耗掉几个
		sub_mask = ~(sub_mask >> sub_number);
		new_sp = (char *)(stack_top - (stack_size >> 3) * sub_number);

		/* step4: 拷贝指针类型变量指向的位置(如果是字符串数组怎么办?参考SGX方法) */
		stack_table = policy->stbl;
		if (stack_table) {
			char *tmp_sp;
			new_sp -= stack_table->total_size;
			new_sp = (char *)LOWER_ALIGNMENT((uint32_t)new_sp, 8);
			tmp_sp = new_sp;
			for(i=0; i<stack_table->ptr_num; i++) {
				/* 获取参数指针的值 */
				if (stack_table->arg[i].type < 0) {
					/* 参数指针在寄存器中 */
					stack_table->arg[i].org_addr = (char *)((uint32_t *)Stack + (~stack_table->arg[i].type)*4);
				} else {
					/* 参数指针在栈上 */
					stack_table->arg[i].org_addr = *((char **)(org_sp + stack_table->arg[i].type));	
				}
				j = stack_table->arg[i].count * stack_table->arg[i].size;
				/* copy需要的size到栈上 */
				memcpy(tmp_sp, stack_table->arg[i].org_addr, j);
#if 0
				for(j=0; j<stack_table->arg[i].count * stack_table->arg[i].size; j++) {
					tmp_sp[j] = stack_table->arg[i].org_addr[j];
				}
#endif
				stack_table->arg[i].new_addr = tmp_sp;
				tmp_sp += j;
			}
			/* step5: 拷贝超过4个参数以后压到栈上的参数 */
			new_sp -= policy->stack_copy_size;
			for(i=0; i<policy->stack_copy_size; i++)
				new_sp[i] = org_sp[i];

			/* step6: 修改参数中指针变量的值 */
			for(i=0; i<stack_table->ptr_num; i++) {
				if (stack_table->arg[i].type < 0) {
					arg_org_pos = (uint32_t *)Stack + (~stack_table->arg[i].type);
				} else {
					arg_org_pos = (uint32_t *)(new_sp + stack_table->arg[i].type);
				}
				*arg_org_pos = (uint32_t)stack_table->arg[i].new_addr;
			}
		} else {
			new_sp -= policy->stack_copy_size;
			new_sp = (char *)LOWER_ALIGNMENT((uint32_t)new_sp, 8);
			memcpy(new_sp, org_sp, policy->stack_copy_size);
#if 0
			for(i=0; i<policy->stack_copy_size; i++) {
				new_sp[i] = org_sp[i];
			}
#endif
		}
		psp = (uint32_t)new_sp;
		/* 切换MPU配置，计算新的栈指针位置 */
		mpu_switch(1, sub_mask, policy->region);
		// modify_stack_pointer(psp);
		break;

	/* Operation Exit */
	case 101:
		/* step 1: 获取当前Operation的shadow table */
		policy = Operation_Stack.policy;		// svc_number = operation_id
		/* step 2: 用户指定的数据进行value check */


		/* step 3: 同步shadow data和修改ptr指针 */
		shadow_data_writeback(policy->shadowdata_tbl_size, policy->shadowdata_tbl);
		/* step 4: 恢复到之前的栈 */
		if (Stack->xPSR & 0x200) {
			psp = Operation_Stack.stack_pointer - 4;
		} else {
			psp = Operation_Stack.stack_pointer;
		}	
		/* step 5: 将拷贝的指针类型变量的值拷贝回去 */
		stack_table = policy->stbl;
		if (stack_table) {
			for(i=0; i<stack_table->ptr_num; i++) {
				memcpy(stack_table->arg[i].org_addr, stack_table->arg[i].new_addr, stack_table->arg[i].count * stack_table->arg[i].size);
#if 0
				for(j=0; j<stack_table->arg[i].count * stack_table->arg[i].size; j++) {
					
					stack_table->arg[i].org_addr[j] = stack_table->arg[i].new_addr[j];
				}
#endif
			}
		}
		/* step 6: 切换MPU配置，计算新的栈指针位置 */
		mpu_switch(0, sub_mask, policy->region);
		clear_regs(Regs);
		// modify_stack_pointer(psp);
		break;
	default:	// unknow SVC number
		OPEC_FAULT
		break;
	}
#ifdef TIMER_OPEC
	_stop_timer();
	SVC_Handler_Main_EXE_time[SVC_Handler_Main_CALL_count++] = _get_cycles();
#endif
	/* 这时候我们还没有修改栈指针，栈指针的修改只能在线程模式下，完成 */
	/* 开中断 */
	__asm volatile ("cpsie f" : : : "memory");
	__asm volatile ("cpsie i" : : : "memory");
	/* 修改PSP的栈内容和指针，用于返回 */
	__asm volatile (
		//copy psp stack to new position
		"push {r4-r7, r11, r12}\n"	// 保存其寄存器
		"mov r11, %0\n"
		"mrs r12, psp\n"
		"ldmia r12, {r0-r7}\n"		// 读取原来压入栈中的上下文	pop
		"stmdb r11!, {r0-r7}\n"		// 将原来stacking的数据移动压入新的栈中 push
		"msr psp, r11\n"			// 修改psp的值
		"pop{r4-r7, r11, r12}\n"
		:
		:"r"(psp)
		:"r0","memory"
	);
}


void DebugMon_Handler(void)
{}


void __operation_start(struct Operation_Policy *default_operation)
{
#ifdef TIMER_OPEC
	_reset_timer();
	_start_timer();
#endif
	uint32_t psp;
	//struct Operation_Policy *default_operation = locate_operation_policy();
	//stack_base = default_operation->region[2].addr;
	//stack_size = 0x2 << ((default_operation->region[2].attr & MPU_RASR_SIZE_Msk) >> MPU_RASR_SIZE_Pos);

	SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;		//Enable MemManage_Fault
	SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;		//Enable BUSManage_Fault
	DEMCR |= 0x10000;  						//Enable the debug monitor
	//SHPR1 |= SCB_SHPR1_BUSFAULTPRIO_Msk;

	psp = stack_top - (stack_size >> 3);
	__asm volatile ("MSR PSP, %0 \n" : : "r"(psp) :"memory");
	__ISB();
	HAL_MPU_Disable();
	mpu_simple_apply(0, default_operation->region);
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
#ifdef TIMER_OPEC
	_stop_timer();
	__operation_start_EXE_time[__operation_start_CALL_count++] = _get_cycles();
#endif
	//to be fulfilled
}


void __environment_init(void)
{
	// HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
	CHANGE_TO_PSP;
	// __asm volatile ("svc 0");
	DROP_PRIVILEGES;
	__asm volatile ("bx lr");
}


/* load operation data section from Flash to SRAM */
void __operation_init_data_section(uint32_t *flash_start, uint32_t *sram_start, uint32_t *sram_end)
{
#ifdef TIMER_OPEC
	_reset_timer();
	_start_timer();
#endif
	uint32_t *src_ptr = flash_start;
	uint32_t *dst_ptr = sram_start;

	while (dst_ptr < sram_end){
		*dst_ptr = *src_ptr;
		dst_ptr += 1;
		src_ptr += 1;
	}
#ifdef TIMER_OPEC
	_stop_timer();
	__operation_init_data_section_EXE_time[__operation_init_data_section_CALL_count++] = _get_cycles();
#endif
}


void __clear_memh_memp(uint32_t *sram_start, uint32_t *sram_end)
{
#ifdef TIMER_OPEC
	_reset_timer();
	_start_timer();
#endif
	uint32_t *ptr;
	for (ptr=sram_start; ptr<sram_end; ptr++) {
		*ptr = 0;
	}
#ifdef TIMER_OPEC
	_stop_timer();
	__clear_memh_memp_EXE_time[__clear_memh_memp_CALL_count++] = _get_cycles();
#endif
}
