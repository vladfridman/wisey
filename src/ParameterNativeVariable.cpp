//
//  ParameterNativeVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/Log.hpp"
#include "wisey/ParameterNativeVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterNativeVariable::ParameterNativeVariable(string name, const ILLVMType* type, Value* value) :
mName(name), mType(type), mValue(value) {
}

ParameterNativeVariable::~ParameterNativeVariable() {
}

string ParameterNativeVariable::getName() const {
  return mName;
}

const ILLVMType* ParameterNativeVariable::getType() const {
  return mType;
}

bool ParameterNativeVariable::isField() const {
  return false;
}

bool ParameterNativeVariable::isSystem() const {
  return false;
}

Value* ParameterNativeVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return mValue;
}

Value* ParameterNativeVariable::generateIdentifierReferenceIR(IRGenerationContext&
                                                                 context) const {
  assert(false);
}

Value* ParameterNativeVariable::generateAssignmentIR(IRGenerationContext& context,
                                                     IExpression* assignToExpression,
                                                     vector<const IExpression*> arrayIndices,
                                                     int line) {
  Log::e("Assignment to function parameters is not allowed");
  exit(1);
}

void ParameterNativeVariable::decrementReferenceCounter(IRGenerationContext& context) const {
}
