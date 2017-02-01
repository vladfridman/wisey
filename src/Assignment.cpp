//
//  Assignment.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "yazyk/Assignment.hpp"
#include "yazyk/log.hpp"
#include "yazyk/Identifier.hpp"

using namespace llvm;
using namespace yazyk;

Value* Assignment::generateIR(IRGenerationContext& context) const {  Scopes& scopes = context.getScopes();

  Variable* variable = scopes.getVariable(mIdentifier.getName());
  if (variable == NULL) {
    Log::e("undeclared variable " + mIdentifier.getName());
    exit(1);
  }
  
  Value* expressionValue = mExpression.generateIR(context);
  
  Value* result;
  
  if (variable->getStorageType() == HEAP_VARIABLE_UNINITIALIZED) {
    result = new BitCastInst(expressionValue,
                             expressionValue->getType(),
                             mIdentifier.getName(),
                             context.getBasicBlock());

    scopes.clearVariable(mIdentifier.getName());
    scopes.clearVariable(expressionValue->getName());
    scopes.setHeapVariable(mIdentifier.getName(), result);
  } else {
    result = new StoreInst(expressionValue,
                           variable->getValue(),
                           context.getBasicBlock());
  }
  
  return result;
}
