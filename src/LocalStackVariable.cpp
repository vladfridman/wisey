//
//  LocalStackVariable.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LocalStackVariable.hpp"

using namespace std;
using namespace llvm;
using namespace yazyk;

string LocalStackVariable::getName() const {
  return mName;
}

IType* LocalStackVariable::getType() const {
  return mType;
}

Value* LocalStackVariable::getValue() const {
  return mValue;
}

Value* LocalStackVariable::generateIdentifierIR(IRGenerationContext& context,
                                                std::string llvmVariableName) const {
  return new LoadInst(mValue, llvmVariableName, context.getBasicBlock());
}

Value* LocalStackVariable::generateAssignmentIR(IRGenerationContext& context,
                                                IExpression& assignToExpression) {
  return new StoreInst(assignToExpression.generateIR(context),
                       mValue,
                       context.getBasicBlock());
}

void LocalStackVariable::free(BasicBlock* basicBlock) const {
}