//
//  ParameterLLVMVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "IRGenerationContext.hpp"
#include "Log.hpp"
#include "ParameterLLVMVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterLLVMVariable::ParameterLLVMVariable(string name,
                                             const ILLVMType* type,
                                             Value* value,
                                             int line) :
mName(name), mType(type), mValue(value), mLine(line) { }

ParameterLLVMVariable::~ParameterLLVMVariable() {
}

string ParameterLLVMVariable::getName() const {
  return mName;
}

bool ParameterLLVMVariable::isField() const {
  return false;
}

const ILLVMType* ParameterLLVMVariable::getType() const {
  return mType;
}

bool ParameterLLVMVariable::isStatic() const {
  return false;
}

int ParameterLLVMVariable::getLine() const {
  return mLine;
}

Value* ParameterLLVMVariable::generateIdentifierIR(IRGenerationContext& context, int line) const {
  return mValue;
}

Value* ParameterLLVMVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                            int line) const {
  assert(false);
}

Value* ParameterLLVMVariable::generateAssignmentIR(IRGenerationContext& context,
                                                   IExpression* assignToExpression,
                                                   vector<const IExpression*> arrayIndices,
                                                   int line) {
  context.reportError(line, "Assignment to method parameters is not allowed");
  throw 1;
}
