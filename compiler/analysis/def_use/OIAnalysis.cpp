//===-- CrossDSOCFI.cpp - Externalize this module's CFI checks ------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This pass exports all llvm.bitset's found in the module in the form of a
// __cfi_check function, which can be used to verify cross-DSO call targets.
//
//===----------------------------------------------------------------------===//
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/Format.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/json/json.h"

#include <fstream>

using namespace llvm;

#define DEBUG_TYPE "OIAnalysis"
static cl::opt<bool> DEFUSE("defuse", cl::init(false), cl::Hidden, cl::desc("Output the defuse of global variable"));
static cl::opt<bool> OPECTRACE("opectrace", cl::init(false), cl::Hidden, cl::desc("build trace version bin"));
static cl::opt<std::string> ReadOnlyGlobalVairables("ro-gv",
                                  cl::desc("JSON File to write read-only global variables"),
                                  cl::init("build9/ro_gv.json"), cl::value_desc("filename"));

static cl::opt<std::string> ReadWriteGlobalVairables("rw-gv",
                                  cl::desc("JSON File to write read-write global variables"),
                                  cl::init("build9/rw_gv.json"), cl::value_desc("filename"));

static cl::opt<std::string> MemoryPoolVariableNames("memory-pool-names",
                                  cl::desc("JSON File to read global variables that will be treated as memory pools"),
                                  cl::init("build9/memory_pool_names.json"), cl::value_desc("filename"));

static cl::opt<std::string> MemoryPoolVariables("memory-pools",
                                  cl::desc("JSON File to write memory pool information"),
                                  cl::init("build9/memory_pool.json"), cl::value_desc("filename"));

static cl::opt<std::string> AllGlobalVairables("all-gv",
                                  cl::desc("JSON File to write all global variables"),
                                  cl::init("build9/all_gv.json"), cl::value_desc("filename"));

static cl::opt<std::string> IndirectCallsites("indirect-callsites",
                                  cl::desc("JSON File to write indirect callsites infomation"),
                                  cl::init("build9/indirect_callsites.json"), cl::value_desc("filename"));

static cl::opt<std::string> AddressTakenFuncions("address-taken-functions",
                                  cl::desc("JSON File to write address taken function results to"),
                                  cl::init("build9/address_taken_funcs.json"), cl::value_desc("filename"));

static cl::opt<std::string> TypeBasedAnalysisJsResults("indicall-analysis-results",
                                  cl::desc("JSON File to write indirect function call anaylsis results to"),
                                  cl::init("build9/type_based_analysis_results.json"), cl::value_desc("filename"));

static cl::opt<std::string> FunctionPeripherals("function-peripheral-dependency",
                                  cl::desc("JSON File to write address taken function results to"),
                                  cl::init("build9/function_peripheral_dependency.json"), cl::value_desc("filename"));

STATISTIC(OICounter, "Counts number of global variables greeted");

namespace {
  // Hello - The first implementation, without getAnalysisUsage.
  struct OIAnalysis : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    OIAnalysis() : FunctionPass(ID) {
      initializeOIAnalysisPass(*PassRegistry::getPassRegistry());
    }

	Json::Value ReadOnlyGVJsonRoot;
	Json::Value ReadWriteGVJsonRoot;
	Json::Value AllGVJsonRoot;
	Json::Value MemoryPoolJsonRoot;
	Json::Value IndirectCallsitesJsonRoot;
	Json::Value TypeAnalysisJsonRoot;
	Json::Value AddressTakenJsonRoot;
	Json::Value FunctionDependencyJsonRoot;


	/**
	 * @brief Given a Type of a global variable, determines the fields that are pointers
	 * 
	 * @param M Module
	 * @param GV_Struct_Type The StructType of current analysized type
	 * @param GVField Json::Value object to add pointer fields info
	 * @param base base offset
	 */
	void find_struct_type_pointer_fields(Module &M, StructType *GV_Struct_Type, Json::Value &GVField, uint32_t parent_base) {
		for (uint32_t i=0; i<GV_Struct_Type->getNumElements(); i++) {
			Type *EleType = GV_Struct_Type->getTypeAtIndex(i);
			uint32_t type_size = M.getDataLayout().getTypeAllocSize(EleType);
			uint32_t current_base = M.getDataLayout().getStructLayout(GV_Struct_Type)->getElementOffset(i);

			/* case 1: the element is a pointer */
			if (PointerType *PT = dyn_cast<PointerType>(EleType)) {
				if (!PT->getElementType()->isFunctionTy()) {
					GVField.append(parent_base + current_base);
					GVField.append(parent_base + current_base+type_size-1);
				}
			}

			/* case 2: the element is an array */
			else if (ArrayType *EleArrayType = dyn_cast<ArrayType>(EleType)) {
				find_array_type_pointer_fields(M, EleArrayType, GVField, parent_base+current_base);
			}

			/* case 3: the element is a struct */
			else if (StructType *EleStructType = dyn_cast<StructType>(EleType)) {
				find_struct_type_pointer_fields(M, EleStructType, GVField, parent_base+current_base);
			}
		}
	}


	void find_array_type_pointer_fields(Module &M, ArrayType *GV_Array_Type, Json::Value &GVField, uint32_t parent_base) {
		Type *EleType = GV_Array_Type->getArrayElementType();
		uint32_t type_size = M.getDataLayout().getTypeAllocSize(EleType);
		for (uint32_t i=0; i<GV_Array_Type->getNumElements(); i++) {
			uint32_t current_base = type_size * i;
			if (PointerType *PT = dyn_cast<PointerType>(EleType)) {
				if (!PT->getElementType()->isFunctionTy()) {
					GVField.append(parent_base + current_base);
					GVField.append(parent_base + current_base+type_size-1);
				}
			}
			else if (ArrayType *EleArrayType = dyn_cast<ArrayType>(EleType)) {
				find_array_type_pointer_fields(M, EleArrayType, GVField, parent_base+current_base);
			}
			else if (StructType *EleStructType = dyn_cast<StructType>(EleType)) {
				find_struct_type_pointer_fields(M, EleStructType, GVField, parent_base+current_base);
			}
		}
	}


	void determine_pointer_fields(Module &M, Type *GV_Type, Json::Value &GVField) {
		if (StructType *GV_Struct_Type = dyn_cast<StructType>(GV_Type)) {
			find_struct_type_pointer_fields(M, GV_Struct_Type, GVField, 0);
		}
		else if (ArrayType *GV_Array_Type = dyn_cast<ArrayType>(GV_Type)) {
			find_array_type_pointer_fields(M, GV_Array_Type, GVField, 0);
		}
		#if 0
		if (StructType *GV_Struct_Type = dyn_cast<StructType>(GV_Type)) {
			// GVNode["num_of_elements"] = GV_Struct_Type->getNumElements();
			for (uint32_t i=0; i<GV_Struct_Type->getNumElements(); i++) {
				Type *EleType = GV_Struct_Type->getTypeAtIndex(i);
				uint32_t type_size = M.getDataLayout().getTypeAllocSize(EleType);
				uint32_t base = M.getDataLayout().getStructLayout(GV_Struct_Type)->getElementOffset(i);
				if (EleType->isPointerTy()) {
					PointerType *PT = dyn_cast<PointerType>(EleType);
					if (!PT->getElementType()->isFunctionTy()) {
						GVField.append(base);
						GVField.append(base+type_size-1);
					}
				}
				else if (EleType->isArrayTy() && EleType->getArrayElementType()->isPointerTy()) {
					ArrayType *array_ty = dyn_cast<ArrayType>(EleType);
					uint64_t array_num = array_ty->getArrayNumElements();
					uint32_t element_size = M.getDataLayout().getTypeAllocSize(array_ty->getElementType());
					PointerType *ele_pt_ty = dyn_cast<PointerType>(EleType->getArrayElementType());
					if (!ele_pt_ty->getElementType()->isFunctionTy()) {
						for (uint64_t j=0; j<array_num; j++) {
							GVField.append(base+j*element_size);
							GVField.append(base+(j+1)*element_size-1);
						}
					}
				}
			}
		}
		else if (ArrayType *GV_Array_Type = dyn_cast<ArrayType>(GV_Type)) {
			// GVNode["num_of_elements"] = GV_Array_Type->getNumElements();
			for (uint32_t i=0; i<GV_Array_Type->getNumElements(); i++) {
				Type *EleType = GV_Array_Type->getTypeAtIndex(i);
				uint32_t type_size = M.getDataLayout().getTypeAllocSize(EleType);
				uint32_t base = type_size * i;
				if (EleType->isPointerTy()) {
					PointerType *PT = dyn_cast<PointerType>(EleType);
					if (!PT->getElementType()->isFunctionTy()) {
						GVField.append(base);
						GVField.append(base+type_size-1);
					}
				}
				// 只支持2层数组
				else if (EleType->isArrayTy() && EleType->getArrayElementType()->isPointerTy()) {
					ArrayType *array_ty = dyn_cast<ArrayType>(EleType);
					uint64_t array_num = array_ty->getArrayNumElements();
					uint32_t element_size = M.getDataLayout().getTypeAllocSize(array_ty->getElementType());
					PointerType *ele_pt_ty = dyn_cast<PointerType>(EleType->getArrayElementType());
					if (!ele_pt_ty->getElementType()->isFunctionTy()) {
						for (uint64_t j=0; j<array_num; j++) {
							GVField.append(base+j*element_size);
							GVField.append(base+(j+1)*element_size-1);
						}
					}
				}
			}
		}
		#endif
	}


	void def_use_analysis(GlobalVariable &GV, Value *GVptr, Module &M) {
		for (User *U : GVptr->users()) {
			// errs() << "U: " << *U << "\n";
			if (Instruction *Inst = dyn_cast<Instruction>(U)) {
				if(isIRQHandler(*Inst->getFunction())){
					continue;
				}
				++OICounter;
				errs() << "[" << OICounter << "] ";
				// errs() << "@";	// there is no "@" before GV Name in LLVM4
				errs() << GV.getName();
				errs() << ":";
				errs() << Inst->getFunction()->getName() << "\n";
				// GV.getType()->dump();
				// GV.getValueType()->dump();
				// errs() << "\n";
				// errs() << GV.getType() << ":" << GV.getValueType();
				// errs() << "\n";

				/* Save RO GV and RW GV respectively */
				if(GV.isConstant())
					ReadOnlyGVJsonRoot.append(GV.getName().str());
				else
					ReadWriteGVJsonRoot.append(GV.getName().str());
				Json::Value GVNode;
				GVNode["size"] = M.getDataLayout().getTypeAllocSize(GV.getValueType());
				// GVNode["num_of_elements"] = 0;
				GVNode["isConstant"] = GV.isConstant();
				GVNode["isStruct"] = GV.getValueType()->isStructTy();
				Json::Value GVField;
				determine_pointer_fields(M, GV.getValueType(), GVField);

				GVNode["PointerField"] = GVField;
				// The size of the type of a global variable won't change
				/* Find out memory pool related global variables, currently specified for application LwIP_TCP_Echo_Server */
				AllGVJsonRoot[GV.getName().str()] = GVNode;
				std::string memp_label = "memp_tab_";
				std::string memh_label = "ram_heap";
				if (GV.getName().startswith(memp_label)) {
					std::string memp_name = GV.getName().str().substr(memp_label.length());
					if (!MemoryPoolJsonRoot.isMember(memp_name)) {
						Json::Value MPNode;
						/* Memory Table */
						MPNode[GV.getName().str()] = M.getDataLayout().getTypeAllocSize(GV.getValueType());
						/* Memory Base */
						std::string GV_Base_Name_Str = "memp_memory_" + memp_name + "_base";
						StringRef *GV_Base_Name = new StringRef(GV_Base_Name_Str);
						if (GlobalVariable *GV_Base = M.getNamedGlobal(*GV_Base_Name)) {
							MPNode[GV_Base_Name_Str] = M.getDataLayout().getTypeAllocSize(GV_Base->getValueType());
						}

						/* Over All */
						MemoryPoolJsonRoot[memp_name] = MPNode;
					}
				}
				else if (GV.getName().equals(StringRef(memh_label))) {
					if (!MemoryPoolJsonRoot.isMember(GV.getName().str())) {
						Json::Value MHNode;
						/* ram_heap */
						MHNode[GV.getName().str()] = M.getDataLayout().getTypeAllocSize(GV.getValueType());
						/* ram */
						StringRef *stringref_ram = new StringRef("ram");
						if (GlobalVariable *GV_ram = M.getNamedGlobal(*stringref_ram))
							MHNode[GV_ram->getName().str()] = M.getDataLayout().getTypeAllocSize(GV_ram->getValueType());
						/* ram_end */
						StringRef *string_ramend = new StringRef("ram_end");
						if (GlobalVariable *GV_ram_end = M.getNamedGlobal(*string_ramend))
							MHNode[GV_ram_end->getName().str()] = M.getDataLayout().getTypeAllocSize(GV_ram_end->getValueType());
						/* lfree */
						StringRef *string_lfree = new StringRef("lfree");
						if (GlobalVariable *GV_lfree = M.getNamedGlobal(*string_lfree))
							MHNode[GV_lfree->getName().str()] = M.getDataLayout().getTypeAllocSize(GV_lfree->getValueType());
						MemoryPoolJsonRoot["MEMHEAP"] = MHNode;
					}
				}
				else if (GV.getName().equals(StringRef("DMARxDscrTab"))) {
					Json::Value DMATbNode;
					/* DMARxDscrTab */
					DMATbNode[GV.getName().str()] = M.getDataLayout().getTypeAllocSize(GV.getValueType());

					/* Rx_Buff */
					StringRef *stringref_rxbuf = new StringRef("Rx_Buff");
					if (GlobalVariable *GV_rxbuf = M.getNamedGlobal(*stringref_rxbuf)) {
						DMATbNode[GV_rxbuf->getName().str()] = M.getDataLayout().getTypeAllocSize(GV_rxbuf->getValueType());
					}

					/* DMATxDscrTab */
					StringRef *stringref_txtab = new StringRef("DMATxDscrTab");
					if (GlobalVariable *GV_txtab = M.getNamedGlobal(*stringref_txtab)) {
						DMATbNode[GV_txtab->getName().str()] = M.getDataLayout().getTypeAllocSize(GV_txtab->getValueType());
					}

					/* Tx_Buff */
					StringRef *stringref_txbuf = new StringRef("Tx_Buff");
					if (GlobalVariable *GV_txbuf = M.getNamedGlobal(*stringref_txbuf)) {
						DMATbNode[GV_txbuf->getName().str()] = M.getDataLayout().getTypeAllocSize(GV_txbuf->getValueType());
					}

					MemoryPoolJsonRoot["ETHDMA"] = DMATbNode;
				}

				else if (GV.getName().equals(StringRef("results"))) {
					Json::Value ResultsVarNode;
					ResultsVarNode[GV.getName().str()] = M.getDataLayout().getTypeAllocSize(GV.getValueType());
					MemoryPoolJsonRoot["CoreMarkResults"] = ResultsVarNode;
				}

				else if (GV.getName().equals(StringRef("stack_memblock"))) {
					Json::Value StackMemoryBlockNode;
					StackMemoryBlockNode[GV.getName().str()] = M.getDataLayout().getTypeAllocSize(GV.getValueType());
					MemoryPoolJsonRoot["StackMemoryBlock"] = StackMemoryBlockNode;
				}

				// errs() << "Instruction: " << *Inst << "\n\n";
			}
			else if (Constant *C = dyn_cast<Constant>(U)) {
				// errs() << "Check constant: ";
				// C->dump();
				def_use_analysis(GV, C, M);
			}
			else{
				errs() << " is unused\n";
			}
		}
	}

    unsigned getConstIntToPtr(Value *V, Type* &ty){
        unsigned addr = 0;
		// errs() << "getConstIntToPtr\n";
        if (IntToPtrInst * I2P = dyn_cast<IntToPtrInst>(V)){
            Value *Val = I2P->getOperand(0);
			// errs() << "Is IntToPtrInst\n";
			// I2P->dump();
            if (ConstantInt * CInt = dyn_cast<ConstantInt>(Val)) {
				// errs() << "Is ConstantInt\n";
				// CInt->dump();
                addr = CInt->getValue().getLimitedValue(0xFFFFFFFF);
                ty = I2P->getType();
                errs() << "[+] Int: " << addr << "\n";
                errs() << "[+] Type: ";
				ty->dump();
                return addr;
            } else {
                return 0;
            }
        } else if (Instruction *I = dyn_cast<Instruction>(V)) {
			// errs() << "Is Instruction\n";
			// I->dump();
            for (unsigned i=0; i<I->getNumOperands(); i++){
                addr = getConstIntToPtr(I->getOperand(i), ty);
                if (addr){
                    return addr;
                }
            }
        } else if (ConstantExpr *C = dyn_cast<ConstantExpr>(V)) {
			// errs() << "Is ConstantExpr\n";
			// C->dump();
            Instruction *Instr = C->getAsInstruction();
            addr = getConstIntToPtr(Instr, ty);
			Instr->deleteValue();
        } else {
            return 0;
        }
        return addr;
    }

	void getPeripheralDependencies(Json::Value &target_addr_json, SmallSet<Constant *,32> &Worklist, const DataLayout &DL) {
		for (auto *C: Worklist) {
			// errs() << "In worklsit\n";
			unsigned int addr;
			Type *ty = nullptr;
			// errs() << "Checking Constant: ";
			// C->dump();
			// errs() << "After dumping constant\n";
			addr = getConstIntToPtr(C, ty);
			if (addr != 0) {
				target_addr_json.append(addr);
				errs() << "[+] Addr: " << addr << "\n\n";
			}
			// errs() << "END\n\n";
		}
	}

	unsigned int getSinglePeripheralDependency(Constant* C) {
		unsigned int addr = 0;
		Type *ty = nullptr;
		addr = getConstIntToPtr(C, ty);
		return addr;
	}

	bool isIRQHandler(const Function& F) {
		if( F.getName().startswith("llvm.dbg.") || 
			F.getName().startswith("llvm.ctlz") ||
			F.getName().startswith("Reset_Handler") ||
			F.getName().startswith("NMI_Handler") ||
			F.getName().startswith("HardFault_Handler") ||
			F.getName().startswith("MemManage_Handler") ||
			F.getName().startswith("BusFault_Handler") ||
			F.getName().startswith("UsageFault_Handler") ||
			F.getName().startswith("SVC_Handler") ||
			F.getName().startswith("DebugMon_Handler") ||
			F.getName().startswith("PendSV_Handler") ||
			F.getName().startswith("SysTick_Handler") ||
			F.getName().startswith("WWDG_IRQHandler") ||
			F.getName().startswith("PVD_IRQHandler") ||
			F.getName().startswith("TAMP_STAMP_IRQHandler") ||
			F.getName().startswith("RTC_WKUP_IRQHandler") ||
			F.getName().startswith("FLASH_IRQHandler") ||
			F.getName().startswith("RCC_IRQHandler") ||
			F.getName().startswith("EXTI0_IRQHandler") ||
			F.getName().startswith("EXTI1_IRQHandler") ||
			F.getName().startswith("EXTI2_IRQHandler") ||
			F.getName().startswith("EXTI3_IRQHandler") ||
			F.getName().startswith("EXTI4_IRQHandler") ||
			F.getName().startswith("DMA1_Stream0_IRQHandler") ||
			F.getName().startswith("DMA1_Stream1_IRQHandler") ||
			F.getName().startswith("DMA1_Stream2_IRQHandler") ||
			F.getName().startswith("DMA1_Stream3_IRQHandler") ||
			F.getName().startswith("DMA1_Stream4_IRQHandler") ||
			F.getName().startswith("DMA1_Stream5_IRQHandler") ||
			F.getName().startswith("DMA1_Stream6_IRQHandler") ||
			F.getName().startswith("ADC_IRQHandler") ||
			F.getName().startswith("CAN1_TX_IRQHandler") ||
			F.getName().startswith("CAN1_RX0_IRQHandler") ||
			F.getName().startswith("CAN1_RX1_IRQHandler") ||
			F.getName().startswith("CAN1_SCE_IRQHandler") ||
			F.getName().startswith("EXTI9_5_IRQHandler") ||
			F.getName().startswith("TIM1_BRK_TIM9_IRQHandler") ||
			F.getName().startswith("TIM1_UP_TIM10_IRQHandler") ||
			F.getName().startswith("TIM1_TRG_COM_TIM11_IRQHandler") ||
			F.getName().startswith("TIM1_CC_IRQHandler") ||
			F.getName().startswith("TIM2_IRQHandler") ||
			F.getName().startswith("TIM3_IRQHandler") ||
			F.getName().startswith("TIM4_IRQHandler") ||
			F.getName().startswith("I2C1_EV_IRQHandler") ||
			F.getName().startswith("I2C1_ER_IRQHandler") ||
			F.getName().startswith("I2C2_EV_IRQHandler") ||
			F.getName().startswith("I2C2_ER_IRQHandler") ||
			F.getName().startswith("SPI1_IRQHandler") ||
			F.getName().startswith("SPI2_IRQHandler") ||
			F.getName().startswith("USART1_IRQHandler") ||
			F.getName().startswith("USART2_IRQHandler") ||
			F.getName().startswith("USART3_IRQHandler") ||
			F.getName().startswith("EXTI15_10_IRQHandler") ||
			F.getName().startswith("RTC_Alarm_IRQHandler") ||
			F.getName().startswith("OTG_FS_WKUP_IRQHandler") ||
			F.getName().startswith("TIM8_BRK_TIM12_IRQHandler") ||
			F.getName().startswith("TIM8_UP_TIM13_IRQHandler") ||
			F.getName().startswith("TIM8_TRG_COM_TIM14_IRQHandler") ||
			F.getName().startswith("TIM8_CC_IRQHandler") ||
			F.getName().startswith("DMA1_Stream7_IRQHandler") ||
			F.getName().startswith("FSMC_IRQHandler") ||	//from Pinlock
			F.getName().startswith("FMC_IRQHandler") ||
			F.getName().startswith("SDIO_IRQHandler") ||
			F.getName().startswith("TIM5_IRQHandler") ||
			F.getName().startswith("SPI3_IRQHandler") ||
			F.getName().startswith("UART4_IRQHandler") ||
			F.getName().startswith("UART5_IRQHandler") ||
			F.getName().startswith("TIM6_DAC_IRQHandler") ||
			F.getName().startswith("TIM7_IRQHandler") ||
			F.getName().startswith("DMA2_Stream0_IRQHandler") ||
			F.getName().startswith("DMA2_Stream1_IRQHandler") ||
			F.getName().startswith("DMA2_Stream2_IRQHandler") ||
			F.getName().startswith("DMA2_Stream3_IRQHandler") ||
			F.getName().startswith("DMA2_Stream4_IRQHandler") ||
			F.getName().startswith("ETH_IRQHandler") ||
			F.getName().startswith("ETH_WKUP_IRQHandler") ||
			F.getName().startswith("CAN2_TX_IRQHandler") ||
			F.getName().startswith("CAN2_RX0_IRQHandler") ||
			F.getName().startswith("CAN2_RX1_IRQHandler") ||
			F.getName().startswith("CAN2_SCE_IRQHandler") ||
			F.getName().startswith("OTG_FS_IRQHandler") ||
			F.getName().startswith("DMA2_Stream5_IRQHandler") ||
			F.getName().startswith("DMA2_Stream6_IRQHandler") ||
			F.getName().startswith("DMA2_Stream7_IRQHandler") ||
			F.getName().startswith("USART6_IRQHandler") ||
			F.getName().startswith("I2C3_EV_IRQHandler") ||
			F.getName().startswith("I2C3_ER_IRQHandler") ||
			F.getName().startswith("OTG_HS_EP1_OUT_IRQHandler") ||
			F.getName().startswith("OTG_HS_EP1_IN_IRQHandler") ||
			F.getName().startswith("OTG_HS_WKUP_IRQHandler") ||
			F.getName().startswith("OTG_HS_IRQHandler") ||
			F.getName().startswith("DCMI_IRQHandler") ||
			F.getName().startswith("HASH_RNG_IRQHandler") ||
			F.getName().startswith("FPU_IRQHandler") ||
			F.getName().startswith("UART7_IRQHandler") ||
			F.getName().startswith("UART8_IRQHandler") ||
			F.getName().startswith("SPI4_IRQHandler") ||
			F.getName().startswith("SPI5_IRQHandler") ||
			F.getName().startswith("SPI6_IRQHandler") ||
			F.getName().startswith("SAI1_IRQHandler") ||
			F.getName().startswith("LTDC_IRQHandler") ||
			F.getName().startswith("LTDC_ER_IRQHandler") ||
			F.getName().startswith("DMA2D_IRQHandler") ||
			F.getName().startswith("QUADSPI_IRQHandler") ||
			F.getName().startswith("DSI_IRQHandler")) {
				return true;
			}
		else {
			return false;
		}
	}

	std::string getSourceLocOfFunction(const Function& F) {
		// refer to SVFUtil::getSourceLocOfFunction
		std::string str;
		const Function *func = &F;
		raw_string_ostream rawstr(str);
		if (DISubprogram *SP = func->getSubprogram())
			if (SP->describes(func)) {
				rawstr << "ln: " << SP->getLine() << "  fl: " << SP->getFilename();
			}
		return rawstr.str();
	}

	std::string getSourceOfInstruction(const Instruction* inst) {
		std::string str;
		raw_string_ostream rawstr(str);
		if(const DebugLoc &location = inst->getDebugLoc()) {
			const DILocation *DL =location.get();
			rawstr << "ln: " << DL->getLine() << "  cl: " << DL->getColumn() << "  fl: " << DL->getFilename();
		}
		return rawstr.str();
	}

	void outputAddressTakenFunction(Module *mptr) {
		errs() << "########################################AddressTaken Functions########################################\n";
		for(Module::iterator FunIt = mptr->begin(); FunIt != mptr->end(); ++FunIt) {
			if(Function *F_ptr = dyn_cast<Function>(FunIt)) {
				if(F_ptr->hasAddressTaken()) {
					std::string fptr_name = F_ptr->getName().str();
					AddressTakenJsonRoot.append(fptr_name);
					errs() << fptr_name << "\n";
				}
			}
		}
		errs() << "########################################AddressTaken Functions########################################\n";
	}

	#if 0
	std::string getSourceOfGlobalVariable(const GlobalValue* gv) {
		std::string str;
		raw_string_ostream rawstr(str);
		if(const DebugLoc &location = gv->getDebugInfo()) {
			const DILocation *DL = location.get();
			rawstr << "line: " << DL->getLine() << ", file: " << DL->getFilename();
		}
		return rawstr.str();
	}
	#endif


    bool TypesEqual(Type *T1, Type *T2, unsigned depth=0) {
        if ( T1 == T2 ) {
            return true;
        }
        if (depth > 10) {
            // If we haven't found  a difference this deep just assume they are
            // the same type. We need to overapproximate (i.e. say more things
            // are equal than really are) so return true
            return true;
        }
        if (PointerType *Pty1 = dyn_cast<PointerType>(T1) ) {
            if (PointerType *Pty2 = dyn_cast<PointerType>(T2)) {
            	return TypesEqual(Pty1->getPointerElementType(), Pty2->getPointerElementType(), depth+1);
            }
			else {
                return false;
            }
        }
        if (FunctionType * FTy1 = dyn_cast<FunctionType>(T1)) {
            if (FunctionType * FTy2 = dyn_cast<FunctionType>(T2)) {
                if (FTy1->getNumParams() != FTy2->getNumParams()) {
                    return false;
                }
                if (! TypesEqual(FTy1->getReturnType(), FTy2->getReturnType(), depth+1)) {
                    return false;
                }
                for (unsigned i=0; i<FTy1->getNumParams(); i++) {
                    if (FTy1->getParamType(i) == FTy1 && FTy2->getParamType(i) == FTy2) {
                        continue;
                    }
					else if (FTy1->getParamType(i) != FTy1 && FTy2->getParamType(i) != FTy2 ) {
                        FTy1->getParamType(i)->dump();
                        FTy2->getParamType(i)->dump();
                        if(!TypesEqual(FTy1->getParamType(i), FTy2->getParamType(i), depth+1)) {
                        	return false;
                        }
                    }
					else {
                        return false;
                    }
                }
                return true;

            }
			else {
                return false;
            }
        }
        if (StructType *STy1 = dyn_cast<StructType>(T1)) {
            if (StructType *STy2 = dyn_cast<StructType>(T2)) {
                if (STy2->getNumElements() != STy1->getNumElements()) {
                    return false;
                }
                if (STy1->hasName() && STy2->hasName()) {
                    if(STy1->getName().startswith(STy2->getName()) || STy2->getName().startswith(STy1->getName())) {
                        return true;
                    }
                }
                return false;
            }
			else {
                return false;
            }
        }
        return false;
    }


	bool doInitialization(Module &M) override {
		Module *mptr = &M;

		if(DEFUSE) {
			#if 0
			errs() << "########################################ALL LOAD & STORE & GETELEMENTPTR########################################\n";
			for(Module::iterator FunIt = mptr->begin(); FunIt != mptr->end(); ++FunIt) {
				errs() << "$FunctionName: " <<FunIt->getName() << "\n";
				for(Function::iterator BBIt=FunIt->begin(); BBIt!=FunIt->end(); ++BBIt) {
					for(BasicBlock::iterator InstIt=BBIt->begin(); InstIt!=BBIt->end(); ++InstIt) {
						if(Instruction *Inst = dyn_cast<Instruction>(InstIt)) {
							// 输出所有的LoadInst&StoreInst
							if(isa<LoadInst>(Inst) || isa<StoreInst>(Inst) || isa<CallInst>(Inst) || isa<GetElementPtrInst>(Inst)) {
								errs() << *Inst << "\n";
							}
						}
					}
				}
			}
			errs() << "########################################ALL LOAD & STORE & GETELEMENTPTR########################################\n";
			#endif
			// 输出所有的全局变量名称，用于指针分析时对变量进行verify
			errs() << "########################################ALL GLOBALVARIABLES########################################\n";
			// OICounter = 0;
			// for(auto GV=M.global_begin(); GV!=M.global_end(); GV++) {
			for(GlobalVariable &GV: M.globals()) {
				// if  (GV.hasInitializer() && !GV.getName().startswith(".")) {
				if (GV.hasInitializer()) {
					//errs() << "[" << ++OICounter << "]" << GV.getName() << "\n";
					errs() << GV.getName();
					if(GV.isConstant()) {
						errs() << ":isConstant";
					}
					std::string gv_type_str;
					llvm::raw_string_ostream rso_gv_type(gv_type_str);
					GV.getType()->print(rso_gv_type);
					errs() << ":" << rso_gv_type.str();
					APInt size = APInt(32, M.getDataLayout().getTypeAllocSize(GV.getValueType()));
					errs() << ":" << size << "\n";
					// errs() << getSourceOfGlobalVariable(&GV) << "\n";
				}
			}
			errs() << "########################################ALL GLOBALVARIABLES########################################\n";
			errs() << "########################################LLVM DEF-USE ANALYSIS########################################\n";
			OICounter = 0;
			// for(auto GV=M.global_begin(); GV!=M.global_end(); GV++) {
			for(GlobalVariable &GV: M.globals()) {
				GlobalVariable *GVptr = &GV;
				// if  (GV.hasInitializer() && !GV.getName().startswith(".")) {
				if  (GV.hasInitializer()) {
					def_use_analysis(GV, &GV, M);
				}
			}
			errs() << "########################################LLVM DEF-USE ANALYSIS########################################\n";
			// Peripheral analysis. Note that we do not analyze the peripheral dependency in IRQHandler function. For callee functions of IRQHandler, we still analyze them because type-based analysis will add call edge from function call to thems
			// IRQHandler functions are acquired from the .s file of each project
			errs() << "########################################LLVM PERIPHERAL ANALYSIS########################################" << "\n";
			for (Function& F: M.functions()) {
				errs() << "\n[*] Function: " << F.getName() << "\n";
				if(isIRQHandler(F)) {
					continue;
				}

				Json::Value current_func;
				Json::Value target_addr;

				SmallSet<Constant *,32> PeripheralWorklist;
				bool has_callee = false;
				for (inst_iterator itr=inst_begin(F); itr != inst_end(F); ++itr) {
					Instruction *I = &*itr;
					for (unsigned i=0; i<I->getNumOperands(); i++){
						Value *V = I->getOperand(i);
						if (ConstantExpr *C = dyn_cast<ConstantExpr>(V)) {
							if (CallInst *CI = dyn_cast<CallInst>(I)) {
								if(isa<ICmpInst>(I))
									continue;

								// Find out callee function whose argument is a peripheral address
								unsigned int addr = getSinglePeripheralDependency(C);
								std::string func_name = "";
								if (Function *callee_func = CI->getCalledFunction()) {
									// normal function calls
									func_name = callee_func->getName().str();
								}

								else if (Function *voidFunc = dyn_cast<Function>(CI->getCalledValue()->stripPointerCasts())) {
									// function calls that contain bitcast command
									func_name = voidFunc->getName().str();
								}

								else {
									// unknown conditions, just dump the callinst
									func_name = "UNKNOWN";
									CI->dump();
								}

								// Ignore function whose argment is not a peripheral address
								if (addr != 0) {
									errs() << "[*] Call function: ";
									if(func_name != "")
										errs() << func_name << "\n";
									errs() << "[+] Addr: " << addr << "\n\n";
									if(!current_func.isMember(func_name)) {
										Json::Value callee_json;
										current_func[func_name] = callee_json;
									}
									current_func[func_name].append(addr);
								}
								// output the end marker of callee function parameter peripheral address checking
								has_callee = true;
								// Since the CallInst does not store/load peripheral address, we neglect the ConstantExpr in it and just continue to the next IR instruction
								continue;
							}
							// collect all ConstantExpr to do further peripheral analysis in function: getPeripheralDependencies
							PeripheralWorklist.insert(C);
						}
						else if (ConstantInt *ConstInt = dyn_cast<ConstantInt>(V)) {
							I->dump();
							uint32_t addr = (uint32_t)ConstInt->getZExtValue();
							errs() << "addr: " << addr << "\n";
							if (addr >= 0x40000000) {
								target_addr.append(addr);
							}
						}
					}
				}
				if (has_callee) {
					has_callee = false;
					errs() << "[!] Callee end\n\n";
				}
				
				getPeripheralDependencies(target_addr, PeripheralWorklist, mptr->getDataLayout());
				current_func["addr"] = target_addr;
				FunctionDependencyJsonRoot[F.getName().str()] = current_func;
			}
			errs() << "########################################LLVM PERIPHERAL ANALYSIS########################################" << "\n";
			// return false;
		}
		outputAddressTakenFunction(mptr);
		errs() << "########################################INDIRECT CALL ANALYSIS########################################" << "\n";
		if(1) {
			for(Module::iterator FunIt = mptr->begin(); FunIt != mptr->end(); ++FunIt) {
				Function *F_ptr = dyn_cast<Function>(FunIt);
				// if(F_ptr && isIRQHandler(*F_ptr))
					// continue;

				errs() << "[*] Function: " <<FunIt->getName() << ":";
				FunIt->getFunctionType()->dump();
				errs() << getSourceLocOfFunction(*F_ptr) << "\n";
				for(Function::iterator BBIt=FunIt->begin(); BBIt!=FunIt->end(); ++BBIt) {
					for(BasicBlock::iterator InstIt=BBIt->begin(); InstIt!=BBIt->end(); ++InstIt) {
						if(Instruction *Inst = dyn_cast<Instruction>(InstIt)) {
							if(CallInst *callinst = dyn_cast<CallInst>(Inst)) {
								/* Direct function call */
								if(Function *fp = callinst->getCalledFunction())
									continue;

								/* Direct function call, target function is type casted */
								else if (Function *voidFunc = dyn_cast<Function>(callinst->getCalledValue()->stripPointerCasts()))
									continue;

								else {
									/* Exclude call asm instructions */
									if (callinst->isInlineAsm())
										continue;

									errs() << "[+] Indirect callsite: \n";
									callinst->dump();
									errs() << getSourceOfInstruction(Inst) << "\n";

									// save raw indirect call information to json files
									std::string indirect_callsite_str;
									llvm::raw_string_ostream rso_callsite(indirect_callsite_str);
									callinst->print(rso_callsite);
									Json::Value IndirectCallsite;
									IndirectCallsite["function"] = callinst->getFunction()->getName().str();
									IndirectCallsite["callsite"] = rso_callsite.str();
									IndirectCallsite["location"] = getSourceOfInstruction(Inst);
									IndirectCallsitesJsonRoot.append(IndirectCallsite);

									// errs() << "\n";
									FunctionType *indirectype = callinst->getFunctionType();
									Json::Value FuncNode, CallsiteNode, TargetsNode;
									for (Function& tgt_func : F_ptr->getParent()->getFunctionList()) {
										if (TypesEqual(indirectype, tgt_func.getFunctionType()) && tgt_func.hasAddressTaken()) {
											errs() << "---\n" << tgt_func.getName().str() << ":";
											tgt_func.getFunctionType()->dump();
											errs() << getSourceLocOfFunction(tgt_func) << "\n";
											std::string target_func_name = tgt_func.getName().str();
											// Json::Value TargetNode;
											// TargetNode["TargetNode"] = target_func_name;
											std::string tgt_func_type_str;
											llvm::raw_string_ostream rso_functype(tgt_func_type_str);
											Type *func_type = tgt_func.getType();
											func_type->print(rso_functype);
											// TargetNode["func_type"] = rso_functype.str();
											// TargetNode["func_location"] = getSourceLocOfFunction(tgt_func);
											// TargetsNode.append(TargetNode);
											TargetsNode.append(target_func_name);
										}

									}
									errs() << "\n";
									CallsiteNode["targets"] = TargetsNode;
									std::string callinst_str;
									llvm::raw_string_ostream rso(callinst_str);
									callinst->print(rso);
									CallsiteNode["callinst"] = callinst_str;
									std::string callsite_location = getSourceOfInstruction(Inst);
									CallsiteNode["location"] = callsite_location;
									Type* t = callinst->getCalledValue()->getType();
									FunctionType* ft = cast<FunctionType>(cast<PointerType>(t)->getElementType());
									std::string callsite_functype;
									llvm::raw_string_ostream rso_callsite_type(callsite_functype);
									ft->print(rso_callsite_type);
									CallsiteNode["type"] = rso_callsite_type.str();

									// Json::Value FuncNode = TypeAnalysisJsonRoot.get(F_ptr->getName().str(), "");

									// TypeAnalysisJsonRoot[F_ptr->getName().str()] = FuncNode;
									TypeAnalysisJsonRoot[F_ptr->getName().str()].append(CallsiteNode);
								}
							}
						}
					}
				}
			}
			return false;
		}
		errs() << "########################################INDIRECT CALL ANALYSIS########################################" << "\n";
	}

	bool doFinalization(Module &M) override{
		std::ofstream jsonFile;

		jsonFile.open(ReadOnlyGlobalVairables);
		jsonFile << ReadOnlyGVJsonRoot;
		jsonFile.close();

		jsonFile.open(ReadWriteGlobalVairables);
		jsonFile << ReadWriteGVJsonRoot;
		jsonFile.close();

		jsonFile.open(MemoryPoolVariables);
		jsonFile << MemoryPoolJsonRoot;
		jsonFile.close();

		jsonFile.open(AllGlobalVairables);
		jsonFile << AllGVJsonRoot;
		jsonFile.close();

		jsonFile.open(IndirectCallsites);
		jsonFile << IndirectCallsitesJsonRoot;
		jsonFile.close();

		jsonFile.open(TypeBasedAnalysisJsResults);
		jsonFile << TypeAnalysisJsonRoot;
		jsonFile.close();

		jsonFile.open(AddressTakenFuncions);
		jsonFile << AddressTakenJsonRoot;
		jsonFile.close();

		jsonFile.open(FunctionPeripherals);
		jsonFile << FunctionDependencyJsonRoot;
		jsonFile.close();

		return false;
	}

    bool runOnFunction(Function & F) override {
		if(OPECTRACE) {
			errs() << "########################################OPEC TRACE########################################" << "\n";
#ifndef something
			if(F.getName().startswith("ITM_SendChar") ||
			   F.getName().startswith("HAL_UART_Transmit_IT") ||
			   F.getName().startswith("_write") ||
			   F.getName().startswith("_write_r") ||
			   F.getName().startswith("__sflush_r") ||
			   F.getName().startswith("_fflush_r") ||
			   F.getName().startswith("__sfvwrite_r") ||
			   F.getName().startswith("__sprint_r.part.0") ||
			   F.getName().startswith("_vfprintf_r") ||
			   F.getName().startswith("printf") ||
			   F.getName().startswith("HAL_IncTick") ||
			   F.getName().startswith("SysTick_Handler")) {
				   errs() << "[!]" << F.getName() << "\n";
				return false;
			}
#else
			if(F.getName().startswith("printf") ||
			   F.getName().startswith("llvm")) {
				errs() << "[!]" << F.getName() << "\n";
				return false;
			}
#endif
			errs() << "[+]" << F.getName() << "\n";

			Function *printfFunc = F.getParent()->getFunction("printf");
			assert(printfFunc && "No printf function");
			LLVMContext &context = F.getParent()->getContext();
			BasicBlock *bb = &F.getEntryBlock();
			IRBuilder<> builder(&*bb->getFirstInsertionPt());
#if 0
			std::string output = "[Trace]";
			output += F.getName().str();
			output += "\n";
#endif
			std::string output = "#";
			output += F.getName().str();
			Value *v = builder.CreateGlobalStringPtr(output, output+"_name");
			ArrayRef<Value *> args(v);
			// builder.SetInsertPoint(bb->getFirstInsertionPt());

#if 1
			builder.CreateCall(printfFunc, args);
#endif
#if 0
			Instruction *newInst = CallInst::Create(printfFunc, args);
			bb->getInstList().push_front(newInst);
			// Instruction *beginInst = dyn_cast<Instruction>(bb->begin());
			// beginInst->insertBefore(newInst);
#endif
			errs() << F.getName() << "\n";

			// std::vector<Value *> CallParams;
			// errs() << "########################################OPEC TRACE########################################" << "\n";
			return true;
		}
        return false;
    }
  };
}


char OIAnalysis::ID = 0;

namespace llvm {
  FunctionPass* createOIAnalysisPass() {
    return new OIAnalysis();
  }
}


INITIALIZE_PASS_BEGIN(OIAnalysis, "OPEC Analysis Pass", 
                      "", 
                      false, false)
// INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass) // Or whatever your Pass dependencies
INITIALIZE_PASS_END(OIAnalysis, "OPEC Analysis Pass",
                    "", 
                    false, false)