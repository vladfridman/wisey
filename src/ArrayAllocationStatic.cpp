//
//  ArrayAllocationStatic.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayAllocation.hpp"
#include "wisey/ArrayAllocationStatic.hpp"
#include "wisey/ArrayElementAssignment.hpp"
#include "wisey/ArrayElementExpression.hpp"
#include "wisey/ArrayOwnerType.hpp"
#include "wisey/IVariable.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalArrayOwnerVariable.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ArrayAllocationStatic::ArrayAllocationStatic(ExpressionList expressionList, int line) :
mExpressionList(expressionList), mLine(line) {
}

ArrayAllocationStatic::~ArrayAllocationStatic() {
  for (const IExpression* expression : mExpressionList) {
    delete expression;
  }
  mExpressionList.clear();
}

Value* ArrayAllocationStatic::generateIR(IRGenerationContext &context,
                                         const IType* assignToType) const {
  const ArraySpecificType* arraySpecificType = getSpecificType(context);
  Value* arrayStructPointer = ArrayAllocation::allocateArray(context, arraySpecificType);
  list<unsigned long> dimensions;
  for (unsigned long dimension : arraySpecificType->getDimensions()) {
    dimensions.push_back(dimension);
  }
  initializeArray(context, arrayStructPointer, arraySpecificType->getElementType(), dimensions);
  
  if (assignToType->isOwner()) {
    return arrayStructPointer;
  }
  
  Value* alloc = IRWriter::newAllocaInst(context, arrayStructPointer->getType(), "");
  IRWriter::newStoreInst(context, arrayStructPointer, alloc);
  const ArrayOwnerType* arrayOwnerType = arraySpecificType->getArrayType(context)->getOwner();
  string variableName = IVariable::getTemporaryVariableName(this);
  IVariable* variable = new LocalArrayOwnerVariable(variableName, arrayOwnerType, alloc);
  context.getScopes().setVariable(variable);
  
  return arrayStructPointer;
}

IVariable* ArrayAllocationStatic::getVariable(IRGenerationContext &context,
                                        vector<const IExpression *> &arrayIndices) const {
  return NULL;
}

bool ArrayAllocationStatic::isConstant() const {
  return false;
}

const ArrayOwnerType* ArrayAllocationStatic::getType(IRGenerationContext& context) const {
  return getSpecificType(context)->getArrayType(context)->getOwner();
}

const ArraySpecificType* ArrayAllocationStatic::getSpecificType(IRGenerationContext&
                                                                context) const {
  checkArrayElements(context);
  const IType* elementType = mExpressionList.front()->getType(context);
  vector<unsigned long> dimensions;
  dimensions.push_back(mExpressionList.size());
  if (elementType->getTypeKind() != ARRAY_OWNER_TYPE) {
    return context.getArraySpecificType(elementType, dimensions);
  }
  
  const ArrayType* subArrayType = ((const ArrayOwnerType*) elementType)->getArrayType();
  for (unsigned long dimension : subArrayType->getDimensions()) {
    dimensions.push_back(dimension);
  }
  return context.getArraySpecificType(subArrayType->getElementType(), dimensions);
}


void ArrayAllocationStatic::printToStream(IRGenerationContext &context, iostream &stream) const {
  stream << "{ ";
  for (const IExpression* expression : mExpressionList) {
    expression->printToStream(context, stream);
    if (expression != mExpressionList.back()) {
      stream << ", ";
    }
  }
  stream << " }";
}

void ArrayAllocationStatic::checkArrayElements(IRGenerationContext &context) const {
  const IType* firstType = mExpressionList.front()->getType(context);
  for (const IExpression* expression : mExpressionList) {
    if (expression->getType(context) == firstType) {
      continue;
    }
    
    Log::e("All elements in static array allocation should be of the same type");
    exit(1);
  }
}

void ArrayAllocationStatic::initializeArray(IRGenerationContext &context,
                                            Value* arrayStructPointer,
                                            const IType* elementType,
                                            list<unsigned long> dimensions) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int64Type = Type::getInt64Ty(llvmContext);
  
  Value* arrayPointer = ArrayElementExpression::unwrapArray(context, arrayStructPointer);
  Value* index[2];
  index[0] = ConstantInt::get(int64Type, 0);
  dimensions.pop_front();
  
  unsigned long arrayIndex = 0;
  for (const IExpression* expression : mExpressionList) {
    index[1] = ConstantInt::get(int64Type, arrayIndex);
    Value* elementStore = IRWriter::createGetElementPtrInst(context, arrayPointer, index);
    if (dimensions.size()) {
      ((ArrayAllocationStatic*) expression)->initializeArray(context,
                                                             elementStore,
                                                             elementType,
                                                             dimensions);
    } else {
      ArrayElementAssignment::generateElementAssignment(context,
                                                        elementType,
                                                        expression,
                                                        elementStore,
                                                        mLine);
    }
    arrayIndex++;
  }
}

ExpressionList ArrayAllocationStatic::getExpressionList() const {
  return mExpressionList;
}

ExpressionList ArrayAllocationStatic::flattenExpressionList(IRGenerationContext&
                                                                  context) const {
  list<const IExpression*> stack;
  for (const IExpression* expression : mExpressionList) {
    stack.push_back(expression);
  }
  list<const IExpression*> result;

  while (stack.size()) {
    const IExpression* expression = stack.back();
    stack.pop_back();
    if (expression->getType(context)->getTypeKind() == ARRAY_OWNER_TYPE) {
      for (const IExpression* subExpression :
           ((ArrayAllocationStatic*) expression)->getExpressionList()) {
        stack.push_back(subExpression);
      }
    } else {
      result.push_front(expression);
    }
  }
  
  ExpressionList resultVector;
  for (const IExpression* expression : result) {
    resultVector.push_back(expression);
  }

  return resultVector;
}
