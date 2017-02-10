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

map<string, IVariable*>& Scope::getLocals() {
  return mLocals;
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

void Scope::maybeFreeOwnedMemory(BasicBlock* block) {
  if (mHasOwnedMemoryBeenFreed) {
    return;
  }
  
  for (map<string, IVariable*>::iterator iterator = mLocals.begin();
      iterator != mLocals.end();
      iterator++) {
    string name = iterator->first;
    IVariable* variable = iterator->second;
    variable->free(block);

    delete variable;
  }
  
  mHasOwnedMemoryBeenFreed = true;
}
