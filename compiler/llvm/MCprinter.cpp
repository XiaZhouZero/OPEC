//===-- MCExperimentPrinterPass.cpp - Insert Thumb-2 IT blocks ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARM.h"
#include "ARMMachineFunctionInfo.h"
#include "Thumb2InstrInfo.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineInstrBundle.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/json/json.h"
#include <fstream>
#include <iostream>
using namespace llvm;

#define DEBUG_TYPE "MCprinter"

static cl::opt<std::string> OIMCprinter("OIMCprinter", cl::desc("modify MachineInstr"), cl::init("-"), cl::value_desc("filename"));

namespace {
    class OIMCprinterPass : public MachineFunctionPass {
    public:
        static char ID;
        OIMCprinterPass() : MachineFunctionPass(ID) {}

        bool runOnMachineFunction(MachineFunction &Fn) override{
            const ARMBaseInstrInfo &TII = *static_cast<const ARMBaseInstrInfo *>(Fn.getSubtarget().getInstrInfo());
            MachineInstrBuilder MIB;
            MachineBasicBlock::iterator I;
            const Function *func;
#if 0
            if(OIMCprinter.compare("-") == 0){
                return false;
            }
#endif
            for (MachineBasicBlock &BB :Fn) {                
                for(MachineInstr &MI: BB) {
                    if(MI.isCall()) {
                        //MI.print(errs());
                        /*遍历查找call target，这里只能找到direct call，我们必须保证Operation的entry function是通过direct call的形式*/
                        for(auto MO = MI.operands_begin(); MO != MI.operands_end(); MO++) {
                            if(MO->isGlobal()) {
                                func = dyn_cast_or_null<Function>(MO->getGlobal());
                                assert(func && "change globalvalue to function fail!");
                                if(func->hasFnAttribute("OperationEntry")) {
                                    auto DbgLoc = MI.getDebugLoc();
                                    /*在函数调用之前位置插入SVC指令和Metadata的地址来进入operation*/
                                    auto  I = MI.getIterator();
                                    std::string *cmd = new std::string(".long __operation_");
                                    cmd->append(func->getFnAttribute("OperationEntry").getValueAsString());
                                    
                                    MIB = BuildMI(BB,I,DbgLoc,TII.get(ARM::tSVC));
                                    MIB.addImm(100);
                                    AddDefaultPred(MIB);

                                    MIB = BuildMI(BB,I,DbgLoc,TII.get(ARM::INLINEASM));
                                    MIB.addOperand(MachineOperand::CreateES(cmd->c_str()));
                                    MIB.addOperand(MachineOperand::CreateImm(InlineAsm::AD_ATT));

                                    MIB = BuildMI(BB,I,DbgLoc,TII.get(ARM::INLINEASM));
                                    std::string *cmd2 = new std::string(".long Policies_Info");
                                    MIB.addOperand(MachineOperand::CreateES(cmd2->c_str()));
                                    MIB.addOperand(MachineOperand::CreateImm(InlineAsm::AD_ATT));
                                    /*在函数调用之后插入SVC指令来退出Operation*/
                                    I = std::next(I);
                                    MIB = BuildMI(BB,I,DbgLoc,TII.get(ARM::tSVC));
                                    MIB.addImm(101);
                                    AddDefaultPred(MIB);
                                }
                            }
                        }
                    }
                }
            }
#if 1
            if(Fn.getName() == "__original_main") {
                for (MachineBasicBlock &BB :Fn) {
                    for(MachineInstr &MI: BB) {
                        MI.print(errs());
                    }
                }
            }
#endif
            return true;
        }

        bool doInitialization(Module &) override{
            if ( OIMCprinter.compare("-") == 0 ){
                return false;
            }
            DEBUG(errs() <<"\n\n\nStarting Machine Instruction Printer\n\n");
            return false;
        }

        bool doFinalization(Module &) override{
            if ( OIMCprinter.compare("-") == 0 ){
                return false;
            }
            return false;
        }

        StringRef getPassName() const override {
            return StringRef("Printer Pass For MachineInstr");
        }
    };
    char OIMCprinterPass::ID = 0;
}

FunctionPass *llvm::createOIMCprinterPass() {
    return new OIMCprinterPass();
}