//
//  Scope.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/Scope.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

IVariable* Scope::findVariable(string name) {
  if (mLocals.count(name)) {
    return mLocals.at(name);
  }
  
  return NULL;
}

void Scope::setVariable(string name, IVariable* variable) {
  mLocals[name] = variable;
}

void Scope::clearVariable(string name) {
  if (!mLocals.count(name)) {
    Log::e("Variable '" + name + "' is not set in this scope.");
    exit(1);
  }
  mLocals.erase(name);
}

void Scope::setBreakToBlock(BasicBlock* block) {
  mBreakToBlock = block;
}

BasicBlock* Scope::getBreakToBlock() {
  return mBreakToBlock;
}

void Scope::setContinueToBlock(BasicBlock* block) {
  mContinueToBlock = block;
}

BasicBlock* Scope::getContinueToBlock() {
  return mContinueToBlock;
}

void Scope::setLandingPadBlock(BasicBlock* block) {
  mLandingPadBlock = block;
}

BasicBlock* Scope::getLandingPadBlock() {
  return mLandingPadBlock;
}

void Scope::setExceptionContinueBlock(BasicBlock* block) {
  mExceptionContinueBlock = block;
}

BasicBlock* Scope::getExceptionContinueBlock() {
  return mExceptionContinueBlock;
}

void Scope::setReturnType(IType* type) {
  mReturnType = type;
}

IType* Scope::getReturnType() {
  return mReturnType;
}

void Scope::maybeFreeOwnedMemory(IRGenerationContext& context) {
  if (mHasOwnedMemoryBeenFreed) {
    return;
  }
  
  BasicBlock* currentBlock = context.getBasicBlock();
  if(currentBlock != NULL &&
     currentBlock->size() > 0 &&
     UnreachableInst::classof(&currentBlock->back())) {
    return;
  }
  
  for (map<string, IVariable*>::iterator iterator = mLocals.begin();
       iterator != mLocals.end();
       iterator++) {
    string name = iterator->first;
    IVariable* variable = iterator->second;
    variable->free(context);
    
    delete variable;
  }
  
  mHasOwnedMemoryBeenFreed = true;
}

void Scope::addException(IType* exception) {
  if (mExceptions.count(exception->getName())) {
    return;
  }
  mExceptions[exception->getName()] = exception;
}

void Scope::addExceptions(vector<IType*> exceptions) {
  for (IType* exception : exceptions) {
    addException(exception);
  }
}

void Scope::removeException(IType* exception) {
  mExceptions.erase(exception->getName());
}

map<string, IType*> Scope::getExceptions() {
  return mExceptions;
}
