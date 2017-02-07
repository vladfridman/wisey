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

Value* Assignment::generateIR(IRGenerationContext& context) const {

  Variable* variable = context.getScopes().getVariable(mIdentifier.getName());
  if (variable == NULL) {
    Log::e("undeclared variable " + mIdentifier.getName());
    exit(1);
  }
  
  return variable->getStorageType() == HEAP_VARIABLE_UNINITIALIZED
    ? generateIRForHeapVariable(context)
    : generateIRForStackVariable(context);
}

Value* Assignment::generateIRForHeapVariable(IRGenerationContext& context) const {
  Scopes& scopes = context.getScopes();
  Value* expressionValue = mExpression.generateIR(context);

  Variable* variable = scopes.getVariable(mIdentifier.getName());
  IType* variableType = variable->getType();
  
  BitCastInst* bitcast = new BitCastInst(expressionValue,
                                         expressionValue->getType(),
                                         mIdentifier.getName(),
                                         context.getBasicBlock());
  
  scopes.clearVariable(mIdentifier.getName());
  scopes.clearVariable(expressionValue->getName());
  scopes.setHeapVariable(mIdentifier.getName(), variableType, bitcast);

  return bitcast;
}

Value* Assignment::generateIRForStackVariable(IRGenerationContext& context) const {
  return new StoreInst(mExpression.generateIR(context),
                       context.getScopes().getVariable(mIdentifier.getName())->getValue(),
                       context.getBasicBlock());
}

IType* Assignment::getType(IRGenerationContext& context) const {
  return mIdentifier.getType(context);
}
