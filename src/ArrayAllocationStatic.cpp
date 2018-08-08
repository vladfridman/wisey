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
#include "wisey/PrimitiveTypes.hpp"

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

int ArrayAllocationStatic::getLine() const {
  return mLine;
}

llvm::Constant* ArrayAllocationStatic::generateIR(IRGenerationContext& context,
                                                  const IType* assignToType) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  const ArrayExactType* arrayExactType = getExactType(context);
  llvm::StructType* arrayStruct = arrayExactType->getLLVMType(context);
  vector<llvm::Constant*> elementValues;
  for (const IExpression* expression : mExpressionList) {
    Value* value = expression->generateIR(context, PrimitiveTypes::VOID);
    elementValues.push_back((llvm::Constant*) value);
  }
  llvm::ArrayType* arrayType = llvm::ArrayType::get(elementValues.front()->getType(),
                                                    elementValues.size());
  llvm::Constant* constantArray = ConstantArray::get(arrayType, elementValues);
  vector<llvm::Constant*> constantStruct;
  constantStruct.push_back(ConstantInt::get(Type::getInt64Ty(llvmContext), 0));
  constantStruct.push_back(ConstantInt::get(Type::getInt64Ty(llvmContext), elementValues.size()));
  constantStruct.push_back(ConstantExpr::getSizeOf(elementValues.front()->getType()));
  constantStruct.push_back(constantArray);

  return ConstantStruct::get(arrayStruct, constantStruct);
}

bool ArrayAllocationStatic::isConstant() const {
  return true;
}

bool ArrayAllocationStatic::isAssignable() const {
  return false;
}

const ArrayExactType* ArrayAllocationStatic::getType(IRGenerationContext& context) const {
  return getExactType(context);
}

const ArrayExactType* ArrayAllocationStatic::getExactType(IRGenerationContext& context) const {
  checkArrayElements(context);
  const IType* elementType = mExpressionList.front()->getType(context);
  list<unsigned long> dimensions;
  dimensions.push_back(mExpressionList.size());
  if (!elementType->isArray()) {
    return context.getArrayExactType(elementType, dimensions);
  }
  
  const ArrayExactType* subArrayType = (const ArrayExactType*) elementType;
  for (unsigned long dimension : subArrayType->getDimensions()) {
    dimensions.push_back(dimension);
  }
  return context.getArrayExactType(subArrayType->getElementType(), dimensions);
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
    if (!expression->isConstant()) {
      context.reportError(mLine, "Static array allocation may only contain array of constants");
      throw 1;
    }
    if (expression->getType(context) == firstType) {
      continue;
    }
    
    context.reportError(mLine,
                        "All elements in static array allocation should be of the same type");
    throw 1;
  }
}

ExpressionList ArrayAllocationStatic::getExpressionList() const {
  return mExpressionList;
}
