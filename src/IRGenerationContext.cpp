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
#include <llvm/Transforms/Scalar.h>

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/log.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

GenericValue IRGenerationContext::runCode() {
  ExecutionEngine *executionEngine = EngineBuilder(move(mOwner)).create();
  vector<GenericValue> noargs;
  if (mMainFunction == NULL) {
    Log::e("Function main is not defined. Exiting.");
    delete executionEngine;
    exit(1);
  }
  Log::i("Running program:");
  GenericValue result = executionEngine->runFunction(mMainFunction, noargs);
  Log::i("Result: " + result.IntVal.toString(10, true));
  delete executionEngine;
  
  return result;
}

Module* IRGenerationContext::getModule() {
  return mModule;
}

Value* IRGenerationContext::getVariable(string name) {
  if (mScopes.size() == 0) {
    return NULL;
  }

  std::vector<Scope *>::iterator iterator = mScopes.end();
  do {
    --iterator;
    Value* value = (*iterator)->getLocals()[name];
    if (value != NULL) {
      return value;
    }
  } while (iterator != mScopes.begin());

  return NULL;
}

void IRGenerationContext::setVariable(string name, Value* value) {
  if (mScopes.size() == 0) {
    Log::e("Can not set variable - scope was not created.");
    exit(1);
  }
  mScopes.back()->getLocals()[name] = value;
}

void IRGenerationContext::setMainFunction(llvm::Function* function) {
  mMainFunction = function;
}

Function* IRGenerationContext::getMainFunction() {
  return mMainFunction;
}

BasicBlock* IRGenerationContext::getBasicBlock() {
  return mBasicBlock;
}

void IRGenerationContext::setBasicBlock(BasicBlock* block) {
  mBasicBlock = block;
}

void IRGenerationContext::pushScope() {
  mScopes.push_back(new Scope());
}

void IRGenerationContext::popScope() {
  Scope* top = mScopes.back();
  mScopes.pop_back();
  delete top;
}

void IRGenerationContext::setBreakToBlock(BasicBlock* block) {
  mScopes.back()->setBreakToBlock(block);
}

BasicBlock* IRGenerationContext::getBreakToBlock() {
  if (mScopes.size() == 0) {
    return NULL;
  }
  
  std::vector<Scope *>::iterator iterator = mScopes.end();
  do {
    --iterator;
    BasicBlock* block = (*iterator)->getBreakToBlock();
    if (block != NULL) {
      return block;
    }
  } while (iterator != mScopes.begin());
  return NULL;
}

LLVMContext& IRGenerationContext::getLLVMContext() {
  return mLLVMContext;
}

void IRGenerationContext::printAssembly(raw_ostream &outputStream) {
  legacy::PassManager passManager;
  passManager.add(createPrintModulePass(outputStream));
  passManager.run(*mModule);
}

void IRGenerationContext::optimizeIR() {
  legacy::PassManager passManager;
  
  // Optimization: Constant Propagation transform
  passManager.add(createConstantPropagationPass());
  // Optimization: Dead Instruction Elimination transform
  passManager.add(createDeadInstEliminationPass());\
  
  passManager.run(*mModule);
}

