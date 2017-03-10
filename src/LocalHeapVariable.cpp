//
//  LocalHeapVariable.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "yazyk/AutoCast.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LocalHeapVariable.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/Scopes.hpp"

using namespace std;
using namespace llvm;
using namespace yazyk;

string LocalHeapVariable::getName() const {
  return mName;
}

IType* LocalHeapVariable::getType() const {
  return mType;
}

Value* LocalHeapVariable::getValue() const {
  return mValue;
}

Value* LocalHeapVariable::generateIdentifierIR(IRGenerationContext& context,
                                                std::string llvmVariableName) const {
  if (mValue == NULL) {
    Log::e("Variable '" + mName + "' is used before it has been initialized.");
    exit(1);
  }
  return mValue;
}

Value* LocalHeapVariable::generateAssignmentIR(IRGenerationContext& context,
                                               IExpression& assignToExpression) {
  Value* assignToValue = assignToExpression.generateIR(context);
  IType* assignToType = assignToExpression.getType(context);
  Value* castAssignToValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType);
  mValue = new BitCastInst(castAssignToValue,
                           castAssignToValue->getType(),
                           mName,
                           context.getBasicBlock());
  context.getScopes().clearVariable(assignToValue->getName());
  
  return mValue;
}

void LocalHeapVariable::free(IRGenerationContext& context) const {
  if (mValue == NULL) {
    return;
  }
  
  BasicBlock* basicBlock = context.getBasicBlock();
  basicBlock->getInstList().push_back(CallInst::CreateFree(mValue, basicBlock));
}
