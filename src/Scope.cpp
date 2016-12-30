//
//  Scope.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "yazyk/Scope.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

map<string, Value*>& Scope::getLocals() {
  return mLocals;
}

void Scope::setBreakToBlock(BasicBlock* block) {
  mBreakToBlock = block;
}

BasicBlock* Scope::getBreakToBlock() {
  return mBreakToBlock;
}
