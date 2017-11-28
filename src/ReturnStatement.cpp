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
  
  IRGenerationFlag irGenerationFlag = IType::isOwnerType(returnType)
  ? IR_GENERATION_RELEASE : IR_GENERATION_NORMAL;
  
  Value* result = AutoCast::maybeCast(context,
                                      mExpression->getType(context),
                                      mExpression->generateIR(context, irGenerationFlag),
                                      returnType);
  if (IType::isReferenceType(returnType)) {
    ((IObjectType*) returnType)->incremenetReferenceCount(context, result);
  }
  
  context.getScopes().freeOwnedMemory(context, mLine);
  
  if (IType::isReferenceType(returnType)) {
    ((IObjectType*) returnType)->decremenetReferenceCount(context, result);
  }
  
  return IRWriter::createReturnInst(context, result);
}

