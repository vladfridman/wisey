//
//  ReturnStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/log.hpp"
#include "yazyk/ReturnStatement.hpp"

#include <llvm/IR/Instructions.h>

using namespace llvm;
using namespace yazyk;

Value* ReturnStatement::generateIR(IRGenerationContext& context) const {
  Value* returnValue = expression.generateIR(context);
  Type* valueType = returnValue->getType();
  Function *parentFunction = context.getBasicBlock()->getParent();
  
  context.getScope()->maybeFreeOwnedMemory(context.getBasicBlock());
  
  if (parentFunction == NULL) {
    Log::e("No corresponding method found for RETURN");
    exit(1);
  }
  
  Type * returnType = parentFunction->getReturnType();
  
  if (returnType != valueType &&
      !CastInst::isCastable(valueType, returnType)) {
    Log::e("Can not cast return value to function type");
    exit(1);
  }
  
  if (returnType != valueType) {
    returnValue = CastInst::CreateZExtOrBitCast(returnValue,
                                                returnType,
                                                "conv",
                                                context.getBasicBlock());
  }
  
  ReturnInst* result = ReturnInst::Create(context.getLLVMContext(),
                                          returnValue,
                                          context.getBasicBlock());
  return result;
}

