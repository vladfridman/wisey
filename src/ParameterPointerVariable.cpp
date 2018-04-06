//
//  ParameterPointerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/Log.hpp"
#include "wisey/ParameterPointerVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterPointerVariable::ParameterPointerVariable(string name,
                                                   const LLVMPointerType* type,
                                                   Value* value) :
mName(name), mType(type), mValue(value) {
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

bool ParameterPointerVariable::isSystem() const {
  return false;
}

Value* ParameterPointerVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return mValue;
}

Value* ParameterPointerVariable::generateIdentifierReferenceIR(IRGenerationContext&
                                                                 context) const {
  assert(false);
}

Value* ParameterPointerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                     IExpression* assignToExpression,
                                                     vector<const IExpression*> arrayIndices,
                                                     int line) {
  Log::e_deprecated("Assignment to method parameters is not allowed");
  exit(1);
}
