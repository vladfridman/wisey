//
//  ParameterArrayReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/ArrayElementAssignment.hpp"
#include "wisey/ArrayElementExpression.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
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

bool ParameterArrayReferenceVariable::isSystem() const {
  return false;
}

Value* ParameterArrayReferenceVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return mValue;
}

Value* ParameterArrayReferenceVariable::generateAssignmentIR(IRGenerationContext& context,
                                                             IExpression* assignToExpression,
                                                             vector<const IExpression*> arrayIndices,
                                                             int line) {
  if (!arrayIndices.size()) {
    Log::e("Assignment to method parameters is not allowed");
    exit(1);
  }
  
  Composer::pushCallStack(context, line);
  
  Value* elementStore = ArrayElementExpression::generateElementIR(context,
                                                                  mArrayType,
                                                                  mValue,
                                                                  arrayIndices);
  
  Value* result = ArrayElementAssignment::generateElementAssignment(context,
                                                                    mArrayType->getElementType(),
                                                                    assignToExpression,
                                                                    elementStore,
                                                                    line);

  Composer::popCallStack(context);
  
  return result;
}

void ParameterArrayReferenceVariable::decrementReferenceCounter(IRGenerationContext&
                                                                context) const {
  mArrayType->decrementReferenceCount(context, mValue);
}

