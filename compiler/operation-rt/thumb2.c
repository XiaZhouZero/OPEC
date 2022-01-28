#include "operation-rt.h"
#include "thumb2.h"


/*
 * Disassemble thumb2 instructions, current only support store load thumb instructions
 */
void thumb2_disasm(struct Instruction *thumb2_inst_struct, uint32_t *Regs) {
	/* thumb-2 32bit inst, A5.3 */
	uint16_t first_hw = thumb2_inst_struct->inst & 0xFFFF;
	uint16_t second_hw = thumb2_inst_struct->inst >> 16 & 0xFFFF;
	uint32_t big_endian_inst = first_hw << 16 | second_hw;
	uint32_t Rm, Rn, Rt, index, add, wback;
	uint32_t imm12, imm8, imm2;
	uint32_t P, U, W;

	/* support thumb-2 32bit store/load inst, A5-137 */
	uint8_t op1 = big_endian_inst >> (16+11) & 0x3;
	uint8_t op2 = big_endian_inst >> (16+4) & 0x7F;
	uint8_t op  = big_endian_inst >> 15 & 0x1;

	if ((op1==0x1) && ((op2&load_multiple_store_multiple_mask)==load_multiple_store_multiple_template)) {	// A5.3.5 Load Multiple and Store Multiple
		OPEC_FAULT
		#if 0
		op = big_endian_inst >> (16+7) & 0x2;
		uint8_t W = big_endian_inst >> (16+5) & 0x1;
		uint8_t L = big_endian_inst >> (16+4) & 0x1;
		uint8_t W_Rn = big_endian_inst >> 16 & 0x3F;
		if ((op==1) && (L==0)) {	// Store Multiple (Increment After, Empty Ascending) STM, STMIA, STMEA on page A7-383
									// A7.7.159 STM, STMIA, STMEA Encoding T2
			OPEC_FAULT
		}
		else if ((op==1) && (L==1) && (W_Rn!=0b11101)) {	// Load Multiple (Increment After, Full Descending) LDM, LDMIA, LDMFD on page A7-242
			OPEC_FAULT
		}
		else if ((op==2) && (L==0) && (W_Rn!=0b11101)) {	// Store Multiple (Decrement Before, Full Descending) STMDB, STMFD on page A7-385
			OPEC_FAULT
		}
		else if ((op==2) && (L==1)) {						// Load Multiple (Decrement Before, Empty Ascending) LDMDB, LDMEA on page A7-244
			OPEC_FAULT
		}
		else
			OPEC_FAULT
		#endif
	}

	else if ((op1==0x1) && ((op2&load_store_dual_or_exclusive_mask)==load_store_dual_or_exclusive_template)) {	// A5.3.6 Load/store dual or exclusive, table branch
		op1 = first_hw >> 7 & 0x3;
		op2 = first_hw >> 4 & 0x3;
		Rn = first_hw & 0xF;
		uint8_t op3 = second_hw >> 4 & 0xF;
		if ((op1==0) && (op2==0)) {				// Store Register Exclusive STREX on page A7-394
			OPEC_FAULT
		}
		else if ((op1==0) && (op2==1)) {		// Load Register Exclusive LDREX on page A7-261
			OPEC_FAULT
		}
		else if (((op1&0x2)==0) && (op2==0x2)) {	// Store Register Dual STRD (immediate) on page A7-393
			OPEC_FAULT
		}
		OPEC_FAULT
	}

	else if ((op1==0x3) && ((op2&store_single_data_item_mask)==store_single_data_item_template)) {	// A5.3.10 Store single data item A5-149
		op1 = big_endian_inst >> (16+5) & 0x7;
		op2 = big_endian_inst >> 6 & 0x3F;
		switch (op1) {
			case 0b100:		// A7-389 STRB (immediate) Encoding T2
				Rt = big_endian_inst >> 12 & 0xF;
				Rn = big_endian_inst >> 16 & 0xF;
				imm12 = big_endian_inst & 0xFFF;
				construct_thumb2_inst(thumb2_inst_struct, IS_STORE, Regs[Rn]+imm12, Rt, ONE_BYTE, IS_UNSIGNED, PC_4_STEP);
				break;

			case 0b000:
				if ((op2 & 0b100000)) {	// op2=0b1xxxxx, Store Register Byte, A7-389 STRB (immediate) Encoding T3
					OPEC_FAULT
					#if 0
					Rt = big_endian_inst >> 12 & 0xF;
					Rn = big_endian_inst >> 16 & 0xF;
					imm8 = big_endian_inst & 0xFF;
					uint8_t P = big_endian_inst >> 10 & 0x1;
					uint8_t U = big_endian_inst >> 9 & 0x1;
					uint8_t W = big_endian_inst >> 8 & 0x1;
					if (P==1 && U==1 && W==1) {
						OPEC_FAULT	// STRBT
					}
					if (Rn==15 || (P==0 && W==0)) {
						OPEC_FAULT 	// UNDEFINED
					}
					if (P==1)
						index=BOOL_TRUE;
					if (U==1)
						add=BOOL_TRUE;
					if (W==1)
						wback=BOOL_TRUE;
					#endif
				}
				else {					// A7.7.164 Store Register Byte STRB (register) on page A7-391
					Rn = first_hw & 0xF;
					Rt = second_hw >> 12 & 0xF;
					Rm = second_hw & 0xF;
					imm2 = second_hw >> 4 & 0x3;
					uint32_t offset = Regs[Rm] << imm2;
					construct_thumb2_inst(thumb2_inst_struct, IS_STORE, Regs[Rn]+offset, Rt, ONE_BYTE, IS_UNSIGNED, PC_4_STEP);
				}
				break;
			
			case 0b101:
				break;
			

			case 0b001:
				break;

			case 0b110:					// A7.7.161 STR (immediate) Encoding T3 on page A7-386
				Rt = big_endian_inst >> 12 & 0xF;
				Rn = big_endian_inst >> 16 & 0xF;
				imm12 = big_endian_inst & 0xFFF;
				construct_thumb2_inst(thumb2_inst_struct, IS_STORE, Regs[Rn]+imm12, Rt, ONE_WORD, IS_UNSIGNED, PC_4_STEP);
				break;

			case 0b010:
				if (!(op2 & 0b100000)) {	// A7.7.162 STR (register) Encoding T2
					Rt = big_endian_inst >> 12 & 0xF;
					Rn = big_endian_inst >> 16 & 0xF;
					Rm = big_endian_inst & 0xF;
					imm2 = big_endian_inst >> 4 & 0x3;
					construct_thumb2_inst(thumb2_inst_struct, IS_STORE, Regs[Rn]+(Regs[Rm]<<imm2), Rt, ONE_WORD, IS_UNSIGNED, PC_4_STEP);
					break;
				}
				else {
					break;
				}

			default:
				break;
		}
	}
	else if ((op1==0x3) && ((op2&load_byte_memory_hints_mask)==load_byte_memory_hints_template)) {
		OPEC_FAULT
	}
	else if ((op1==0x3) && ((op2&load_halfword_meory_hints_mask)==load_halfword_meory_hints_template)) {
		OPEC_FAULT
	}
	else if ((op1==0x3) && ((op2&load_word_mask)==load_word_template)) {	// Load word on page A5-146
		op1 = big_endian_inst >> (16+7) & 0x3;
		op2 = big_endian_inst >> 6 & 0x3F;
		Rn = big_endian_inst >> 16 & 0xF;
		if ((op1==0b01) && (Rn!=0xF)) {										// Load Register LDR (immediate) on page A7-246, Encoding T3
			imm12 = big_endian_inst & 0xFFF;
			Rt = big_endian_inst >> 12 & 0xF;
			construct_thumb2_inst(thumb2_inst_struct, IS_LOAD, Regs[Rn]+imm12, Rt, ONE_WORD, IS_UNSIGNED, PC_4_STEP);
		}
		else if ((op1==0) && ((op2&0b100100)==0b100100) && (Rn!=0xF)) {		// Load Register LDR (immediate) on page A7-246, Encoding T4
			Rt = big_endian_inst >> 12 & 0xF;
			imm8 = big_endian_inst & 0xFF;
			uint32_t PUW = big_endian_inst >> 8 & 0x7;
				switch (PUW) {
				case 0b000:		// (P==0) && (U==0) && (W==0)
				case 0b010:		// (P==0) && (U==1) && (W==0)
					construct_thumb2_inst(thumb2_inst_struct, IS_LOAD, Regs[Rn], Rt, ONE_WORD, IS_UNSIGNED, PC_4_STEP);
					break;
				case 0b001:		// (P==0) && (U==0) && (W==1)
					construct_thumb2_inst(thumb2_inst_struct, IS_LOAD, Regs[Rn], Rt, ONE_WORD, IS_UNSIGNED, PC_4_STEP);
					Regs[Rn] = Regs[Rn] - imm8;
					break;
				case 0b011:		// (P==0) && (U==1) && (W==1)
					construct_thumb2_inst(thumb2_inst_struct, IS_LOAD, Regs[Rn], Rt, ONE_WORD, IS_UNSIGNED, PC_4_STEP);
					Regs[Rn] = Regs[Rn] + imm8;
					break;
				case 0b100:		// (P==1) && (U==0) && (W==0)
					construct_thumb2_inst(thumb2_inst_struct, IS_LOAD, Regs[Rn]-imm8, Rt, ONE_WORD, IS_UNSIGNED, PC_4_STEP);
					break;
				case 0b101:		// (P==1) && (U==0) && (W==1)
					construct_thumb2_inst(thumb2_inst_struct, IS_LOAD, Regs[Rn]-imm8, Rt, ONE_WORD, IS_UNSIGNED, PC_4_STEP);
					Regs[Rn] = Regs[Rn] - imm8;
					break;
				case 0b110:		// (P==1) && (U==1) && (W==0)
					construct_thumb2_inst(thumb2_inst_struct, IS_LOAD, Regs[Rn]+imm8, Rt, ONE_WORD, IS_UNSIGNED, PC_4_STEP);
					break;
				case 0b111:		// (P==1) && (U==1) && (W==1)
					construct_thumb2_inst(thumb2_inst_struct, IS_LOAD, Regs[Rn]+imm8, Rt, ONE_WORD, IS_UNSIGNED, PC_4_STEP);
					Regs[Rn] = Regs[Rn] + imm8;
					break;
			}
		}
		else if ((op1==0) && ((op2&0b111100)==0b110000) && (Rn!=0xF)) {		// Load Register LDR (immediate) on page A7-246, Encoding T4
			OPEC_FAULT
		}
		else if ((op1==0) && ((op2&0b111000)==0b111000) && (Rn!=0xF)) {		// Load Register Unprivileged LDRT on page A7-281
			Rt = big_endian_inst >> 12 & 0xF;
			imm8 = big_endian_inst & 0xFF;
			construct_thumb2_inst(thumb2_inst_struct, IS_LOAD, Regs[Rn]+imm8, Rt, ONE_WORD, IS_UNSIGNED, PC_4_STEP);
		}
		else if ((op1==0) && (op2==0) && (Rn!=0xF)) {						// Load Register LDR (register) on page A7-250
			Rt = big_endian_inst >> 12 & 0xF;
			imm2 = big_endian_inst >> 4 & 0x3;
			Rm = big_endian_inst & 0xF;
			construct_thumb2_inst(thumb2_inst_struct, IS_LOAD, Regs[Rn]+(Regs[Rm]<<imm2), Rt, ONE_WORD, IS_UNSIGNED, PC_4_STEP);
		}
		else if (((op1&0b10)==0) && Rn==0xF) {								// Load Register LDR (literal) on page A7-248, Encoding T2
			Rt = big_endian_inst >> 12 & 0xF;
			imm12 = big_endian_inst & 0xFFF;
			if((big_endian_inst >> (16+7) & 0x1) == 1)	// if bit U==1
				construct_thumb2_inst(thumb2_inst_struct, IS_LOAD, Regs[Rn]+imm12, Rt, ONE_WORD, IS_UNSIGNED, PC_4_STEP);
			else
				construct_thumb2_inst(thumb2_inst_struct, IS_LOAD, Regs[Rn]-imm12, Rt, ONE_WORD, IS_UNSIGNED, PC_4_STEP);
		}
	}
	else {
		OPEC_FAULT
	}
}