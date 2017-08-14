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
                                      mExpression->generateIR(context),
                                      returnType);
  if (IType::isOwnerType(returnType)) {
    result = IRWriter::newLoadInst(context, result, "");
    mExpression->releaseOwnership(context);
  } else if (returnType->getTypeKind() != PRIMITIVE_TYPE &&
             !mExpression->existsInOuterScope(context)) {
    Log::e("Returning a reference to a value that does not exist in caller's scope.");
    exit(1);
  }
  
  context.getScopes().freeOwnedMemory(context);
  
  return IRWriter::createReturnInst(context, result);
}

