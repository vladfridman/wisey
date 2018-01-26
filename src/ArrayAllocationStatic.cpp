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
  for (IExpression* expression : mExpressionList) {
    delete expression;
  }
  mExpressionList.clear();
}

Value* ArrayAllocationStatic::generateIR(IRGenerationContext &context,
                                         const IType* assignToType) const {
  const ArrayType* arrayType = getType(context)->getArrayType();
  Value* arrayPointer = ArrayAllocation::allocateArray(context, arrayType);
  initializeArray(context, arrayPointer, arrayType);
  
  if (assignToType->isOwner()) {
    return arrayPointer;
  }
  
  Value* alloc = IRWriter::newAllocaInst(context, arrayPointer->getType(), "");
  IRWriter::newStoreInst(context, arrayPointer, alloc);
  IVariable* variable = new LocalArrayOwnerVariable(IVariable::getTemporaryVariableName(this),
                                                    arrayType->getOwner(),
                                                    alloc);
  context.getScopes().setVariable(variable);
  
  return arrayPointer;
}

IVariable* ArrayAllocationStatic::getVariable(IRGenerationContext &context,
                                        vector<const IExpression *> &arrayIndices) const {
  return NULL;
}

bool ArrayAllocationStatic::isConstant() const {
  return false;
}

const ArrayOwnerType* ArrayAllocationStatic::getType(IRGenerationContext& context) const {
  checkArrayElements(context);
  const IType* elementType = mExpressionList.front()->getType(context);
  vector<unsigned long> dimensions;
  dimensions.push_back(mExpressionList.size());
  if (elementType->getTypeKind() != ARRAY_OWNER_TYPE) {
    return context.getArrayType(elementType, dimensions)->getOwner();
  }

  const ArrayType* subArrayType = ((const ArrayOwnerType*) elementType)->getArrayType();
  for (unsigned long dimension : subArrayType->getDimensions()) {
    dimensions.push_back(dimension);
  }
  return context.getArrayType(subArrayType->getElementType(), dimensions)->getOwner();
}

void ArrayAllocationStatic::printToStream(IRGenerationContext &context, iostream &stream) const {
  stream << "{ ";
  for (IExpression* expression : mExpressionList) {
    expression->printToStream(context, stream);
    if (expression != mExpressionList.back()) {
      stream << ", ";
    }
  }
  stream << " }";
}

void ArrayAllocationStatic::checkArrayElements(IRGenerationContext &context) const {
  const IType* firstType = mExpressionList.front()->getType(context);
  for (IExpression* expression : mExpressionList) {
    if (expression->getType(context) == firstType) {
      continue;
    }
    
    Log::e("All elements in static array allocation should be of the same type");
    exit(1);
  }
}

void ArrayAllocationStatic::initializeArray(IRGenerationContext &context,
                                            Value* arrayPointer,
                                            const ArrayType* arrayType) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int64Type = Type::getInt64Ty(llvmContext);
  
  Value* array = ArrayElementExpression::unwrapArray(context, arrayPointer);
  Value* index[2];
  index[0] = ConstantInt::get(int64Type, 0);
  
  unsigned long arrayIndex = 0;
  for (IExpression* expression : mExpressionList) {
    index[1] = ConstantInt::get(int64Type, arrayIndex);
    Value* elementStore = IRWriter::createGetElementPtrInst(context, array, index);
    ArrayElementAssignment::generateElementAssignment(context,
                                                      arrayType->getElementType(),
                                                      expression,
                                                      elementStore,
                                                      mLine);
    arrayIndex++;
  }
}
