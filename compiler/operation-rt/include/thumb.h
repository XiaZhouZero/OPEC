#ifndef __THUMB_H
#define __THUMB_H
#include "operation-rt.h"

void thumb_disasm(struct Instruction *thumb_inst_struct, uint32_t *Regs);

#endif