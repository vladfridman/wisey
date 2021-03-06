//
//  ParameterPrimitiveVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "IRGenerationContext.hpp"
#include "Log.hpp"
#include "ParameterPrimitiveVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterPrimitiveVariable::ParameterPrimitiveVariable(string name,
                                                       const IPrimitiveType* type,
                                                       Value* value,
                                                       int line) :
mName(name), mType(type), mValue(value), mLine(line) { }

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

bool ParameterPrimitiveVariable::isStatic() const {
  return false;
}

int ParameterPrimitiveVariable::getLine() const {
  return mLine;
}

Value* ParameterPrimitiveVariable::generateIdentifierIR(IRGenerationContext& context,
                                                        int line) const {
  return mValue;
}

Value* ParameterPrimitiveVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                                 int line) const {
  assert(false);
}

Value* ParameterPrimitiveVariable::generateAssignmentIR(IRGenerationContext& context,
                                                        IExpression* assignToExpression,
                                                        vector<const IExpression*> arrayIndices,
                                                        int line) {
  context.reportError(line, "Assignment to method parameters is not allowed");
  throw 1;
}

