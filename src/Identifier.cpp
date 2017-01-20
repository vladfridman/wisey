//
//  Identifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include "llvm/IR/TypeBuilder.h"

#include "yazyk/Identifier.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/log.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

const string& Identifier::getName() const {
  return mName;
}

Value* Identifier::generateIR(IRGenerationContext& context) const {
  Value* value = context.getScopes().getVariable(mName);
  if (value == NULL) {
    Log::e("Undeclared variable " + mName);
    exit(1);
  }
  return new LoadInst(value, mVariableName, context.getBasicBlock());
}
