//
//  StackVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/StackVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

string StackVariable::getName() const {
  return mName;
}

const IType* StackVariable::getType() const {
  return mType;
}

Value* StackVariable::getValue() const {
  return mValue;
}

Value* StackVariable::generateIdentifierIR(IRGenerationContext& context,
                                                std::string llvmVariableName) const {
  return IRWriter::newLoadInst(context, mValue, llvmVariableName);
}

Value* StackVariable::generateAssignmentIR(IRGenerationContext& context,
                                                IExpression* assignToExpression) {
  Value* assignToValue = assignToExpression->generateIR(context);
  const IType* assignToType = assignToExpression->getType(context);
  Value* castAssignToValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType);
  return IRWriter::newStoreInst(context, castAssignToValue, mValue);
}

void StackVariable::free(IRGenerationContext& context) const {
}

bool StackVariable::existsInOuterScope() const {
  return false;
}
