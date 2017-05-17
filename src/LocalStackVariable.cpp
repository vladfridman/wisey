//
//  LocalStackVariable.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalStackVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

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
  return IRWriter::newLoadInst(context, mValue, llvmVariableName);
}

Value* LocalStackVariable::generateAssignmentIR(IRGenerationContext& context,
                                                IExpression& assignToExpression) {
  Value* assignToValue = assignToExpression.generateIR(context);
  IType* assignToType = assignToExpression.getType(context);
  Value* castAssignToValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType);
  return IRWriter::newStoreInst(context, castAssignToValue, mValue);
}

void LocalStackVariable::free(IRGenerationContext& context) const {
}
