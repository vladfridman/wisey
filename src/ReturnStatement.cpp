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

ReturnStatement::~ReturnStatement() {
  delete mExpression;
}

Value* ReturnStatement::generateIR(IRGenerationContext& context) const {
  const IType* returnType = context.getScopes().getReturnType();
  if (returnType == NULL) {
    Log::e("No corresponding method found for RETURN");
    exit(1);
  }
  
  Value* returnValue = AutoCast::maybeCast(context,
                                           mExpression->getType(context),
                                           mExpression->generateIR(context),
                                           returnType);
  
  TypeKind returnTypeKind = returnType->getTypeKind();
  if (returnTypeKind == MODEL_OWNER_TYPE ||
      returnTypeKind == INTERFACE_OWNER_TYPE ||
      returnTypeKind == CONTROLLER_OWNER_TYPE) {
    mExpression->releaseOwnership(context);
  }
  
  context.getScopes().freeOwnedMemory(context);
  
  return IRWriter::createReturnInst(context, returnValue);
}

