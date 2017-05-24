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
#include "wisey/Log.hpp"
#include "wisey/ReturnStatement.hpp"

#include <llvm/IR/Instructions.h>

using namespace llvm;
using namespace wisey;

void ReturnStatement::prototype(IRGenerationContext& context) const {
}

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

