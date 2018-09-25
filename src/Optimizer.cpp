//
//  Optimizer.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/23/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/ADT/Triple.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>

#include "IRGenerationContext.hpp"
#include "Optimizer.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

void Optimizer::optimize(IRGenerationContext& context) {
  legacy::PassManager passManager;
  legacy::FunctionPassManager functionPassManager(context.getModule());
  
  PassManagerBuilder Builder;
  Builder.OptLevel = 3u;
  Builder.SizeLevel = 0;

  Builder.populateFunctionPassManager(functionPassManager);
  Builder.populateModulePassManager(passManager);

  functionPassManager.doInitialization();
  for (Function& function : *context.getModule()) {
    functionPassManager.run(function);
  }
  functionPassManager.doFinalization();

  passManager.run(*context.getModule());
}
