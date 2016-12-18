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

using namespace llvm;
using namespace std;
using namespace yazyk;

const string& Identifier::getName() const {
  return mName;
}

Value* Identifier::generateIR(IRGenerationContext& context) {
  if (context.locals().find(mName) == context.locals().end()) {
    cerr << "undeclared variable " << mName << endl;
    return NULL;
  }
  return new LoadInst(context.locals()[mName], mVariableName, context.currentBlock());
}
