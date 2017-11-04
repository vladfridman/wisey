//
//  ParameterPrimitiveVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/Log.hpp"
#include "wisey/ParameterPrimitiveVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterPrimitiveVariable::ParameterPrimitiveVariable(string name,
                                                       const IPrimitiveType* type,
                                                       Value* value) :
mName(name), mType(type), mValue(value) { }

ParameterPrimitiveVariable::~ParameterPrimitiveVariable() {
}

string ParameterPrimitiveVariable::getName() const {
  return mName;
}

const IPrimitiveType* ParameterPrimitiveVariable::getType() const {
  return mType;
}

Value* ParameterPrimitiveVariable::generateIdentifierIR(IRGenerationContext& context,
                                                        string llvmVariableName) const {
  return mValue;
}

Value* ParameterPrimitiveVariable::generateAssignmentIR(IRGenerationContext& context,
                                                        IExpression* assignToExpression) {
  Log::e("Assignment to method parameters is not allowed");
  exit(1);
}

void ParameterPrimitiveVariable::setToNull(IRGenerationContext& context) {
  return;
}

void ParameterPrimitiveVariable::free(IRGenerationContext& context) const {
  return;
}

bool ParameterPrimitiveVariable::existsInOuterScope() const {
  return true;
}

