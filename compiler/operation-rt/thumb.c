#include "operation-rt.h"
#include "thumb.h"


/*
 * Disassemble thumb instructions, current only support store load thumb instructions
 */
void thumb_disasm(struct Instruction *thumb_inst_struct, uint32_t *Regs) {
	uint16_t thumb_inst, opcode;
	uint8_t Rm, Rn, Rt;
	thumb_inst = (uint16_t)(thumb_inst_struct->inst & 0xFFFF);
	opcode = thumb_inst & 0xFE00;
	switch (opcode) {			// A5.2.4, Load/store single data item, A-133
		// A5.2.4 Store Register
		case 0x5000:			// STR<c> <Rt>,[<Rn>,<Rm>], A7-388
			Rt = thumb_inst & 0x7;
			Rn = thumb_inst >> 3 & 0x7;
			Rm = thumb_inst >> 6 & 0x7;
			construct_thumb2_inst(thumb_inst_struct, IS_STORE, Regs[Rn] + Regs[Rm], Rt, ONE_WORD, IS_UNSIGNED, PC_2_STEP);
			break;

		// A5.2.4 Store Register Halfword
		case 0x5200:			// STRH<c> <Rt>,[<Rn>,<Rm>], A7-399
			Rt = thumb_inst & 0x7;
			Rn = thumb_inst >> 3 & 0x7;
			Rm = thumb_inst >> 6 & 0x7;
			construct_thumb2_inst(thumb_inst_struct, IS_STORE, Regs[Rn] + Regs[Rm], Rt, ONE_HALFWORD, IS_UNSIGNED, PC_2_STEP);
			break;

		// A5.2.4 Store Register Byte
		case 0x5400:			// STRB<c> <Rt>,[<Rn>,<Rm>], A7-391
			Rt = thumb_inst & 0x7;
			Rn = thumb_inst >> 3 & 0x7;
			Rm = thumb_inst >> 6 & 0x7;
			construct_thumb2_inst(thumb_inst_struct, IS_STORE, Regs[Rn] + Regs[Rm], Rt, ONE_BYTE, IS_UNSIGNED, PC_2_STEP);
			break;
		
		// A5.2.4 Load Register Signed Byte
		case 0x5600:			// LDRSB<c> <Rt>,[<Rn>,<Rm>], A7-273
			Rt = thumb_inst & 0x7;
			Rn = thumb_inst >> 3 & 0x7;
			Rm = thumb_inst >> 6 & 0x7;
			construct_thumb2_inst(thumb_inst_struct, IS_LOAD, Regs[Rn] + Regs[Rm], Rt, ONE_BYTE, IS_SIGNED, PC_2_STEP);
			break;

		// A5.2.4 Load Register
		case 0x5800:			// LDR<c> <Rt>,[<Rn>,<Rm>], A7-250
			Rt = thumb_inst & 0x7;
			Rn = thumb_inst >> 3 & 0x7;
			Rm = thumb_inst >> 6 & 0x7;
			construct_thumb2_inst(thumb_inst_struct, IS_LOAD, Regs[Rn] + Regs[Rm], Rt, ONE_WORD, IS_UNSIGNED, PC_2_STEP);
			break;

		// A5.2.4 Load Register Halfword
		case 0x5A00:			// LDRH<c> <Rt>,[<Rn>,<Rm>], A7-267
			Rt = thumb_inst & 0x7;
			Rn = thumb_inst >> 3 & 0x7;
			Rm = thumb_inst >> 6 & 0x7;
			construct_thumb2_inst(thumb_inst_struct, IS_LOAD, Regs[Rn] + Regs[Rm], Rt, ONE_HALFWORD, IS_UNSIGNED, PC_2_STEP);
			break;

		// A.5.2.4 Load Register Byte
		case 0x5C00:			// LDRB<c> <Rt>,[<Rn>,<Rm>], A7-255
			Rt = thumb_inst & 0x7;
			Rn = thumb_inst >> 3 & 0x7;
			Rm = thumb_inst >> 6 & 0x7;
			construct_thumb2_inst(thumb_inst_struct, IS_LOAD, Regs[Rn] + Regs[Rm], Rt, ONE_BYTE, IS_UNSIGNED, PC_2_STEP);
			break;

		// A.5.2.4 Load Register Signed Halfword
		case 0x5E00:			// LDRSH<c> <Rt>,[<Rn>,<Rm>], A7-278
			Rt = thumb_inst & 0x7;
			Rn = thumb_inst >> 3 & 0x7;
			Rm = thumb_inst >> 6 & 0x7;
			construct_thumb2_inst(thumb_inst_struct, IS_LOAD, Regs[Rn] + Regs[Rm], Rt, ONE_HALFWORD, IS_SIGNED, PC_2_STEP);
			break;
	}

	opcode = thumb_inst & 0xF800;
	switch (opcode)	{
		uint32_t imm5, imm8;
		// A5.2.4 Store Register
		case 0x6000:			// STR<c> <Rt>, [<Rn>{,#<imm5>}], A7-386
			Rt = thumb_inst & 0x7;
			Rn = thumb_inst >> 3 & 0x7;
			imm5 = thumb_inst >> 6 & 0x1F;
			construct_thumb2_inst(thumb_inst_struct, IS_STORE, Regs[Rn] + imm5, Rt, ONE_WORD, IS_UNSIGNED, PC_2_STEP);
			break;

		// A5.2.4 Load Register
		case 0x6800:			// LDR<c> <Rt>, [<Rn>{,#<imm5>}], A7-246
			Rt = thumb_inst & 0x7;
			Rn = thumb_inst >> 3 & 0x7;
			imm5 = thumb_inst >> 6 & 0x1F;
			construct_thumb2_inst(thumb_inst_struct, IS_LOAD, Regs[Rn] + imm5, Rt, ONE_WORD, IS_UNSIGNED, PC_2_STEP);
			break;

		// A5.2.4 Store Register Byte
		case 0x7000:			// STRB<c> <Rt>,[<Rn>,#<imm5>], A7-389
			Rt = thumb_inst & 0x7;
			Rn = thumb_inst >> 3 & 0x7;
			imm5 = thumb_inst >> 6 & 0x1F;
			construct_thumb2_inst(thumb_inst_struct, IS_STORE, Regs[Rn] + imm5, Rt, ONE_BYTE, IS_UNSIGNED, PC_2_STEP);
			break;

		// A5.2.4 Load Register Byte
		case 0x7800:			// LDRB<c> <Rt>,[<Rn>{,#<imm5>}], A7-252
			Rt = thumb_inst & 0x7;
			Rn = thumb_inst >> 3 & 0x7;
			imm5 = thumb_inst >> 6 & 0x1F;
			construct_thumb2_inst(thumb_inst_struct, IS_LOAD, Regs[Rn] + imm5, Rt, ONE_BYTE, IS_UNSIGNED, PC_2_STEP);
			break;

		// A5.2.4 Store Register Halfword
		case 0x8000:			// STRH<c> <Rt>,[<Rn>{,#<imm5>}], A7-397
			Rt = thumb_inst & 0x7;
			Rn = thumb_inst >> 3 & 0x7;
			imm5 = thumb_inst >> 6 & 0x1F;
			construct_thumb2_inst(thumb_inst_struct, IS_STORE, Regs[Rn] + imm5, Rt, ONE_HALFWORD, IS_UNSIGNED, PC_2_STEP);
			break;

		// A5.2.4 Load Register Halfword
		case 0x8800:			// LDRH<c> <Rt>,[<Rn>{,#<imm5>}], A7-264
			Rt = thumb_inst & 0x7;
			Rn = thumb_inst >> 3 & 0x7;
			imm5 = thumb_inst >> 6 & 0x1F;
			construct_thumb2_inst(thumb_inst_struct, IS_LOAD, Regs[Rn] + imm5, Rt, ONE_HALFWORD, IS_UNSIGNED, PC_2_STEP);
			break;

		// A5.2.4 Store Register SP relative
		case 0x9000:			// Store Register SP relative, A7-386
			Rt = thumb_inst >> 8 & 0x7;
			imm8 = thumb_inst & 255;
			construct_thumb2_inst(thumb_inst_struct, IS_STORE, Regs[REG_SP] + imm8, Rt, ONE_WORD, IS_UNSIGNED, PC_2_STEP);
			break;

		// A5.2.4 Load Register SP relative
		case 0x9800:			// LDR<c> <Rt>,[SP{,#<imm8>}], A7-246
			Rt = thumb_inst >> 8 & 0x7;
			imm8 = thumb_inst & 255;
			construct_thumb2_inst(thumb_inst_struct, IS_LOAD, Regs[REG_SP] + imm8, Rt, ONE_WORD, IS_UNSIGNED, PC_2_STEP);
			break;
	}
}