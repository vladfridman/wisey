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

Value* ArrayAllocationStatic::generateIR(IRGenerationContext& context,
                                         const IType* assignToType) const {
  const ArrayExactType* arrayExactType = getExactType(context);
  Value* arrayStructPointer = allocateArray(context, arrayExactType);
  initializeArray(context,
                  arrayStructPointer,
                  arrayExactType->getElementType(),
                  arrayExactType->getDimensions());
  
  if (assignToType->isOwner()) {
    return arrayStructPointer;
  }
  
  Value* alloc = IRWriter::newAllocaInst(context, arrayStructPointer->getType(), "");
  IRWriter::newStoreInst(context, arrayStructPointer, alloc);
  const ArrayOwnerType* arrayOwnerType = arrayExactType->getArrayType(context)->getOwner();
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

const ArrayExactOwnerType* ArrayAllocationStatic::getType(IRGenerationContext& context) const {
  return getExactType(context)->getOwner();
}

const ArrayExactType* ArrayAllocationStatic::getExactType(IRGenerationContext& context) const {
  checkArrayElements(context);
  const IType* elementType = mExpressionList.front()->getType(context);
  list<unsigned long> dimensions;
  dimensions.push_back(mExpressionList.size());
  if (elementType->getTypeKind() != ARRAY_OWNER_TYPE) {
    return context.getArrayExactType(elementType, dimensions);
  }
  
  const ArrayExactOwnerType* subArrayType = (const ArrayExactOwnerType*) elementType;
  for (unsigned long dimension : subArrayType->getArrayExactType()->getDimensions()) {
    dimensions.push_back(dimension);
  }
  return context.getArrayExactType(subArrayType->getArrayExactType()->getElementType(), dimensions);
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
  
  Value* index[2];
  index[0] = ConstantInt::get(int64Type, 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), ArrayType::ARRAY_ELEMENTS_START_INDEX);
  Value* arrayPointer = IRWriter::createGetElementPtrInst(context, arrayStructPointer, index);
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

Value* ArrayAllocationStatic::allocateArray(IRGenerationContext& context,
                                            const ArrayExactType* arrayExactType) {
  PointerType* arrayPointerType = arrayExactType->getLLVMType(context);
  StructType* structType = (StructType*) arrayPointerType->getPointerElementType();
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  llvm::Constant* one = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.getLLVMContext()), 1);
  Instruction* malloc = IRWriter::createMalloc(context, structType, allocSize, one, "newarray");
  IntrinsicFunctions::setMemoryToZero(context, malloc, ConstantExpr::getSizeOf(structType));
  
  initializeEmptyArray(context, malloc, arrayExactType->getDimensions());
  
  return malloc;
}

void ArrayAllocationStatic::initializeEmptyArray(IRGenerationContext& context,
                                                 Value* arrayStructPointer,
                                                 list<unsigned long> dimensions) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Value* sizeStore = IRWriter::createGetElementPtrInst(context, arrayStructPointer, index);
  unsigned long arraySize = dimensions.front();
  ConstantInt* sizeValue = ConstantInt::get(Type::getInt64Ty(llvmContext), arraySize);
  IRWriter::newStoreInst(context, sizeValue, sizeStore);
  
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), ArrayType::ARRAY_ELEMENTS_START_INDEX);
  Value* arrayPointer = IRWriter::createGetElementPtrInst(context, arrayStructPointer, index);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  Value* firstElement = IRWriter::createGetElementPtrInst(context, arrayPointer, index);
  Value* elementSize = ConstantExpr::getSizeOf(firstElement->getType()->getPointerElementType());
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 2u);
  Value* elementSizeStore = IRWriter::createGetElementPtrInst(context, arrayStructPointer, index);
  IRWriter::newStoreInst(context, elementSize, elementSizeStore);
  
  if (dimensions.size() == 1) {
    return;
  }
  
  dimensions.pop_front();
  
  for (unsigned int i = 0; i < arraySize; i++) {
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), i);
    Value* arrayElement = IRWriter::createGetElementPtrInst(context, arrayPointer, index);
    initializeEmptyArray(context, arrayElement, dimensions);
  }
}
