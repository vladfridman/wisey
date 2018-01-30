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
  LLVMContext& llvmContext = context.getLLVMContext();
  const IType* expressionType = mArrayExpression->getType(context);
  if (!IType::isArrayType(expressionType)) {
    reportErrorArrayType(expressionType->getTypeName());
    exit(1);
  }
  const ArrayType* arrayType = expressionType->getTypeKind() == ARRAY_TYPE
  ? (const ArrayType*) expressionType
  : ((const ArrayOwnerType*) expressionType)->getArrayType();
  Value* arrayStructPointer = mArrayExpression->generateIR(context, assignToType);
  
  Composer::pushCallStack(context, mLine);
  CheckForNullAndThrowFunction::call(context, arrayStructPointer);
  Value* index[2];
  index[0] = ConstantInt::get(llvm::Type::getInt64Ty(llvmContext), 0);
  index[1] = ConstantInt::get(llvm::Type::getInt32Ty(llvmContext), 2);
  Value* elementSizeStore = IRWriter::createGetElementPtrInst(context, arrayStructPointer, index);
  Value* elementSize = IRWriter::newLoadInst(context, elementSizeStore, "elementSize");
  index[1] = ConstantInt::get(llvm::Type::getInt32Ty(llvmContext),
                              ArrayType::ARRAY_ELEMENTS_START_INDEX);
  Value* arrayPointer = IRWriter::createGetElementPtrInst(context, arrayStructPointer, index);
  Composer::popCallStack(context);
  
  Value* pointer = getArrayElement(context, arrayPointer, elementSize, mArrayIndexExpresion);
  if (arrayType->getNumberOfDimensions() > 1) {
    Type* resultType = getType(context)->getLLVMType(context);
    return IRWriter::newBitCastInst(context, pointer, resultType);
  }
  
  const IType* elementType = arrayType->getElementType();
  Type* resultLLVMType = elementType->getLLVMType(context);
  Value* resultStore = IRWriter::newBitCastInst(context, pointer, resultLLVMType->getPointerTo());
  Value* result = IRWriter::newLoadInst(context, resultStore, "");
  
  if (assignToType->isOwner()) {
    assert(IType::isOwnerType(elementType));
    Value* null = ConstantPointerNull::get((PointerType* ) resultLLVMType);
    IRWriter::newStoreInst(context, null, resultStore);
  }
  
  return result;
}

Value* ArrayElementExpression::getArrayElement(IRGenerationContext &context,
                                               Value* arrayPointer,
                                               Value* elementSize,
                                               const IExpression* indexExpression) {
  LLVMContext& llvmContext = context.getLLVMContext();
  const IType* arrayIndexExpressionType = indexExpression->getType(context);
  if (arrayIndexExpressionType != PrimitiveTypes::INT_TYPE &&
      arrayIndexExpressionType != PrimitiveTypes::LONG_TYPE) {
    Log::e("Array index should be integer type, but it is " +
           arrayIndexExpressionType->getTypeName());
    exit(1);
  }
  
  Value* indexValue = indexExpression->generateIR(context, PrimitiveTypes::VOID_TYPE);
  Value* indexValueCast = arrayIndexExpressionType->castTo(context,
                                                           indexValue,
                                                           PrimitiveTypes::LONG_TYPE,
                                                           0);
  Value* offset = IRWriter::createBinaryOperator(context,
                                                 Instruction::Mul,
                                                 elementSize,
                                                 indexValueCast,
                                                 "offset");
  
  PointerType* int8Pointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  Value* genericPointer = IRWriter::newBitCastInst(context, arrayPointer, int8Pointer);
  Value* idx[1];
  idx[0] = offset;
  
  return IRWriter::createGetElementPtrInst(context, genericPointer, idx);
}

Value* ArrayElementExpression::generateElementIR(IRGenerationContext& context,
                                                 const ArrayType* arrayType,
                                                 Value* arrayStructPointer,
                                                 vector<const IExpression*> arrayIndices) {
  LLVMContext& llvmContext = context.getLLVMContext();

  if (arrayType->getNumberOfDimensions() != arrayIndices.size()) {
    Log::e("Expression does not reference an array element");
    exit(1);
  }
  
  CheckForNullAndThrowFunction::call(context, arrayStructPointer);
  
  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), 0);
  
  list<unsigned long> dimensions;
  for (unsigned long dimension : arrayType->getDimensions()) {
    dimensions.push_back(dimension);
  }

  Value* value = arrayStructPointer;
  while (arrayIndices.size()) {
    index[1] = ConstantInt::get(llvm::Type::getInt32Ty(llvmContext), 2);
    Value* elementSizeStore = IRWriter::createGetElementPtrInst(context, value, index);
    Value* elementSize = IRWriter::newLoadInst(context, elementSizeStore, "elementSize");
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext),
                                ArrayType::ARRAY_ELEMENTS_START_INDEX);
    Value* arrayPointer = IRWriter::createGetElementPtrInst(context, value, index);

    const IExpression* indexExpression = arrayIndices.back();
    arrayIndices.pop_back();
    
    dimensions.pop_front();
    vector<unsigned long> subDimensions;
    for (unsigned long dimension : dimensions) {
      subDimensions.push_back(dimension);
    }
    
    Value* element = getArrayElement(context, arrayPointer, elementSize, indexExpression);
    Type* subArrayType = subDimensions.size()
    ? context.getArrayType(arrayType->getElementType(), subDimensions)->getLLVMType(context)
    : arrayType->getElementType()->getLLVMType(context)->getPointerTo();
    
    value = IRWriter::newBitCastInst(context, element, subArrayType);
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

  list<unsigned long> dimensions;
  for (unsigned long dimension : arrayType->getDimensions()) {
    dimensions.push_back(dimension);
  }
  dimensions.pop_front();
  vector<unsigned long> subDimensions;
  for (unsigned long dimension : dimensions) {
    subDimensions.push_back(dimension);
  }

  return arrayType->getNumberOfDimensions() - 1
  ? context.getArrayType(elementType, subDimensions)
  : elementType;
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
