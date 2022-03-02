#include <fstream>
#include <iostream>
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Format.h"
#include "llvm/Transforms/Instrumentation.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/json/json.h" //From https://github.com/open-source-parsers/jsoncpp

using namespace llvm;

#define DEBUG_TYPE "OIApplication"
#define OPERATION_MPU_RW 0		//	deprecated
#define INSERT_LoadInst_CONSIDER_VOLATILE

static cl::opt<std::string> OIPolicy("OIpolicy", cl::desc("JSON defining the policy"), cl::init("-"), cl::value_desc("filename"));
static cl::opt<std::string> MemoryPool("MemoryPool", cl::desc("Memory pool information"), cl::init("build9/memory_pool_aligned.json"), cl::value_desc("filename"));

namespace {
	struct OIApplication : public ModulePass {
		static char ID;

		class OperationMetadata {
		public:
			Function *func;
			CallInst *inst;
			SmallSet<GlobalVariable *, 16> InternalGV;
			DenseMap<GlobalVariable *, GlobalVariable *> ExternalGV;

			OperationMetadata():func(NULL),inst(NULL){};
		};

		class OperationInfo {
			StringMap<OperationMetadata *> NameToMD;
			DenseMap<Function *, OperationMetadata *> FuncToMD;
		public:
			DenseMap<GlobalVariable *, GlobalVariable *> GVptr;

			OperationMetadata *GetMD(Function *func) {
				DenseMap<Function *, OperationMetadata *>::iterator it;

				it = FuncToMD.find(func);
				if(it != FuncToMD.end())
					return it->second;
				return NULL;
			};

			OperationMetadata *GetMD(StringRef name) {
				StringMap<OperationMetadata *>::iterator it;

				it = NameToMD.find(name);
				if(it != NameToMD.end())
					return it->second;
				return NULL;
			};

			void insert_metadata(Function *Func, std::string Name, OperationMetadata *MD) {
				FuncToMD.insert(std::make_pair(Func, MD));
				NameToMD.insert(std::make_pair(Name, MD));
				return;
			};
		} OInfo;

		OIApplication():ModulePass(ID) {}

		bool doInitialization(Module &M) override {
			return true;
		}


		bool isIntrinsics(const Function& F) {
			if(F.getName().startswith("llvm.dbg.") || F.getName().startswith("llvm.ctlz")) {
				return true;
			}
			else {
				return false;
			}
		}


		bool isIRQHandlerorIntrinsics(const Function& F) {
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

		Constant *get_MPU_region_data(Module & M, StructType * RegionTy, unsigned int Addr, unsigned int Attr){
			SmallVector<Constant *,8> RegionsVec;
			//DEBUG(errs()<< Addr << "\n");
			//DEBUG(errs()<< Attr << "\n");
			APInt addr = APInt(32,Addr);
			APInt attr = APInt(32,Attr);
			RegionsVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), addr));
			RegionsVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), attr));
			Constant *Region = ConstantStruct::get(RegionTy, RegionsVec);		// 这个函数是什么作用？
			return Region;
		}


		Constant *get_ptr_info(Module & M, StructType * PtrInfoTy, int Type, unsigned int Count, unsigned int Size) {
			SmallVector<Constant *, 5> PtrInfoVec;

			DEBUG(errs() << "[ShadowStack] Ptr info: Type: "<< Type << ", Count: " << Count << ", Size: " << Size << "\n");
			APInt type = APInt(32, Type);
			APInt count = APInt(32, Count);
			APInt size = APInt(32, Size);
			PtrInfoVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), type));
			PtrInfoVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), count));
			PtrInfoVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), size));
			PtrInfoVec.push_back(Constant::getIntegerValue(Type::getInt8PtrTy(M.getContext()), APInt(32, 0)));
			PtrInfoVec.push_back(Constant::getIntegerValue(Type::getInt8PtrTy(M.getContext()), APInt(32, 0)));
			Constant *PtrInfo = ConstantStruct::get(PtrInfoTy, PtrInfoVec);
			return PtrInfo;
		}


		Constant *set_shadow_table_entities(Module & M, StructType *PointerFieldTy, StructType *ShadowTblTy, Json::Value gv_name, Json::Value external_variable_fields_info, Json::Value external_variable_san_info, OperationMetadata *MD, unsigned int Attr){
			SmallVector<Constant *,16> ShadowTblsVec;
			GlobalVariable *GV;
			DenseMap<GlobalVariable *, GlobalVariable *>::iterator it;

			assert(MD && "Metadata is NULL!");
			DEBUG(errs() << "[ShadowData] Search " << gv_name.asString() << "\n");
			GV = M.getGlobalVariable(StringRef(gv_name.asString()), true);
			if(!GV)
				assert(false && "set_shadow_table_entities search global data fail!");

			it = MD->ExternalGV.find(GV);
			if(it == MD->ExternalGV.end())
				assert(false && "set_shadow_table_entities search external data fail!");
			APInt attr = APInt(32, Attr);	// set attr to RW
			Constant *sha_addr = ConstantExpr::getCast(Instruction::BitCast, it->second, Type::getInt8PtrTy(M.getContext()));
			Constant *ori_addr = ConstantExpr::getCast(Instruction::BitCast, it->first, Type::getInt8PtrTy(M.getContext()));
			APInt size = APInt(32, M.getDataLayout().getTypeAllocSize(it->first->getValueType()));

			it = OInfo.GVptr.find(GV);
			if(it == OInfo.GVptr.end())
				assert(false && "set_shadow_table_entities search external data ptr fail!");
			Constant *ptr_addr = ConstantExpr::getCast(Instruction::BitCast, it->second, Type::getInt8PtrTy(M.getContext()));

			ShadowTblsVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), attr));
			ShadowTblsVec.push_back(ptr_addr);
			ShadowTblsVec.push_back(sha_addr);
			ShadowTblsVec.push_back(ori_addr);
			ShadowTblsVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), size));

			Json::Value san_info = external_variable_san_info[gv_name.asString()];
			ShadowTblsVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), APInt(32, san_info[0].asUInt())));
			ShadowTblsVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), APInt(32, san_info[1].asUInt())));
			ShadowTblsVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), APInt(32, san_info[2].asUInt())));
			ShadowTblsVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), APInt(32, san_info[3].asUInt())));

			Json::Value fields_info = external_variable_fields_info[gv_name.asString()];
			uint32_t has_pointer_fileds = 0;
			Constant *pointer_field_ptr;
			SmallVector<Type *, 1> PointerFieldStructTyVec;
			if (fields_info.size()) {
				DEBUG(errs() << "fields size: " << fields_info.size() << "\n");
				has_pointer_fileds = 1;
				ArrayType *PointerFieldArrayTy = ArrayType::get(PointerFieldTy, fields_info.size()/2);
				PointerFieldStructTyVec.push_back(PointerFieldArrayTy);
				StructType *PointerFieldStructTy = StructType::create(PointerFieldStructTyVec, "__operation_poiter_field");
				PointerFieldStructTyVec.clear();

				SmallVector<Constant *, 2> PointerFieldElementVec;
				for (uint32_t i=0; i<fields_info.size(); i=i+2) {
					DEBUG(errs() << "[ShadowData] Adding PoitnerField: " << fields_info[i].asUInt() << ":" << fields_info[i+1].asUInt() << "\n");
					SmallVector<Constant *, 2> PointerFieldVariableVec;
					APInt first_index = APInt(32, fields_info[i].asUInt());
					APInt second_index = APInt(32, fields_info[i+1].asUInt());
					PointerFieldVariableVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), first_index));
					PointerFieldVariableVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), second_index));
					Constant *PointerFieldVariable = ConstantStruct::get(PointerFieldTy, PointerFieldVariableVec);
					PointerFieldElementVec.push_back(PointerFieldVariable);
				}

				Constant *PointerFieldArray = ConstantArray::get(PointerFieldArrayTy, PointerFieldElementVec);
				SmallVector<Constant *, 1> PointerFieldStructVec;
				PointerFieldStructVec.push_back(PointerFieldArray);
				Constant *PointerFieldStruct = ConstantStruct::get(PointerFieldStructTy, PointerFieldStructVec);
				GlobalVariable *pointer_field_variable = new GlobalVariable(M, PointerFieldStructTy, true, GlobalVariable::ExternalLinkage, PointerFieldStruct, "Poinerfield");
				pointer_field_ptr = ConstantExpr::getCast(Instruction::BitCast, pointer_field_variable, Type::getInt8PtrTy(M.getContext()));
			}
			else {
				DEBUG(errs() << "doesn't have fields\n");
				pointer_field_ptr = Constant::getIntegerValue(Type::getInt8PtrTy(M.getContext()), APInt(32, 0));
			}
			APInt has_fields = APInt(32, has_pointer_fileds);
			ShadowTblsVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), has_fields));
			APInt fields_num = APInt(32, fields_info.size()/2);
			ShadowTblsVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), fields_num));
			ShadowTblsVec.push_back(pointer_field_ptr);
			DEBUG(errs() << ShadowTblsVec.size() << "\n");

			Constant *ShadowDataTable = ConstantStruct::get(ShadowTblTy, ShadowTblsVec);
			return ShadowDataTable;
		}


		GlobalVariable *build_policy_section(Module &M, Json::Value &Root) {
			//GlobalVariable *shadow_GV, *GV;
			GlobalVariable *ret;
			unsigned int i;
			Json::Value operations = Root.get("Operation", "");
			Json::Value operation_data_section_info = Root.get("OperationDataSectionInfo", "");
			Json::Value num_mpu_regions = Root.get("NUM_MPU_REGIONS", 8);

			/* (0) 构造数据结构 */
			/* 构造 MPU_region 结构体
				struct MPU_region {
					uint32_t addr;
					uint32_t attr;
				};
			*/
			SmallVector<Type *,8> MPUConfigVec;
			MPUConfigVec.push_back(Type::getInt32Ty(M.getContext()));
			MPUConfigVec.push_back(Type::getInt32Ty(M.getContext()));
			// MPU_region 结构体 addr+size
			StructType *RegionTy = StructType::create(MPUConfigVec, "__operation_mpu_regions");
        	ArrayType *MPURegionTy = ArrayType::get(RegionTy, num_mpu_regions.asUInt());        //num_mpu_regions占一个Type

			/* 构造 struct shadow_data_table *tbl
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
					uint32_t has_pointer_fields;
					uint32_t need_sanitize;
					uint32_t offset_of_the_var_to_check;
					uint32_t min_value;
					uint32_t max_value;
					uint32_t pointer_fields_num;
					struct Pointer_Field *pointer_fields_ptr;
				};
			*/

			SmallVector<Type *, 2> PointerFieldVec;
			PointerFieldVec.push_back(Type::getInt32Ty(M.getContext()));
			PointerFieldVec.push_back(Type::getInt32Ty(M.getContext()));
			StructType *PointerFieldTy = StructType::create(PointerFieldVec, "__operation_pointer_field");


			SmallVector<Type *, 8> ShadowTblVec;
			ShadowTblVec.push_back(Type::getInt32Ty(M.getContext()));
			ShadowTblVec.push_back(Type::getInt8PtrTy(M.getContext()));		// llvm 没有 void*，用int8 *代替
			ShadowTblVec.push_back(Type::getInt8PtrTy(M.getContext()));
			ShadowTblVec.push_back(Type::getInt8PtrTy(M.getContext()));
			ShadowTblVec.push_back(Type::getInt32Ty(M.getContext()));		// size
			ShadowTblVec.push_back(Type::getInt32Ty(M.getContext()));		// has_pointer_fields
			ShadowTblVec.push_back(Type::getInt32Ty(M.getContext()));		// need_sanitize
			ShadowTblVec.push_back(Type::getInt32Ty(M.getContext()));		// offset_of_the_var_to_check
			ShadowTblVec.push_back(Type::getInt32Ty(M.getContext()));		// min_value
			ShadowTblVec.push_back(Type::getInt32Ty(M.getContext()));		// max_value
			ShadowTblVec.push_back(Type::getInt32Ty(M.getContext()));
			ShadowTblVec.push_back(Type::getInt8PtrTy(M.getContext()));

			// shaodow_data_table, addr+size
			StructType *ShadowTblTy = StructType::create(ShadowTblVec, "__operation_shadow_tables");
			// ArrayType * ShadowSectionTy = ArrayType::get(ShadowTblTy, num_shadow_variables);

			/* 构造 Perpheral_Regionlist 结构体
				struct Peripheral_Regionlist_Array {
					struct Peripheral_MPU_Region {
						uint32_t start_addr;
						uint32_t size;
						uint32_t attr;
						uint32_t mpu_rbar;
						uint32_t mpu_rasr;
					}peripehral[0];
				}
			 */
			SmallVector<Type *, 5> PeripheralRegionTyVec;
			PeripheralRegionTyVec.push_back(Type::getInt32Ty(M.getContext()));
			PeripheralRegionTyVec.push_back(Type::getInt32Ty(M.getContext()));
			PeripheralRegionTyVec.push_back(Type::getInt32Ty(M.getContext()));
			PeripheralRegionTyVec.push_back(Type::getInt32Ty(M.getContext()));
			PeripheralRegionTyVec.push_back(Type::getInt32Ty(M.getContext()));
			StructType *PeripheralRegionTy = StructType::create(PeripheralRegionTyVec, "__operation_peripheral_region");
			SmallVector<Type *, 1> PeripheralRegionlistStructTyVec;

			/* 构造 PPB_Register_Whitelist 结构体
				struct Whitelist_Struct {
					struct PPB_Register_Whitelist {
						uint32_t start_addr;
						uint32_t size;
						uint32_t attr;
					}ppb_reg[0];
				}
			 */
			SmallVector<Type *, 3> PPBRegTyVec;
			PPBRegTyVec.push_back(Type::getInt32Ty(M.getContext()));
			PPBRegTyVec.push_back(Type::getInt32Ty(M.getContext()));
			PPBRegTyVec.push_back(Type::getInt32Ty(M.getContext()));
			StructType *PPBRegTy = StructType::create(PPBRegTyVec, "__operation_ppb_reg");
			
			SmallVector<Type *, 1> PPBWhitelistStructTyVec;


			/* 构造struct shadow_stack_table
			    struct shadow_stack_table {
					uint32_t total_size;
					uint32_t ptr_num;
					struct {
						int type;		// type为正表示为offset，为负表示寄存器-1到-4表示寄存器r0-r3
						int count;		// count表示个数，负数表示跟其他参数关联，绝对值为索引
				   		int size;		// size表示每个的大小，负数表示和其他参数关联，绝对值为索引
						char *org_addr;
						char *new_addr;
					}arg[0];
			    }
			 */

			SmallVector<Type *, 3> ShadowStackTyVec;
			ShadowStackTyVec.push_back(Type::getInt32Ty(M.getContext()));
			ShadowStackTyVec.push_back(Type::getInt32Ty(M.getContext()));

			SmallVector<Type *, 5> ArgInfoVec;
			ArgInfoVec.push_back(Type::getInt32Ty(M.getContext()));
			ArgInfoVec.push_back(Type::getInt32Ty(M.getContext()));
			ArgInfoVec.push_back(Type::getInt32Ty(M.getContext()));
			ArgInfoVec.push_back(Type::getInt8PtrTy(M.getContext()));
			ArgInfoVec.push_back(Type::getInt8PtrTy(M.getContext()));
			StructType *ArgInfoTy = StructType::create(ArgInfoVec, "PtrInfo");

			/* 需要构建的mpu region
				struct Operation_Policy {
					struct MPU_Region region[8];	// 8 MPU regions for Cortex-M4 MCU
					uint32_t current_operation_id;
					struct Operations_Data_Section_Info *OpeDataSecInfo_ptr;
					struct Operation_Policies *PoliciesInfo_ptr;
					uint32_t stack_copy_size;
					struct Shadow_Stack_TBL *stbl;
					uint32_t peripehral_region_num;
					struct Peripheral_MPU_Region *regions_ptr;
					uint32_t ppb_whitelist_num;
					struct PPB_Register_Whitelist *ppb_ptr;
					uint32_t shadowdata_tbl_size;
					struct Shadow_Data_TBL shadowdata_tbl[0];
				};
			*/
			SmallVector<Type *, 11> OpeTyVec;
			OpeTyVec.push_back(MPURegionTy);								// struct MPU_Region [8]
			OpeTyVec.push_back(Type::getInt32Ty(M.getContext()));			// uint32_t operation_id;
			OpeTyVec.push_back(Type::getInt8PtrTy(M.getContext()));			// struct Operations_Data_Section_Info *OpeDataSecInfo_ptr;
			OpeTyVec.push_back(Type::getInt32Ty(M.getContext()));			// uint32_t stack_copy_size
			OpeTyVec.push_back(Type::getInt8PtrTy(M.getContext()));			// struct Shadow_Stack_TBL *
			OpeTyVec.push_back(Type::getInt32Ty(M.getContext()));			// uint32_t peripehral_region_num
			OpeTyVec.push_back(Type::getInt8PtrTy(M.getContext()));			// struct Peripheral_Regionlist *
			OpeTyVec.push_back(Type::getInt32Ty(M.getContext()));			// uint32_t ppb_whitelist_num
			OpeTyVec.push_back(Type::getInt8PtrTy(M.getContext()));			// struct PPB_Register_Whitelist *
			OpeTyVec.push_back(Type::getInt32Ty(M.getContext()));			// uint32_t tb_size

			/*
				struct Operation_Data_Section {
					uint32_t start_addr;
					uint32_t size;
					uint32_t operation_id;
				};
			*/
			SmallVector<Type *, 3> OpeDataSecTyVec;
			OpeDataSecTyVec.push_back(Type::getInt32Ty(M.getContext()));
			OpeDataSecTyVec.push_back(Type::getInt32Ty(M.getContext()));
			OpeDataSecTyVec.push_back(Type::getInt32Ty(M.getContext()));
			StructType *OpeDataSecTy = StructType::create(OpeDataSecTyVec, "__operation_data_sec");
			SmallVector<Type *, 1> OpeDataSecStructTyVec;

			/*
				struct Operations_Data_Section_Info {
					uint32_t operation_data_section_num;
					struct Operation_Data_Section *operation_data_section_ptr;
				};
			*/
			SmallVector<Type *, 2> OpeDataSecInfoTyVec;
			OpeDataSecInfoTyVec.push_back(Type::getInt32Ty(M.getContext()));
			OpeDataSecInfoTyVec.push_back(Type::getInt8PtrTy(M.getContext()));
			StructType *OpeDataSecInfoTy = StructType::create(OpeDataSecInfoTyVec, "__operation_data_sec_info");

			/*
				struct Operation_Policy_Info {
					struct Operation_Policy *policy_addr;
					uint32_t operation_id;
				};
			*/
			SmallVector<Type *, 2> PolicyInfoTyVec;
			PolicyInfoTyVec.push_back(Type::getInt8PtrTy(M.getContext()));
			PolicyInfoTyVec.push_back(Type::getInt32Ty(M.getContext()));
			StructType *PolicyInfoTy = StructType::create(PolicyInfoTyVec, "__operation_policy_info");
			SmallVector<Type *, 1> PolicyInfoStructTyVec;
			/*
				struct Operation_Policies {
					uint32_t operation_policy_num;
					struct Operation_Policy_Info *operation_policy_info_ptr;
				};
			*/
			SmallVector<Type *, 2> PoliciesInfoTyVec;
			PoliciesInfoTyVec.push_back(Type::getInt32Ty(M.getContext()));
			PoliciesInfoTyVec.push_back(Type::getInt8PtrTy(M.getContext()));
			StructType *PoliciesInfoTy = StructType::create(PoliciesInfoTyVec, "__operation_policies_info");


			/* (1) Build operation data section summary information */
			ArrayType *OpeDataSecArrayTy = ArrayType::get(OpeDataSecTy, operation_data_section_info.size());
			OpeDataSecStructTyVec.push_back(OpeDataSecArrayTy);
			StructType *OpeDataSecStructTy = StructType::create(OpeDataSecStructTyVec, "__operation_whitelist");
			OpeDataSecStructTyVec.clear();

			SmallVector<Constant *, 16> OpeDataSecArrayElementVec;
			Constant *OpeDataSec;
			for (uint8_t i=0; i<operation_data_section_info.size(); i++) {
				SmallVector<Constant *, 12> OpeDataSecVariableVec;
				Json::Value per_operation_data_info = operation_data_section_info[i];
				APInt addr = APInt(32, per_operation_data_info[0].asUInt());
				APInt size = APInt(32, per_operation_data_info[1].asUInt());
				APInt id   = APInt(32, per_operation_data_info[2].asUInt());
				OpeDataSecVariableVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), addr));
				OpeDataSecVariableVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), size));
				OpeDataSecVariableVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), id));
				OpeDataSec = ConstantStruct::get(OpeDataSecTy, OpeDataSecVariableVec);
				OpeDataSecArrayElementVec.push_back(OpeDataSec);
			}
			Constant *OpeDataSecArray = ConstantArray::get(OpeDataSecArrayTy, OpeDataSecArrayElementVec);
			SmallVector<Constant *, 1> OpeDataSecStructVec;
			OpeDataSecStructVec.push_back(OpeDataSecArray);

			DEBUG(errs() << "[OperationDataSectionInfo] Construct a OperationDataSectionInfo struct\n");
			Constant *OpeDataSecStruct = ConstantStruct::get(OpeDataSecStructTy, OpeDataSecStructVec);
			DEBUG(errs() << "[OperationDataSectionInfo] Construct a pointer\n");
			GlobalVariable *ope_sec_info_variable = new GlobalVariable(M, OpeDataSecStructTy, true, GlobalVariable::ExternalLinkage, OpeDataSecStruct, "operation_data_section_info");
			Constant *ope_sec_info_ptr = ConstantExpr::getCast(Instruction::BitCast, ope_sec_info_variable, Type::getInt8PtrTy(M.getContext()));

			SmallVector<Constant *, 2> OpeDataSecInfoValue;
			OpeDataSecInfoValue.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), APInt(32, operation_data_section_info.size())));
			OpeDataSecInfoValue.push_back(ope_sec_info_ptr);
			Constant *OpeDataSecInfoInit = ConstantStruct::get(OpeDataSecInfoTy, OpeDataSecInfoValue);
			GlobalVariable *OpeDataSecInfo = new GlobalVariable(M, OpeDataSecInfoTy, true, GlobalVariable::ExternalLinkage, OpeDataSecInfoInit, "OpeDataSecInfo");
			Constant *ope_data_sec_info_ptr = ConstantExpr::getCast(Instruction::BitCast, OpeDataSecInfo, Type::getInt8PtrTy(M.getContext()));
			OpeDataSecInfo->setSection(".rodata");

			/* Build Operation PolicyInfo StructTy */
			ArrayType *PolicyInfoArrayTy = ArrayType::get(PolicyInfoTy, operations.size());
			PolicyInfoStructTyVec.push_back(PolicyInfoArrayTy);
			StructType *PolicyInfoStructTy = StructType::create(PolicyInfoStructTyVec, "__operation_whitelist");
			PolicyInfoStructTyVec.clear();
			SmallVector<Constant *, 16> PolicyInfoArrayElementVec;
			Constant *PolicyInfo;

			/* Traverse operation information */
			for(JSONCPP_STRING operation_name : operations.getMemberNames()) {
				Json::Value function_name = operations[operation_name]["Entry"];
				Function *Fn = M.getFunction(function_name.asString());
				SmallVector<Constant *, 16> OpeValue; 
				Constant *stack_table_ptr;
				int status = 0;
				uint32_t size, copy_size = 0, total_size = 0;
				DEBUG(errs() << "[+] Start " << operation_name << "\n");

				/* (2) 根据Function的参数信息, 填入stack_copy_size和stbl */
				Json::Value stack_info = operations[operation_name]["Stack"];
				Json::Value::iterator arg_info = stack_info.begin();
				/* add ArgInfoArrayTy to ShadowStackTy */
				ArrayType * ArgInfoArrayTy = ArrayType::get(ArgInfoTy, stack_info.size());		// Get the number of annotated point-type arguments
				if(ShadowStackTyVec.size() == 3) {
					ShadowStackTyVec[2] = ArgInfoArrayTy;
				} else {
					ShadowStackTyVec.push_back(ArgInfoArrayTy);
				}
				StructType *ShadowStackStructTy = StructType::create(ShadowStackTyVec, "shadow_stack_table");
				for(Function::arg_iterator it = Fn->arg_begin(); it != Fn->arg_end(); it++) {
					DEBUG(errs() << "[ShadowStack] Status: " << status << "\n");
					if(arg_info != stack_info.end() && it->getType()->isPtrOrPtrVectorTy() && (it->getArgNo() == (*arg_info)["Index"].asUInt())) {
						DEBUG(errs() << "[ShadowStack] Name: " << (*arg_info)["Name"].asString() << ", Index: " << (*arg_info)["Index"].asUInt() << ", ArgNo: " << it->getArgNo() << "\n");
						Constant *ptr_value;
						if(status < 4) {
							/* 说明指针保存在寄存器内 */
							ptr_value = get_ptr_info(M, ArgInfoTy, ~status, (*arg_info)["Count"].asUInt(), (*arg_info)["Size"].asUInt());
						} else {
							/* 说明指针保存在栈上 */
							ptr_value = get_ptr_info(M, ArgInfoTy, copy_size, (*arg_info)["Count"].asUInt(), (*arg_info)["Size"].asUInt());
						}
						total_size += (*arg_info)["Size"].asUInt();
						OpeValue.push_back(ptr_value);
						arg_info++;
					}
/* 这里很有可能有问题，else分支会比较稳妥一点，TODO: 换LLVM版本后使用else */
#if 1
					if(it->hasByValAttr()) {
						size =  M.getDataLayout().getTypeAllocSize(it->getType()->getPointerElementType());
					} else {
						size =  M.getDataLayout().getTypeAllocSize(it->getType());
					}
#else
					/* LLVM-10 version support, this version does not support */
					size = M.getDataLayout().getTypeAllocSize(it->getParamByValType());
					//it->getType()->dump();
#endif
					DEBUG(errs() << "[ShadowStack] Arg alloc size: " << size << "\n");
					if(status < 4) {
						status += ((size + 3)/4);
						if(status > 4)
							copy_size += (status - 4) * 4;
					} else {
						copy_size += size;
					}
				}
				DEBUG(errs() << "[ShadowStack] Create ArgInfoArray. Tysize: " << stack_info.size() << " Value size: " << OpeValue.size()<<"\n");
				Constant *ArgInfoArray = ConstantArray::get(ArgInfoArrayTy, OpeValue);
				OpeValue.clear();
				OpeValue.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), APInt(32, total_size)));
				OpeValue.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), APInt(32, stack_info.size())));
				OpeValue.push_back(ArgInfoArray);
				Constant *stack_table_value = ConstantStruct::get(ShadowStackStructTy, OpeValue);
				if(stack_info.size() || copy_size) {
					GlobalVariable *stack_table_variable = new GlobalVariable(M, ShadowStackStructTy, true, GlobalVariable::ExternalLinkage, stack_table_value, operation_name+"stack");
					stack_table_variable->setSection(".data");
					stack_table_ptr = ConstantExpr::getCast(Instruction::BitCast, stack_table_variable, Type::getInt8PtrTy(M.getContext()));
				} else {
					stack_table_ptr =  Constant::getIntegerValue(Type::getInt8PtrTy(M.getContext()), APInt(32, 0));
				}
				
				//GlobalVariable *stack_table_variable = new GlobalVariable(M, ShadowStackTyVec, true, GlobalVariable::ExternalLinkage, OpeInit, "__operation_"+operation_name);
				DEBUG(errs() << "[ShadowStack] Finish creating stack_table\n");
				OpeValue.clear();


				/* (3) 填入peripheral_mpu_region_list */
				Constant *peripheral_regionlist_ptr;
				Json::Value peripheral_region_list = operations[operation_name].get("Peripherals", "");
				uint32_t peripehral_region_num = peripheral_region_list.size();
				ArrayType * PeripheralRegionlistArrayTy = ArrayType::get(PeripheralRegionTy, peripehral_region_num);
				PeripheralRegionlistStructTyVec.push_back(PeripheralRegionlistArrayTy);
				StructType * PeripheralRegionlistStructTy = StructType::create(PeripheralRegionlistStructTyVec, "__operation_peripheral_region_list");
				PeripheralRegionlistStructTyVec.clear();

				/* Build Peripheral_Region_list */
				DEBUG(errs() << "[PeripheralRegion] Adding peripheral regionlist items\n");
				SmallVector<Constant *, 16> PeripheralArrayElementVec;
				Constant *Peripheral;
				DEBUG(errs() << "[PeripheralRegion] Peripheral regionlist size: " << peripehral_region_num << "\n");

				for(i = 0; i < peripehral_region_num; i++) {
					Json::Value PeriAttr = peripheral_region_list[i];
					DEBUG(errs() << "[PeripheralRegion] Adding peripheral: " << PeriAttr[2].asString() << "\n");

					SmallVector<Constant *, 12> PeripheralVaraibleVec;
					APInt peri_start_addr = APInt(32, PeriAttr[0].asUInt());
					APInt peri_rw = APInt(32, PeriAttr[1].asUInt());
					APInt peri_size = APInt(32, PeriAttr[3].asUInt());
					APInt peri_mpu_rbar = APInt(32, PeriAttr[4].asUInt());
					APInt peri_mpu_rasr = APInt(32, PeriAttr[5].asUInt());
					// APInt peri_activated = APInt(32, PeriAttr[6].asUInt());
					PeripheralVaraibleVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), peri_start_addr));
					PeripheralVaraibleVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), peri_rw));
					PeripheralVaraibleVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), peri_size));
					PeripheralVaraibleVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), peri_mpu_rbar));
					PeripheralVaraibleVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), peri_mpu_rasr));
					// PeripheralVaraibleVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), peri_activated));

					Peripheral = ConstantStruct::get(PeripheralRegionTy, PeripheralVaraibleVec);
					PeripheralArrayElementVec.push_back(Peripheral);
				}
				Constant *PeripheralArray = ConstantArray::get(PeripheralRegionlistArrayTy, PeripheralArrayElementVec);
				SmallVector<Constant *, 1> PeripheralRegionlistStructVec;
				PeripheralRegionlistStructVec.push_back(PeripheralArray);

				DEBUG(errs() << "[PeripheralRegion] Construct a Peripheral_Regionlist struct for " << operation_name << "\n");
				Constant *PeripheralRegionlistStruct = ConstantStruct::get(PeripheralRegionlistStructTy, PeripheralRegionlistStructVec);

				DEBUG(errs() << "[PeripheralRegion] Construct a pointer\n");
				GlobalVariable *peripheral_regionlist_variable = new GlobalVariable(M, PeripheralRegionlistStructTy, true, GlobalVariable::ExternalLinkage, PeripheralRegionlistStruct, operation_name+"PeripheralRegionlist");
				peripheral_regionlist_ptr = ConstantExpr::getCast(Instruction::BitCast, peripheral_regionlist_variable, Type::getInt8PtrTy(M.getContext()));
				PeripheralRegionlistStructVec.clear();
				DEBUG(errs() << "[PeripheralRegion] Finish adding peripehral region list items!\n");


				/* (4) 填入whitelist_size和ppbl, 参考shadow_stack_table */
				Constant *ppb_whitelist_ptr;
				Json::Value whitelist_names = operations[operation_name].get("Whitelist_Namelist", "");
				uint32_t whitelist_size_per_operation = whitelist_names.size();
				ArrayType *PPBWhitelistArrayTy = ArrayType::get(PPBRegTy, whitelist_size_per_operation);
				PPBWhitelistStructTyVec.push_back(PPBWhitelistArrayTy);
				StructType *WhitelistStructTy = StructType::create(PPBWhitelistStructTyVec, "__operation_whitelist");
				PPBWhitelistStructTyVec.clear();

				/* Build PPB Whitelist */
				DEBUG(errs() << "[PPBWhitelist] Adding whitelist items\n");
				SmallVector<Constant *, 16> PPBRegArrayElementVec;
				Constant *PPBReg;
				for(i = 0; i < whitelist_size_per_operation; i++) {
					Json::Value PPBRegName = whitelist_names[i];
					Json::Value PPBRegAttr = operations[operation_name]["Whitelist"][StringRef(PPBRegName.asString())];
					DEBUG(errs() << "[PPBWhitelist] Adding PPB: " << PPBRegName.asString() << "\n");

					SmallVector<Constant *, 12> PPBRegVariableVec;
					APInt ppb_reg_start_addr = APInt(32, PPBRegAttr[0].asUInt());
					APInt ppb_reg_size = APInt(32, PPBRegAttr[1].asUInt());
					APInt ppb_reg_rw = APInt(32, PPBRegAttr[2].asUInt());
					PPBRegVariableVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), ppb_reg_start_addr));
					PPBRegVariableVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), ppb_reg_size));
					PPBRegVariableVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), ppb_reg_rw));

					PPBReg = ConstantStruct::get(PPBRegTy, PPBRegVariableVec);
					PPBRegArrayElementVec.push_back(PPBReg);
				}

				Constant *PPBRegArray = ConstantArray::get(PPBWhitelistArrayTy, PPBRegArrayElementVec);
				SmallVector<Constant *, 1> PPBWhilistStructVec;
				PPBWhilistStructVec.push_back(PPBRegArray);

				DEBUG(errs() << "[PPBWhitelist] Construct a Whitelist struct for " << operation_name << "\n");
				Constant *PPBWhitelistStruct = ConstantStruct::get(WhitelistStructTy, PPBWhilistStructVec);
				DEBUG(errs() << "[PPBWhitelist] Construct a pointer\n");
				if(whitelist_size_per_operation) {
					GlobalVariable *ppb_whitelist_variable = new GlobalVariable(M, WhitelistStructTy, true, GlobalVariable::ExternalLinkage, PPBWhitelistStruct, operation_name+"Whitelist");
					ppb_whitelist_ptr = ConstantExpr::getCast(Instruction::BitCast, ppb_whitelist_variable, Type::getInt8PtrTy(M.getContext()));
				} else {
					ppb_whitelist_ptr = Constant::getIntegerValue(Type::getInt8PtrTy(M.getContext()), APInt(32, 0));
				}
				PPBWhilistStructVec.clear();

				DEBUG(errs() << "[PPBWhitelist] Finish adding whitelist items!\n");


				/* (5) 遍历JSON得到信息，往policy结构中填shadowdata数据 */
				/* 读json中的shadowdata的name，在IR全局变量中找对应的地址 */
				Json::Value external_variable_names = operations[operation_name]["ExternalData"];
				Json::Value external_variable_fields_info = operations[operation_name]["ExternalDataFields"];
				Json::Value external_variable_san_info = operations[operation_name]["ExternalDataSan"];

				ArrayType *ShadowSectionTy = ArrayType::get(ShadowTblTy, external_variable_names.size());

				/* 如果policy struct扩展或者删除成员类型，需要修改1) if条件==右侧的值; 2) if taken branch的index */
				if(OpeTyVec.size() == 11) {
					DEBUG(errs() << "[ShadowData] If taken, OpeTyVec.size()==" << OpeTyVec.size() << "\n");
					OpeTyVec[10] = ShadowSectionTy;
				} else {
					DEBUG(errs() << "[ShadowData] If not taken, OpeTyVec.size()==" << OpeTyVec.size() << "\n");
					OpeTyVec.push_back(ShadowSectionTy);
				}
				/* 创建每个operation自己的operation_type, 因为每个operation的ShadowSectionType size不同 */
				StructType * OpeTy = StructType::create(OpeTyVec, "__operations");

				DEBUG(errs() << "[ShadowData] Building ShadowData Region\n");
				SmallVector<Constant *, 16> ShadowTableVec;
				for(i = 0; i < external_variable_names.size(); i++) {
					Constant *ShadowDataTable;
					ShadowDataTable = set_shadow_table_entities(M, PointerFieldTy, ShadowTblTy, external_variable_names[i], external_variable_fields_info, external_variable_san_info, OInfo.GetMD(operation_name), OPERATION_MPU_RW);
					ShadowTableVec.push_back(ShadowDataTable);
				}
				Constant *ShadowTables = ConstantArray::get(ShadowSectionTy, ShadowTableVec);

				/* (6) Build MPU Regions */
				DEBUG(errs() << "Building MPU regions\n");
				/* 读json中的MPU配置信息 */
				Json::Value Attrs = operations[operation_name]["MPU_Config"]["Attr"];
				Json::Value Addrs = operations[operation_name]["MPU_Config"]["Addr"];
				SmallVector<Constant *, 16> MPURegionVec;
				for(i = 0; i < num_mpu_regions.asUInt(); i++){
					Constant *Region;
					if (i < Attrs.size()){
						Region = get_MPU_region_data(M, RegionTy, Addrs[i].asUInt(), Attrs[i].asUInt());
					} else {
						Region = get_MPU_region_data(M, RegionTy, 0, 0);
					}
					MPURegionVec.push_back(Region);
				}
				Constant *MPRegions = ConstantArray::get(MPURegionTy, MPURegionVec);
				uint32_t operation_name_len = operation_name.length();
				uint32_t ope_id = (uint32_t)(operation_name[operation_name_len-2] - '0');		/* e.g., _Operation_0_ */
				APInt operation_id = APInt(32, ope_id);
				DEBUG(errs() << ope_id << "\n");

				OpeValue.push_back(MPRegions);
				OpeValue.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), operation_id));
				OpeValue.push_back(ope_data_sec_info_ptr);
				OpeValue.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), APInt(32, copy_size)));
				OpeValue.push_back(stack_table_ptr);
				OpeValue.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), APInt(32, peripehral_region_num)));
				OpeValue.push_back(peripheral_regionlist_ptr);
				OpeValue.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), APInt(32, whitelist_size_per_operation)));
				OpeValue.push_back(ppb_whitelist_ptr);
				OpeValue.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), APInt(32, external_variable_names.size())));
				OpeValue.push_back(ShadowTables);

				DEBUG(errs() << "Adding data to .data section\n");
				DEBUG(errs() << "Type Number: " << OpeTy->getNumElements() << "\n");
				DEBUG(errs() << "Size Number: " << OpeValue.size() << "\n");
				Constant *OpeInit = ConstantStruct::get(OpeTy, OpeValue);
				GlobalVariable *Operation = new GlobalVariable(M, OpeTy, true, GlobalVariable::ExternalLinkage, OpeInit, "__operation_"+operation_name);
				/* policy metadata should be writeable during operation switch */
				Operation->setSection(".rodata");
				DEBUG(errs() << "Finish adding data to .rodata or .data section\n");

				if(operation_name == "_default_") {
					ret = Operation;
				}

				/* (7) PolicyInfo */
				Constant *policy_ptr = ConstantExpr::getCast(Instruction::BitCast, Operation, Type::getInt8PtrTy(M.getContext()));
				SmallVector<Constant *, 12> PolicyInfoVariableVec;
				PolicyInfoVariableVec.push_back(policy_ptr);
				PolicyInfoVariableVec.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), operation_id));
				PolicyInfo = ConstantStruct::get(PolicyInfoTy, PolicyInfoVariableVec);
				PolicyInfoArrayElementVec.push_back(PolicyInfo);

				/* At this moment, the metadata of an operation is created */
				// Operation->dump();
#if 0
				Constant *OperationPtr = ConstantExpr::getCast(Instruction::BitCast, Operation, OperationTable->getValueType());
				OperationPtrVec.push_back(OperationPtr);
#endif
				DEBUG(errs() << "[+] End " << operation_name << "\n");
			}
			Constant *PolicyInfoArray = ConstantArray::get(PolicyInfoArrayTy, PolicyInfoArrayElementVec);
			SmallVector<Constant *, 1> PolicyInfoStructVec;
			PolicyInfoStructVec.push_back(PolicyInfoArray);
			Constant *PolicyInfoStruct = ConstantStruct::get(PolicyInfoStructTy, PolicyInfoStructVec);
			GlobalVariable *policy_info_variable = new GlobalVariable(M, PolicyInfoStructTy, true, GlobalVariable::ExternalLinkage, PolicyInfoStruct, "policies_info_variable");
			Constant *policy_info_ptr = ConstantExpr::getCast(Instruction::BitCast, policy_info_variable, Type::getInt8PtrTy(M.getContext()));
			SmallVector<Constant *, 2> PoliciesInfoValue;
			PoliciesInfoValue.push_back(Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), APInt(32, operations.size())));
			PoliciesInfoValue.push_back(policy_info_ptr);
			Constant *PoliciesInfoInit = ConstantStruct::get(PoliciesInfoTy, PoliciesInfoValue);
			GlobalVariable *PoliciesInfo = new GlobalVariable(M, PoliciesInfoTy, true, GlobalVariable::ExternalLinkage, PoliciesInfoInit, "Policies_Info");
			// GlobalVariable *PoliciesInfo = M.getGlobalVariable("Policies_Info", true);
			// PoliciesInfo->setInitializer(PoliciesInfoInit);
			PoliciesInfo->setSection(".rodata");
#if 0
			Constant *OperationPtrTbl = ConstantArray::get(OperationPtrTyVec, OperationPtrVec);
			GlobalVariable *OperationTableG = new GlobalVariable(M, OperationPtrTyVec, true, GlobalVariable::ExternalLinkage, OperationPtrTbl, "OperationTableG");
			OperationTableG->setSection(".rodata");
			//OperationTable->getType()->dump();
			//OperationTableG->getType()->dump();
			OperationTable->setInitializer(ConstantExpr::getCast(Instruction::BitCast, OperationTableG, OperationTable->getValueType()));
#endif
			return ret;
		}


		bool runOnModule(Module &M) override {
			GlobalVariable *default_operation_policy;
			std::ifstream PolicyFile, MemoryPoolFile;
			Json::Value PolicyRoot, MemoryPoolRoot;

			if(OIPolicy.compare("-") == 0)
				return false;
			PolicyFile.open(OIPolicy);
			PolicyFile >> PolicyRoot;
			PolicyFile.close();

			MemoryPoolFile.open(MemoryPool);
			MemoryPoolFile >> MemoryPoolRoot;
			MemoryPoolFile.close();

			set_memory_pool_variable_section(M, MemoryPoolRoot);
			DEBUG(errs()<<"[+] Finish set_memory_pool_variable_section!\n");

			/* 
			 * collect operation information, create shadow data for external global variables
			 * set section for shadow global variables and internal global variables
			 */
			generate_OperationInfo_and_handle_data(M, PolicyRoot);
			DEBUG(errs()<<"[+] Finish generate_OperationInfo_and_handle_data!\n");
			/* change memory access from global variables to shadow global variables */
			change_external_data_access(M, &OInfo.GVptr);
			DEBUG(errs()<<"[+] Finish change_external_data_access!\n");

			// print_global_use(M);

			/* create policy section */
			default_operation_policy = build_policy_section(M, PolicyRoot);
			DEBUG(errs()<<"build_policy_section finish!\n");
			assert(default_operation_policy && "Lack of default policy!");
			// M.getGlobalList().push_back(default_operation_policy);
			
			/* replace main function with operation init */
			interceptMain(M, PolicyRoot, default_operation_policy);
			DEBUG(errs()<<"interceptMain finish!\n");
			// errs()<<"Setting the value of default_operation_policy...\n";
			// GlobalVariable *default_policy = M.getNamedGlobal("default_operation_policy");
			// for(auto gv_user: default_policy->users()) {
			// 	if (Instruction * inst_ld_gv = dyn_cast<Instruction>(gv_user)) {
			// 		inst_ld_gv->setOperand(0, default_operation_policy);
			// 		inst_ld_gv->dump();
			// 	}
			// }
			// errs()<<"Finish setting the value of default_operation_policy!\n";
			
			/* 修改了module，所以返回true */
			return true;
		}


		bool doFinalization(Module &M) override {
			if ( OIPolicy.compare("-") == 0 )
				return false;
			return false;
		}


		bool isConstAddr(Instruction * Inst) {
			Value *operand = nullptr;
			bool usesConstAddress = false;
			if(isa<LoadInst>(Inst)) {
				operand = Inst->getOperand(0);
			} else if (isa<StoreInst>(Inst)) {
				operand = Inst->getOperand(1);
			} else {
				return false;
			}
			if (operand) {
				/* captures direct use of inttoptr  e.g inttoptr (i32 40020000 to i32*) */
				if (ConstantExpr * intPtr = dyn_cast<ConstantExpr>(operand)) {
					Instruction * inst = intPtr->getAsInstruction();
					if (CastInst * castPtr = dyn_cast<CastInst>(inst)) {
						/* know it is a constant cast to a pointer */
						if (castPtr->getDestTy()->isPointerTy()) {
							castPtr->getOperand(0);
							if(ConstantInt *constInt = dyn_cast<ConstantInt>(castPtr->getOperand(0))) {
								DEBUG(errs() << "Address is: " << constInt->getValue());
								//Function *inst_2_func = Inst->getParent()->getParent();
								//inst_2_func->printAsOperand(errs(), true, NULL);
								//errs() << inst_2_func->getName();
								DEBUG(errs() << "\n");
							}
							usesConstAddress = true;
						}
					}
				}
			}
			return usesConstAddress;
		}

#if 0
		void print_global_use(Module &M) {
			for(Module::global_iterator it=M.global_begin(); it!=M.global_end(); it++) {
				for(User::use_iterator uit=it->use_begin(); uit!=it->use_end(); uit++) {
					uit->dump();
				}
			}
		}
#endif
#if 0
		 void getAnalysisUsage(AnalysisUsage &AU) const override {
			AU.addRequiredTransitive<AliasAnalysis>();
			AU.addPreserved<AliasAnalysis>();
		}
#endif
#if 0
		/**
		 * @brief 如果原来的Operation不是一个function，我们需要将原来的code wapper成为一个Operation的function。
		 * 如果原来的我们先假设用户的operation就是function。后面对Operation的操作，就是对Operation function的操作。
		 * 这里我们的wrapper函数名称是唯一的。
		 * 对于entry已经是一个function的，我们会创建另外一个全局变量为function的别名，然后调用这个别名。
		 * 
		 * @param M 
		 * @param Root 
		 * @param operation_name 
		 * @return Function* 
		 */
		Function *generate_operation_function(Module &M, Json::Value &Root, stringRef operation_name) {
			//默认为一个完整的function
			Function *entry; 
			Json::Value OperationRegion = Root.get("Operation", "");
			Json::Value Region = OperationRegion[operation_name];
			
			entry = M.getFunction()
		}
#endif
		/**
		 * @brief interceptMain
		 * This initializes operation.  It does it by renaming main to __original_main and then creates a new main.
		 * Taking the name main is required because assembly is used to initialize the device and then calls main. 
		 * LLVM can analyze the assembly, so we hijack the symbol name. 
		 * The main built initializes each compartment bss and data section.
		 * Then ...
		 * 
		 * @param M 
		 * @param PolicyRoot 
		 * @param default_policy 
		 */
		void interceptMain(Module & M, Json::Value PolicyRoot, GlobalVariable *default_policy) {
            Function * OrgMain = M.getFunction("main");
            Function * InitMain;
            IRBuilder<> *IRB;
            assert(OrgMain && "Main not found");
            OrgMain->setName("__original_main");
            DEBUG(OrgMain->getFunctionType()->dump());

            InitMain = Function::Create(OrgMain->getFunctionType(), OrgMain->getLinkage(), "main", &M);
            InitMain->addFnAttr(Attribute::NoUnwind);
            BasicBlock * BB = BasicBlock::Create(M.getContext(), "entry", InitMain);
            IRB = new IRBuilder<>(M.getContext());
            IRB->SetInsertPoint(BB);

            //initBssAndDataSections(M, IRB, PolicyRoot);
			initOperationDataSections(M, IRB, PolicyRoot);
			DEBUG(errs()<<"initOperationDataSections finished\n");


			initMemoryHeapPool(M, IRB, PolicyRoot);
			DEBUG(errs()<<"initMemoryHeapPool finished\n");

			//std::string OperationTableName = "";
			//std::string OperationNum = "";

			//initOperationStartFunc(M, IRB, OperationTableName, operationNum);
			initOperationStartFunc(M, IRB, default_policy);
			DEBUG(errs()<<"initOperationStartFunc finished\n");

			Function * environment_init = M.getFunction("__environment_init");
			assert(environment_init&&"No __environment_init Found");
			IRB->CreateCall(environment_init);

			SmallVector<Value *,1> Args;
			Value * V;
			for (auto & arg : InitMain->args()){
				Args.push_back(&arg);
			}
			DEBUG(errs()<<"Create call to old main\n");
			/* 调用old_main */
			V = IRB->CreateCall(OrgMain, Args);
			//SmallVector<Function *, 12> Callees;
			//Callees.push_back(OrgMain);
			//CallSite cs = CallSite(V);
			//addTransition(M, cs, Callees);
			IRB->CreateUnreachable();

			delete IRB;
		}


		void initOperationDataSections(Module &M, IRBuilder<> * IRB, Json::Value &Root)
		{
			OperationMetadata *omd;
			Json::Value OperationRegions = Root.get("Operation", "");
			DEBUG(errs()<<"initOperationDataSections\n");
			for (auto operation_name: OperationRegions.getMemberNames())
			{
				DEBUG(errs()<<"iterations for moving data\n");
				//Json::Value Operation = OperationRegions[operation_name];
				omd = OInfo.GetMD(operation_name);
				if(!omd) {
					DEBUG(errs() << "initOperationDataSections search operation fail! " << operation_name << "\n");
					assert(false && "initOperationDataSections get operation name fail!");
				}

				if(omd->InternalGV.size() || omd->ExternalGV.size()) {
					std::string section_name = (std::string)operation_name + "_data";
					std::string FlashStart	= section_name + "_vma_start";
					std::string SRAMStart 	= section_name + "_start";
					std::string SRAMEnd     = section_name + "_end";

					StringRef FlashStartVar = StringRef(FlashStart);
					StringRef SRAMStartVar  = StringRef(SRAMStart);
					StringRef SRAMEndVar    = StringRef(SRAMEnd);

					add_operation_data_section_init(M, IRB, FlashStartVar, SRAMStartVar, SRAMEndVar);
				}
			}
		}

		/**
		 * @brief Set memory heap memory pool sections to zero, since these variables were in .bss section.
		 * 
		 * @param M 
		 * @param IRB 
		 * @param Root 
		 */
		void initMemoryHeapPool(Module &M, IRBuilder<> *IRB, Json::Value &Root)
		{
			Json::Value MemoryPoolRegions = Root.get("MemoryPool", "");
			for (auto memory_pool_name: MemoryPoolRegions.getMemberNames())
			{
				DEBUG(errs()<<"iterations for clear memory heap/pool sections\n");
				std::string section_name = "memp_" + (std::string)memory_pool_name;
				std::string start_addr   = section_name + "_start";
				std::string end_addr     = section_name + "_end";

				StringRef DataStartVar = StringRef(start_addr);
				StringRef DataEndVar   = StringRef(end_addr);

				add_memory_heap_pool_init(M, IRB, DataStartVar, DataEndVar);
			}
		}


		void initOperationStartFunc(Module &M, IRBuilder<> * IRB, GlobalVariable *policy)
		{
			std::vector<Value *> CallParams;
			Function * OperationStartFn = M.getFunction("__operation_start");
            assert(OperationStartFn && "Function operation_start not found");
			/*
			Type *arg0Type = Type::getInt32Ty(M.getContext());
			Type *arg1Type = Type::getInt32Ty(M.getContext());
			
			Value *OperationTablePtr = M.getOrInsertGlobal(OperationTableName, arg0Type);
			Value *OperationNumPtr = M.getOrInsertGloabl(OperationNum, arg1Type);

			assert(OperationTablePtr && OperationNumPtr && "OperationTableName and OperationNum need but not found");

			OperationTablePtr = IRB->CreateIntToPtr(OperationTablePtr, Type::getInt32PtrTy(M.getContext()));
			OperationNumPtr = IRB->CreateIntToPtr(OperationNumPtr, Type::getInt32PtrTy(M.getContext()));
			*/
			OperationStartFn->arg_begin()->getType()->dump();
			Constant *arg = ConstantExpr::getCast(Instruction::BitCast, policy, OperationStartFn->arg_begin()->getType());
			CallParams.push_back(arg);
			IRB->CreateCall(OperationStartFn, CallParams);
			return;
		}


		void add_operation_data_section_init(Module &M, IRBuilder<> *IRB, StringRef &FlashStart, StringRef &SRAMStart, StringRef &SRAMStop)
		{
			Function *func_init_operation_datasec = M.getFunction("__operation_init_data_section");
			assert(func_init_operation_datasec && "Can't find initialization routine check RT Lib");
#if 1
			/* 由于pass在run的时候，还没有这个global的存在(这个global在链接脚本中)，所以这里需要insert一个。这也行宏else中代码会出错的原因 */
			Type *arg0Type = Type::getInt32Ty(M.getContext());
			Type *arg1Type = Type::getInt32Ty(M.getContext());
			Type *arg2Type = Type::getInt32Ty(M.getContext());

			/* global define at linkscript */
			Value *FlashAddr   = M.getOrInsertGlobal(FlashStart,   arg0Type);
			Value *SRAMStartAddr = M.getOrInsertGlobal(SRAMStart, arg1Type);
			Value *SRAMStopAddr  = M.getOrInsertGlobal(SRAMStop,  arg2Type);
#else
			Value *SRAMStartAddr =M.getGlobalVariable(SRAMStart, true);
			if(!SRAMStartAddr) {
				errs() << "Can't find global " << SRAMStart << "\n";
				SRAMStartAddr = Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), APInt(32, 0));
			}
			Value *SRAMStopAddr = M.getGlobalVariable(SRAMStop, true);
			if(!SRAMStopAddr) {
				errs() << "Can't find global " << SRAMStop << "\n";
				SRAMStopAddr = Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), APInt(32, 0));
			}
			Value *FlashAddr = M.getGlobalVariable(FlashStart, true);
			if(!FlashAddr) {
				errs() << "Can't find global " << FlashStart << "\n";
				FlashAddr = Constant::getIntegerValue(Type::getInt32Ty(M.getContext()), APInt(32, 0));
			}
#endif
#if 0
			assert(SRAMStartAddr && SRAMStopAddr && VMAAddr && \
				"Data Section Addresses Need but not found");
#endif
			std::vector<Value *> CallParams;
			FlashAddr = IRB->CreateIntToPtr(FlashAddr, Type::getInt32PtrTy(M.getContext()));
			SRAMStartAddr = IRB->CreateIntToPtr(SRAMStartAddr, Type::getInt32PtrTy(M.getContext()));
			SRAMStopAddr = IRB->CreateIntToPtr(SRAMStopAddr, Type::getInt32PtrTy(M.getContext()));

			CallParams.push_back(FlashAddr);
			CallParams.push_back(SRAMStartAddr);
			CallParams.push_back(SRAMStopAddr);
			IRB->CreateCall(func_init_operation_datasec, CallParams);
		}


		void add_memory_heap_pool_init(Module &M, IRBuilder<> *IRB, StringRef &SRAMStart, StringRef &SRAMStop) {
			Function *func_clear_memh_memp = M.getFunction("__clear_memh_memp");
			assert(func_clear_memh_memp && "Can't find func_clear_memh_memp");

			Type *arg0Type = Type::getInt32Ty(M.getContext());
			Type *arg1Type = Type::getInt32Ty(M.getContext());

			Value *SRAMStartAddr = M.getOrInsertGlobal(SRAMStart, arg0Type);
			Value *SRAMStopAddr  = M.getOrInsertGlobal(SRAMStop,  arg1Type);

			std::vector<Value *> CallParams;
			SRAMStartAddr = IRB->CreateIntToPtr(SRAMStartAddr, Type::getInt32PtrTy(M.getContext()));
			SRAMStopAddr = IRB->CreateIntToPtr(SRAMStopAddr, Type::getInt32PtrTy(M.getContext()));
			CallParams.push_back(SRAMStartAddr);
			CallParams.push_back(SRAMStopAddr);
			IRB->CreateCall(func_clear_memh_memp, CallParams);
		}



		void get_call_inst(Module &M) {
			for (Function &F : M) {
				for(BasicBlock &B : F) {
					for(Instruction &I: B) {
						if(CallSite cs = CallSite(&I)) {
							Function *callee = cs.getCalledFunction();
							OperationMetadata *md = OInfo.GetMD(callee);
							if(md) {
								if(md->inst) {
									DEBUG(errs() << "call operation entry function " << callee->getName() << "more than once!\n");
									assert(false);
								}
								md->inst = dyn_cast<CallInst>(&I);
							}
						}
					}
				}
			}
			return;
		}


		/**
		 * @brief Set the memory pool variable to each section
		 * 
		 * @param M 
		 * @param MemoryPoolRoot 
		 */
		void set_memory_pool_variable_section(Module &M, Json::Value &MemoryPoolRoot) {
			for (JSONCPP_STRING memory_pool_name : MemoryPoolRoot.getMemberNames()) {
				Json::Value MPNode = MemoryPoolRoot[memory_pool_name];
				for (JSONCPP_STRING var_name_str : MPNode.getMemberNames()) {
					StringRef var_name_strref = StringRef(var_name_str);
					if (var_name_strref.startswith("aligned_size"))
						continue;
					else {
						if(GlobalVariable *GV_MP = M.getNamedGlobal(var_name_str)) {
							GV_MP->setSection(StringRef("memp_" + memory_pool_name));
							DEBUG(errs() << "Section name: " << "memp_" << memory_pool_name << "\n");
						}
					}
				}
			}
		}


		/**
		 * @brief 为operation需要访问的external global data创建shadow data, 并将所有的external and internal data放在特定section
		 * 
		 * @param M 
		 * @param Root 
		 */
		void generate_OperationInfo_and_handle_data(Module &M, Json::Value &Root) {
			Function *entry_function;
			GlobalVariable *GV, *shadow_GV, *GV_ptr;
			DenseMap<GlobalVariable *, GlobalVariable *>::iterator it;
			std::string DataSection;
			//DenseMap<Function *, OperationMetadata *> *ret = new DenseMap<Function *, OperationMetadata *>;
			Json::Value OperationRegion = Root.get("Operation", "");

			for (JSONCPP_STRING operation_name : OperationRegion.getMemberNames()) {
				DataSection = operation_name + "_data";
				Json::Value Region = OperationRegion[operation_name];
				OperationMetadata *omd = new OperationMetadata;

				for(Json::Value gv_name : Region.get("ExternalData", "")) {
					/* for shared global variables we generate a shadow copy */
					GV = M.getGlobalVariable(StringRef(gv_name.asString()), true);
					if(GV) {
						// shadow data don't need Initializer cause it will be updated when entering an operation
						DEBUG(std::cout << "[ExternalData] Find gloabl variable " << gv_name << std::endl);
						shadow_GV = new GlobalVariable(M, GV->getValueType(), GV->isConstant(), GlobalVariable::InternalLinkage, Constant::getNullValue(GV->getValueType()), operation_name + gv_name.asString());
						shadow_GV->setAlignment(GV->getAlignment());
						// shadow_GV->setAlignment(M.getDataLayout().getTypeAllocSize(GV->getValueType()));
						shadow_GV->setSection(StringRef(DataSection));
						DEBUG(errs() << "[ExternalData] Generate new global variables: " << shadow_GV->getName() << " Alignment: " << shadow_GV->getAlignment() << "\n");
						omd->ExternalGV.insert(std::make_pair(GV, shadow_GV));
						it = OInfo.GVptr.find(GV);
						if(it == OInfo.GVptr.end()) {
							GV_ptr = new GlobalVariable(M, GV->getType(), GV->isConstant(), GlobalVariable::InternalLinkage, GV, gv_name.asString() + "_ptr");
							OInfo.GVptr.insert(std::make_pair(GV, GV_ptr));
						}
					} else {
						DEBUG(errs() << "[ExternalData] Failed to found gloabl variable: " << gv_name.asString() << "\n");
					}
				}
				for(Json::Value gv_name : Region.get("InternalData", "")) {
					/* for global data on use in one operation, we only can the section */
					GV = M.getGlobalVariable(StringRef(gv_name.asString()), true);
					GV->setSection(StringRef(DataSection));
					omd->InternalGV.insert(GV);
				}

				/* 处理operation entry function */
				Json::Value function_value = Region["Entry"];
				DEBUG(std::cout << "Operation Entry: " << function_value << "\n");
				entry_function = M.getFunction(StringRef(function_value.asString()));
				if(entry_function) {
					if(entry_function->getName() != "main")
						entry_function->addFnAttr("OperationEntry", operation_name);
					omd->func = entry_function;
					OInfo.insert_metadata(entry_function, operation_name, omd);
				} else {
					DEBUG(errs() << "can't find operation enter function: " << entry_function << "\n");
					assert(false);
				}
			}
			get_call_inst(M);
			return;
		}


		/* 遍历程序所有IR Instruction，如果一个Instruction访问了External global data，则在Instruction之前insert一条LoadInst, 改为对external global data ptr的先load后访问 */
		LoadInst *find_and_replace(Instruction &I,  GlobalVariable *GV, DenseMap<GlobalVariable *, GlobalVariable *> *GVptr)
		{
			DenseMap<GlobalVariable *, GlobalVariable *>::iterator it;
			LoadInst *LI;
			bool is_volatile = false;
			
			it = GVptr->find(GV);
			if(it != GVptr->end()) {
				DEBUG(errs() << "Change global value access: " << GV->getName() <<"\n");

#ifdef INSERT_LoadInst_CONSIDER_VOLATILE
				if (LoadInst *tmp_loadinst = dyn_cast<LoadInst>(&I)) {
					is_volatile = tmp_loadinst->isVolatile();
				}
				else if (StoreInst *tmp_storeinst = dyn_cast<StoreInst>(&I)) {
					is_volatile = tmp_storeinst->isVolatile();
				}
				// DEBUG(errs() << "volatile: " << is_volatile << "\n");
#endif

				LI = new LoadInst(it->first->getType(), it->second, it->first->getName() + "_p", is_volatile, &I);
				// LI->dump();

				return LI;
			}
			return NULL;
		}


		GetElementPtrInst *gepo_find_and_replace(Instruction &I, GEPOperator *gepo, GlobalVariable *GV, DenseMap<GlobalVariable *, GlobalVariable *> *GVptr)
		{
			DenseMap<GlobalVariable *, GlobalVariable *>::iterator it;
			LoadInst *LI;
			SmallVector<Value *, 4> Idx;
			bool is_volatile = false;
			
			it = GVptr->find(GV);
			if(it != GVptr->end()) {
				DEBUG(errs() << "Change global value access: " << GV->getName() <<"\n");

#ifdef INSERT_LoadInst_CONSIDER_VOLATILE
				if (LoadInst *tmp_loadinst = dyn_cast<LoadInst>(&I)) {
					is_volatile = tmp_loadinst->isVolatile();
				}
				else if (StoreInst *tmp_storeinst = dyn_cast<StoreInst>(&I)) {
					is_volatile = tmp_storeinst->isVolatile();
				}
				// DEBUG(errs() << "volatile: " << is_volatile << "\n");
#endif
				LI = new LoadInst(it->first->getType(), it->second, it->first->getName() + "_p", is_volatile, &I);
				// LI->dump();

				//Idx.push_back(LI);
				//LI->getType()->dump();
				for (auto it = gepo->idx_begin(), et = gepo->idx_end(); it != et; ++it) {
					Idx.push_back(*it);
					//dyn_cast<Value>(*it)->dump();
				}
#if 0
				for(unsigned x=0; x<geop->getNumOperands(); x++) {
					Idx.push_back(gepo->getOperand(x));
				}
#endif
				//ArrayRef<Value *> IdxList(Idx);
				return GetElementPtrInst::CreateInBounds(it->first->getValueType(), LI, ArrayRef<Value*>(Idx), it->first->getName() + "_gep", &I);
				//GI->dump();
				//I.setOperand(i, GI);
				//I.dump();
			}
			return NULL;
		}

		GetElementPtrInst *gepo_index_find_and_replace(Instruction &I, unsigned i, GEPOperator *gepo, GlobalVariable *GV, DenseMap<GlobalVariable *, GlobalVariable *> *GVptr) {
			DenseMap<GlobalVariable *, GlobalVariable *>::iterator it;
			SmallVector<Value *, 4> Idx;
			LoadInst *LI;
			bool is_volatile = false;

			it = GVptr->find(GV);
			if(it != GVptr->end()) {

#ifdef INSERT_LoadInst_CONSIDER_VOLATILE
				if (LoadInst *tmp_loadinst = dyn_cast<LoadInst>(&I)) {
					is_volatile = tmp_loadinst->isVolatile();
				}
				else if (StoreInst *tmp_storeinst = dyn_cast<StoreInst>(&I)) {
					is_volatile = tmp_storeinst->isVolatile();
				}
				// DEBUG(errs() << "volatile: " << is_volatile << "\n");
#endif

				LI = new LoadInst(it->first->getType(), it->second, it->first->getName() + "_p", is_volatile, &I);
				// LI->dump();

				for (auto it = gepo->idx_begin(), et = gepo->idx_end(); it != et; ++it) {
					Idx.push_back(*it);
				}
				Idx[i-1] = LI; 
				return GetElementPtrInst::CreateInBounds(it->first->getValueType(), gepo->getPointerOperand(), ArrayRef<Value*>(Idx), it->first->getName() + "_gep", &I);
			}
			return NULL;
		}


		void change_external_data_access(Module &M, DenseMap<GlobalVariable *, GlobalVariable *> *GVPtrMap) {
			for (Function &F : M) {
				DEBUG(errs() << "[Function]: " << F.getName() << "\n");
				for (BasicBlock &B : F) {
					for (Instruction &I : B) {
						for (unsigned i=0; i < I.getNumOperands(); i++) {
							Value *V = I.getOperand(i);
							if (GlobalVariable *GV = dyn_cast_or_null<GlobalVariable>(V)) {
								// DEBUG(errs() << "#1.0\n");
								LoadInst *LI = find_and_replace(I, GV, GVPtrMap);
								// DEBUG(errs() << "#1.1\n");
								if (LI) {
									// DEBUG(errs() << "#1.2\n");
									I.setOperand(i, LI);
									// DEBUG(errs() << "#1.3\n");
								}
							}

							else if (GEPOperator* gepo = dyn_cast<GEPOperator>(V)) {
								/* 看指针是否指向全局变量 */
								// DEBUG(errs() << "#2.0\n");
								if (GlobalVariable *GV = dyn_cast_or_null<GlobalVariable>(gepo->getPointerOperand())) {
									gepo->dump();
									// DEBUG(errs() << "#2.1\n");
									if (GetElementPtrInst *GI = gepo_find_and_replace(I, gepo, GV, GVPtrMap)) {
										// DEBUG(errs() << "#2.2\n");
										I.setOperand(i, GI);
										// DEBUG(errs() << "#2.3\n");
									}
								}
								/**
								 *  %0 = load %struct.__DMA_HandleTypeDef.80*, %struct.__DMA_HandleTypeDef.80** getelementptr inbounds (%struct.SD_HandleTypeDef.82, %struct.SD_HandleTypeDef.82* bitcast (%struct.SD_HandleTypeDef* @uSdHandle to %struct.SD_HandleTypeDef.82*), i32 0, i32 11)
								 * 
								 */
								else if (BitCastOperator *BCO = dyn_cast<BitCastOperator>(gepo->getPointerOperand())) {
									// DEBUG(errs() << "#3.4\n");
									BCO->dump();
									if (GlobalVariable *GV = dyn_cast<GlobalVariable>(gepo->getPointerOperand()->stripPointerCasts())) {
										// DEBUG(errs() << "#3.5\n");
										LoadInst *LI = find_and_replace(I, GV, GVPtrMap);
										if (LI) {
											if (BitCastInst *BCI = new BitCastInst(LI, BCO->getDestTy(), GV->getName()+"_bitcast", &I)) {
												SmallVector<Value *, 4> Idx;
												for (auto it = gepo->idx_begin(), et = gepo->idx_end(); it != et; ++it) {
													Idx.push_back(*it);
												}
												GetElementPtrInst *GEPI = GetElementPtrInst::CreateInBounds(gepo->getSourceElementType(), BCI, ArrayRef<Value*>(Idx), GV->getName()+"_gep", &I);
												I.setOperand(i, GEPI);
											}
										}
									}

								}
								/* 看索引是否是全局变量 */
								for(unsigned j=1; j<gepo->getNumOperands(); j++) {
									// DEBUG(errs() << "#3.0\n");
									if (GlobalVariable* GV = dyn_cast<GlobalVariable>(gepo->getOperand(j))) {
										//DEBUG(errs() << "find global variables from index\n");
										// DEBUG(errs() << "#3.1\n");
										GetElementPtrInst *GI = gepo_index_find_and_replace(I, j, gepo, GV, GVPtrMap);
										if (GI) {
											// DEBUG(errs() << "#3.2\n");
											I.setOperand(i, GI);
											// DEBUG(errs() << "#3.3\n");
										}
									}
								}
							}

							else if (BitCastOperator *BCO = dyn_cast<BitCastOperator>(V)) {
								// DEBUG(errs() << "#4.0\n");
								if (GlobalVariable *GV = dyn_cast<GlobalVariable>(V->stripPointerCasts())) {
									// DEBUG(errs() << "#4.1\n");
									I.dump();
									if (GV->hasName() && !(GV->getName().endswith("_ptr"))) {
										// DEBUG(errs() << "#4.2\n");
										LoadInst *LI = find_and_replace(I, GV, GVPtrMap);
										// DEBUG(errs() << "#4.3\n");
										if (LI) {
											// DEBUG(errs() << "#4.4\n");
											BitCastInst *BCI = new BitCastInst(LI, BCO->getDestTy(), GV->getName()+"_bitcast", &I);
											I.setOperand(i, BCI);
											// DEBUG(errs() << "#4.5\n");
										}
									}
								}
							}

							else if (PtrToIntOperator *PTIO = dyn_cast<PtrToIntOperator>(V)) {
								// DEBUG(errs() << "#5.0\n");
								Value *v_ptr = PTIO->getPointerOperand();
								// DEBUG(errs() << "#5.1\n");
								if (GlobalVariable *GV = dyn_cast<GlobalVariable>(v_ptr)) {
									// DEBUG(errs() << "#5.2\n");
									if (GV->hasName() && !(GV->getName().endswith("_ptr"))) {
										// DEBUG(errs() << "#5.3\n");
										LoadInst *LI = find_and_replace(I, GV, GVPtrMap);
										// DEBUG(errs() << "#5.4\n");
										if (LI) {
											// DEBUG(errs() << "#5.5\n");
											PtrToIntInst *PTII = new PtrToIntInst(LI, PTIO->getType(), GV->getName()+"_ptrtoint", &I);
											I.setOperand(i, PTII);
											// DEBUG(errs() << "#5.6\n");
										}
									}
								}
							}

						#if 0
						if (CallInst *CI = dyn_cast<CallInst>(&I)) {
							Function *callee_func;
							if ((callee_func=CI->getCalledFunction()) || (callee_func=dyn_cast<Function>(CI->getCalledValue()->stripPointerCasts()))) {
								if (!isIntrinsics(*callee_func)) {
									for (unsigned i=0; i < CI->getNumArgOperands(); i++) {
										if (GlobalVariable *GV = dyn_cast<GlobalVariable>(CI->getArgOperand(i))) {
											if (GV->hasName()) {
												DEBUG(errs() << "[CalledArg Type 0]: " << F.getName() << "=>"  << callee_func->getName() << " " << i << " " << GV->getName() << "\n");
												LoadInst *LI = find_and_replace(I, GV, GVPtrMap);
												if(LI)
													I.setOperand(i, LI);
											}
										}
										else if (GEPOperator* gepo = dyn_cast<GEPOperator>(CI->getArgOperand(i))) {
											if (GlobalVariable *GV = dyn_cast_or_null<GlobalVariable>(gepo->getPointerOperand())) {
												DEBUG(errs() << "[CalledArg Type 1]: " << F.getName() << "=>"  << callee_func->getName() << " " << i << " " << GV->getName() << "\n");
												GetElementPtrInst *GI = gepo_find_and_replace(I, gepo, GV, GVPtrMap);
												if(GI)
													I.setOperand(i, GI);
											}
										}
										else if (GlobalVariable *GV = dyn_cast<GlobalVariable>(CI->getOperand(i)->stripPointerCasts())) {
												DEBUG(errs() << "[CalledArg Type 2]: " << F.getName() << "=>"  << callee_func->getName() << " " << i << " " << GV->getName() << "\n");
												LoadInst *LI = find_and_replace(I, GV, GVPtrMap);
												if(LI) {
													if (BitCastOperator *BCO = dyn_cast<BitCastOperator>(CI->getOperand(i))) {
														BitCastInst *BCI = new BitCastInst(LI, BCO->getDestTy(), GV->getName()+"_bitcast", &I);
														if(BCI)
															I.setOperand(i, BCI);
													}
												}
										}
									}
								}
							}
						}
						#endif
						#if 0
						else if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
							/* 全局变量取址用的store指令, opcode0是指针类型 */
							/* 替换为对GV_ptr的先load后store，保证在operation内的取址操作是对shadow的取址操作 */
							Value *V = I.getOperand(0);
							if (GlobalVariable *GV = dyn_cast<GlobalVariable>(V)) {
								/* Case 1: Directly store value to a GV */
								if (GV->hasName()) {
									DEBUG(errs() << "[StoreInst Operand 0 Type 0]: " << F.getName() << " " << GV->getName() << "\n");
									LoadInst *LI = find_and_replace(I, GV, GVPtrMap);
									if(LI) {
										DEBUG(errs() << "[StoreInst Operand 0 Type 0]: Replace\n");
										I.setOperand(0, LI);
									}
								}
							}
							else if (GlobalVariable *GV = dyn_cast<GlobalVariable>(SI->getOperand(0)->stripPointerCasts())) {
								if (GV->hasName() && !(GV->getName().endswith("_ptr"))) {
									LoadInst *LI = find_and_replace(I, GV, GVPtrMap);
									if (LI) {
										/* Case 2: first bitcast GV, second store value to the bitcasted GV */
										if (BitCastOperator *BCO = dyn_cast<BitCastOperator>(SI->getOperand(0))) {
											BitCastInst *BCI = new BitCastInst(LI, BCO->getDestTy(), GV->getName()+"_bitcast", &I);
											DEBUG(errs() << "[StoreInst Operand 0 Type 1]: " << F.getName() << " " << GV->getName() << "\n");
											DEBUG(errs() << "[StoreInst Operand 0 Type 1]: Replace\n");
											I.setOperand(0, BCI);

										} else {
											/* Case 2: first cast GV, second store value to the casted GV */
											DEBUG(errs() << "[StoreInst Operand 0 Type 2]: " << F.getName() << " " << GV->getName() << "\n");
											DEBUG(errs() << "[StoreInst Operand 0 Type 2]: Replace\n");
											I.setOperand(0, LI);
										}
									}
								}
							}
							else if (PtrToIntOperator *PTIO = dyn_cast<PtrToIntOperator>(SI->getOperand(0))) {
								Value *v_ptr = PTIO->getPointerOperand();
								if (GlobalVariable *GV = dyn_cast<GlobalVariable>(v_ptr)) {
									if (GV->hasName() && !(GV->getName().endswith("_ptr"))) {
										LoadInst *LI = find_and_replace(I, GV, GVPtrMap);
										if (LI) {
											PtrToIntInst *PTII = new PtrToIntInst(LI, PTIO->getType(), GV->getName()+"_ptrtoint", &I);
											DEBUG(errs() << "[StoreInst Operand 0 Type 3]: " << F.getName() << " " << GV->getName() << "\n");
											DEBUG(errs() << "[StoreInst Operand 0 Type 3]: Replace\n");
											I.setOperand(0, PTII);
										}
									}
								}
							}
						}
						#endif
						}
					}
				}
			}
			return;
		}
	};
}

char OIApplication::ID = 0;
INITIALIZE_PASS(OIApplication, "OIApplication", "Applies specified operation isolation policy", false, false)

ModulePass *llvm::createOIApplicationPass() {
	DEBUG(errs() << "OPEC Application Pass" <<"\n");
	return new OIApplication();
}