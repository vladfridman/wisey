//
//  Optimizer.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/23/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/TargetParser/Triple.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>

#include "IRGenerationContext.hpp"
#include "Optimizer.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

void Optimizer::optimize(IRGenerationContext& context) {
  PassBuilder passBuilder;
  LoopAnalysisManager loopAnalysisManager;
  FunctionAnalysisManager functionAnalysisManager;
  CGSCCAnalysisManager cgsccAnalysisManager;
  ModuleAnalysisManager moduleAnalysisManager;

  passBuilder.registerModuleAnalyses(moduleAnalysisManager);
  passBuilder.registerCGSCCAnalyses(cgsccAnalysisManager);
  passBuilder.registerFunctionAnalyses(functionAnalysisManager);
  passBuilder.registerLoopAnalyses(loopAnalysisManager);
  passBuilder.crossRegisterProxies(loopAnalysisManager,
                                   functionAnalysisManager,
                                   cgsccAnalysisManager,
                                   moduleAnalysisManager);

  ModulePassManager modulePassManager =
    passBuilder.buildPerModuleDefaultPipeline(OptimizationLevel::O3);
  modulePassManager.run(*context.getModule(), moduleAnalysisManager);
}
