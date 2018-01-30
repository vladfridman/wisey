//
//  ArrayElementExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayElementExpression.hpp"
#include "wisey/ArrayOwnerType.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/Composer.hpp"
#include "wisey/CheckForNullAndThrowFunction.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ArrayElementExpression::ArrayElementExpression(const IExpression* arrayExpression,
                                               const IExpression* arrayIndexExpresion,
                                               int line) :
mArrayExpression(arrayExpression), mArrayIndexExpresion(arrayIndexExpresion), mLine(line) {
}

ArrayElementExpression::~ArrayElementExpression() {
  delete mArrayExpression;
  delete mArrayIndexExpresion;
}

Value* ArrayElementExpression::generateIR(IRGenerationContext& context,
                                          const IType* assignToType) const {
  const IType* expressionType = mArrayExpression->getType(context);
  if (!IType::isArrayType(expressionType)) {
    reportErrorArrayType(expressionType->getTypeName());
    exit(1);
  }
  const ArrayType* arrayType = expressionType->getTypeKind() == ARRAY_TYPE
  ? (const ArrayType*) expressionType
  : ((const ArrayOwnerType*) expressionType)->getArrayType();
  Value* arrayPointer = mArrayExpression->generateIR(context, assignToType);
  
  Composer::pushCallStack(context, mLine);
  CheckForNullAndThrowFunction::call(context, arrayPointer);
  arrayPointer = unwrapArray(context, arrayPointer);
  Composer::popCallStack(context);

  Value* pointer = getArrayElement(context, arrayPointer, mArrayIndexExpresion);
  if (arrayType->getDimensions().size() > 1) {
    return pointer;
  }
  
  Value* result = IRWriter::newLoadInst(context, pointer, "");
  
  if (assignToType->isOwner()) {
    const IType* elementType = arrayType->getElementType();
    assert(IType::isOwnerType(elementType));
    PointerType* llvmType = (PointerType*) elementType->getLLVMType(context);
    IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), pointer);
  }
  
  return result;
}

Value* ArrayElementExpression::unwrapArray(IRGenerationContext& context,
                                           llvm::Value* arrayPointer) {
  LLVMContext& llvmContext = context.getLLVMContext();

  Value* index[2];
  index[0] = ConstantInt::get(llvm::Type::getInt64Ty(llvmContext), 0);
  index[1] = ConstantInt::get(llvm::Type::getInt32Ty(llvmContext),
                              ArrayType::ARRAY_ELEMENTS_START_INDEX);
  
  return IRWriter::createGetElementPtrInst(context, arrayPointer, index);
}

Value* ArrayElementExpression::getArrayElement(IRGenerationContext &context,
                                               Value* arrayPointer,
                                               const IExpression* indexExpression) {
  const IType* arrayIndexExpressionType = indexExpression->getType(context);
  if (arrayIndexExpressionType != PrimitiveTypes::INT_TYPE &&
      arrayIndexExpressionType != PrimitiveTypes::LONG_TYPE) {
    Log::e("Array index should be integer type, but it is " +
           arrayIndexExpressionType->getTypeName());
    exit(1);
  }
  
  Value* arrayIndexValue = indexExpression->generateIR(context, PrimitiveTypes::VOID_TYPE);
  Type* indexType = arrayIndexExpressionType->getLLVMType(context);
  Value* index[2];
  index[0] = ConstantInt::get(indexType, 0);
  index[1] = arrayIndexValue;
  
 return IRWriter::createGetElementPtrInst(context, arrayPointer, index);
}

Value* ArrayElementExpression::generateElementIR(IRGenerationContext& context,
                                                 const ArrayType* arrayType,
                                                 Value* arrayStructPointer,
                                                 vector<const IExpression*> arrayIndices) {
  if (arrayType->getDimensions().size() != arrayIndices.size()) {
    Log::e("Expression does not reference an array element");
    exit(1);
  }
  
  CheckForNullAndThrowFunction::call(context, arrayStructPointer);
  
  Value* value = arrayStructPointer;
  const IType* valueType = arrayType;
  while (arrayIndices.size()) {
    Value* arrayPointer = unwrapArray(context, value);
    const IExpression* indexExpression = arrayIndices.back();
    arrayIndices.pop_back();
    
    if (valueType->getTypeKind() != ARRAY_TYPE) {
      reportErrorArrayType(valueType->getTypeName());
      exit(1);
    }
    
    value = getArrayElement(context, arrayPointer, indexExpression);
  }
  
  return value;
}

IVariable* ArrayElementExpression::getVariable(IRGenerationContext& context,
                                               vector<const IExpression*>& arrayIndices) const {
  arrayIndices.push_back(mArrayIndexExpresion);
  return mArrayExpression->getVariable(context, arrayIndices);
}

bool ArrayElementExpression::isConstant() const {
  return false;
}

const IType* ArrayElementExpression::getType(IRGenerationContext& context) const {
  const IType* arrayExpressionType = mArrayExpression->getType(context);
  
  if (!IType::isArrayType(arrayExpressionType)) {
    reportErrorArrayType(arrayExpressionType->getTypeName());
    exit(1);
  }
  
  const ArrayType* arrayType = arrayExpressionType->getTypeKind() == ARRAY_TYPE
  ? (const ArrayType*) arrayExpressionType
  : ((const ArrayOwnerType*) arrayExpressionType)->getArrayType();
  const IType* elementType = arrayType->getElementType();

  vector<unsigned long> dimensions = arrayType->getDimensions();
  dimensions.erase(dimensions.begin());
  
  return dimensions.size() ? context.getArrayType(elementType, dimensions) : elementType;
}

void ArrayElementExpression::printToStream(IRGenerationContext& context, iostream& stream) const {
  mArrayExpression->printToStream(context, stream);
  stream << "[";
  mArrayIndexExpresion->printToStream(context, stream);
  stream << "]";
}

void ArrayElementExpression::reportErrorArrayType(string typeName) {
  Log::e("Expecting array type expression before [] but expression type is " + typeName);
}
