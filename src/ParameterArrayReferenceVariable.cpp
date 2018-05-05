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
                                                                 Value* value,
                                                                 int line) :
mName(name), mArrayType(arrayType), mValue(value), mLine(line) {
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

bool ParameterArrayReferenceVariable::isField() const {
  return false;
}

bool ParameterArrayReferenceVariable::isSystem() const {
  return false;
}

int ParameterArrayReferenceVariable::getLine() const {
  return mLine;
}

Value* ParameterArrayReferenceVariable::generateIdentifierIR(IRGenerationContext& context,
                                                             int line) const {
  return mValue;
}

Value* ParameterArrayReferenceVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                                      int line) const {
  assert(false);
}

Value* ParameterArrayReferenceVariable::generateAssignmentIR(IRGenerationContext& context,
                                                             IExpression* assignToExpression,
                                                             vector<const IExpression*>
                                                             arrayIndices,
                                                             int line) {
  if (!arrayIndices.size()) {
    context.reportError(line, "Assignment to method parameters is not allowed");
    throw 1;
  }
  
  Composer::setLineNumber(context, line);
  
  Value* elementStore = ArrayElementExpression::generateElementIR(context,
                                                                  mArrayType,
                                                                  mValue,
                                                                  arrayIndices,
                                                                  line);
  
  Value* result = ArrayElementAssignment::generateElementAssignment(context,
                                                                    mArrayType->getElementType(),
                                                                    assignToExpression,
                                                                    elementStore,
                                                                    line);
  
  return result;
}

void ParameterArrayReferenceVariable::decrementReferenceCounter(IRGenerationContext&
                                                                context) const {
  mArrayType->decrementReferenceCount(context, mValue);
}

