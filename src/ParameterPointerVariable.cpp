//
//  ParameterPointerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "IRGenerationContext.hpp"
#include "Log.hpp"
#include "ParameterPointerVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterPointerVariable::ParameterPointerVariable(string name,
                                                   const LLVMPointerType* type,
                                                   Value* value,
                                                   int line) :
mName(name), mType(type), mValue(value), mLine(line) {
}

ParameterPointerVariable::~ParameterPointerVariable() {
}

string ParameterPointerVariable::getName() const {
  return mName;
}

const LLVMPointerType* ParameterPointerVariable::getType() const {
  return mType;
}

bool ParameterPointerVariable::isField() const {
  return false;
}

bool ParameterPointerVariable::isStatic() const {
  return false;
}

int ParameterPointerVariable::getLine() const {
  return mLine;
}

Value* ParameterPointerVariable::generateIdentifierIR(IRGenerationContext& context,
                                                      int line) const {
  return mValue;
}

Value* ParameterPointerVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                               int line) const {
  assert(false);
}

Value* ParameterPointerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                     IExpression* assignToExpression,
                                                     vector<const IExpression*> arrayIndices,
                                                     int line) {
  context.reportError(line, "Assignment to method parameters is not allowed");
  throw 1;
}
