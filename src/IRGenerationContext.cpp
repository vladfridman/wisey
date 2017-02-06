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

void IRGenerationContext::addModel(Model* model) {
  string name = model->getName();
  if (mModels.count(name)) {
    Log::e("Redefinition of MODEL " + name);
    exit(1);
  }
  
  mModels[name] = model;
}

Model* IRGenerationContext::getModel(string name) {
  if (!mModels.count(name)) {
    Log::e("MODEL " + name + " is not defined");
    exit(1);
  }

  return mModels.at(name);
}

Scopes& IRGenerationContext::getScopes() {
  return mScopes;
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
  passManager.add(createDeadInstEliminationPass());
  
  passManager.run(*mModule);
}

IRGenerationContext::~IRGenerationContext() {
  for(map<string, Model*>::iterator iterator = mModels.begin();
      iterator != mModels.end();
      iterator++) {
    Model* model = iterator->second;
    delete model;
  }
}
