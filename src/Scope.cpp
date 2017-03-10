//
//  Scope.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

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
