//
//  ReturnStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/AutoCast.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/IRWriter.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/ReturnStatement.hpp"

#include <llvm/IR/Instructions.h>

using namespace llvm;
using namespace yazyk;

Value* ReturnStatement::generateIR(IRGenerationContext& context) const {
  IType* returnType = context.getScopes().getReturnType();
  if (returnType == NULL) {
    Log::e("No corresponding method found for RETURN");
    exit(1);
  }
  
  Value* returnValue = AutoCast::maybeCast(context,
                                           mExpression.getType(context),
                                           mExpression.generateIR(context),
                                           returnType);
  context.getScopes().getScope()->maybeFreeOwnedMemory(context);
  
  return IRWriter::createReturnInst(context, returnValue);
}

