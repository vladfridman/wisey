//
//  ParameterArrayReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/ParameterArrayReferenceVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterArrayReferenceVariable::ParameterArrayReferenceVariable(string name,
                                                                 const ArrayType* arrayType,
                                                                 Value* value) :
mName(name), mArrayType(arrayType), mValue(value) {
  assert(value->getType()->isPointerTy());
  assert(value->getType()->getPointerElementType()->isStructTy());
}

ParameterArrayReferenceVariable::~ParameterArrayReferenceVariable() {
}

string ParameterArrayReferenceVariable::getName() const {
  return mName;
}

const wisey::ArrayType* ParameterArrayReferenceVariable::getType() const {
  return mArrayType;
}

Value* ParameterArrayReferenceVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return mValue;
}

Value* ParameterArrayReferenceVariable::generateAssignmentIR(IRGenerationContext& context,
                                                             IExpression* assignToExpression,
                                                             vector<const IExpression*> arrayIndices,
                                                             int line) {
  Log::e("Assignment to method parameters is not allowed");
  exit(1);
}

void ParameterArrayReferenceVariable::decrementReferenceCounter(IRGenerationContext&
                                                                context) const {
  mArrayType->decrementReferenceCount(context, mValue);
}
