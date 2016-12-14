//
//  IRGenerationContext.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/log.hpp"
#include "yazyk/node.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

/* Compile the AST into a module */
void IRGenerationContext::generateIR(Block& root) {
  root.generateIR(*this);
  
  verifyModule(*mModule);
  
  legacy::PassManager passManager;
  
  // Optimization: Constant Propagation transform
  // passManager.add(createConstantPropagationPass());
  // Optimization: Dead Instruction Elimination transform
  // passManager.add(createDeadInstEliminationPass());
  
  // print out assembly code
  if (Log::isDebugLevel()) {
    passManager.add(createPrintModulePass(outs()));
  }
  
  passManager.run(*mModule);
}

/* Executes the AST by running the main function */
GenericValue IRGenerationContext::runCode() {
  ExecutionEngine *executionEngine = EngineBuilder(move(mOwner)).create();
  vector<GenericValue> noargs;
  if (mMainFunction == NULL) {
    Log::e("Function main() is not defined. Exiting.");
    delete executionEngine;
    exit(1);
  }
  Log::i("Running program:");
  GenericValue result = executionEngine->runFunction(mMainFunction, noargs);
  Log::i("Result: " + result.IntVal.toString(10, true));
  delete executionEngine;
  
  return result;
}
