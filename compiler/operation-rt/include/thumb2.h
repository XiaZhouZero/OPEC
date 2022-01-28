#ifndef __THUMB2_H
#define __THUMB2_H
#include "operation-rt.h"

// op2=00xx0xx
#define load_multiple_store_multiple_mask       0b1100100
#define load_multiple_store_multiple_template   0b0000000
// op2=00xx1xx
#define load_store_dual_or_exclusive_mask       0b1100100
#define load_store_dual_or_exclusive_template   0b0000100
// op2=000xxx0
#define store_single_data_item_mask     0b1110001
#define store_single_data_item_template 0b0000000
// op2=00xx001
#define load_byte_memory_hints_mask     0b1100111
#define load_byte_memory_hints_template 0b0000001
// op2=00xx011
#define load_halfword_meory_hints_mask      0b1100111
#define load_halfword_meory_hints_template  0b0000011
// op2=00xx101
#define load_word_mask      0b1100111
#define load_word_template  0b0000101

void thumb2_disasm(struct Instruction *thumb2_inst_struct, uint32_t *Regs);

#endif