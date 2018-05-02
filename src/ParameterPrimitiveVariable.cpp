//
//  ParameterPrimitiveVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
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

bool ParameterPrimitiveVariable::isField() const {
  return false;
}

const IPrimitiveType* ParameterPrimitiveVariable::getType() const {
  return mType;
}

bool ParameterPrimitiveVariable::isSystem() const {
  return false;
}

Value* ParameterPrimitiveVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return mValue;
}

Value* ParameterPrimitiveVariable::generateIdentifierReferenceIR(IRGenerationContext&
                                                                 context) const {
  assert(false);
}

Value* ParameterPrimitiveVariable::generateAssignmentIR(IRGenerationContext& context,
                                                        IExpression* assignToExpression,
                                                        vector<const IExpression*> arrayIndices,
                                                        int line) {
  context.reportError(line, "Assignment to method parameters is not allowed");
  exit(1);
}

