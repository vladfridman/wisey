//
//  ReturnStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/AutoCast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/ReturnStatement.hpp"

#include <llvm/IR/Instructions.h>

using namespace llvm;
using namespace wisey;

ReturnStatement::ReturnStatement(IExpression* expression, int line) :
mExpression(expression), mLine(line) { }

ReturnStatement::~ReturnStatement() {
  delete mExpression;
}

Value* ReturnStatement::generateIR(IRGenerationContext& context) const {
  const IType* returnType = context.getScopes().getReturnType();
  if (returnType == NULL) {
    Log::e("No corresponding method found for RETURN");
    exit(1);
  }
  
  Value* result = AutoCast::maybeCast(context,
                                      mExpression->getType(context),
                                      mExpression->generateIR(context, returnType),
                                      returnType,
                                      mLine);
  if (returnType->isReference() && !returnType->isNative()) {
    ((IReferenceType*) returnType)->incrementReferenceCount(context, result);
  }
  
  context.getScopes().freeOwnedMemory(context, mLine);
  
  if (returnType->isReference() && !returnType->isNative()) {
    ((IReferenceType*) returnType)->decrementReferenceCount(context, result);
  }
  
  return IRWriter::createReturnInst(context, result);
}

