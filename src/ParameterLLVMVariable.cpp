//
//  ParameterLLVMVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/Log.hpp"
#include "wisey/ParameterLLVMVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterLLVMVariable::ParameterLLVMVariable(string name,
                                             const ILLVMType* type,
                                             Value* value) :
mName(name), mType(type), mValue(value) { }

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

bool ParameterLLVMVariable::isSystem() const {
  return false;
}

Value* ParameterLLVMVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return mValue;
}

Value* ParameterLLVMVariable::generateIdentifierReferenceIR(IRGenerationContext&
                                                            context) const {
  assert(false);
}

Value* ParameterLLVMVariable::generateAssignmentIR(IRGenerationContext& context,
                                                   IExpression* assignToExpression,
                                                   vector<const IExpression*> arrayIndices,
                                                   int line) {
  Log::e_deprecated("Assignment to method parameters is not allowed");
  exit(1);
}
