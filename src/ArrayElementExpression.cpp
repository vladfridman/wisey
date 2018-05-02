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
#include "wisey/CheckArrayIndexFunction.hpp"
#include "wisey/CheckForNullAndThrowFunction.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ImmutableArrayType.hpp"
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

int ArrayElementExpression::getLine() const {
  return mLine;
}

Value* ArrayElementExpression::generateIR(IRGenerationContext& context,
                                          const IType* assignToType) const {
  const IType* expressionType = mArrayExpression->getType(context);
  if (!expressionType->isArray()) {
    reportErrorArrayType(expressionType->getTypeName());
    exit(1);
  }
  const ArrayType* arrayType = expressionType->getArrayType(context);
  Value* arrayStructPointer = mArrayExpression->generateIR(context, PrimitiveTypes::VOID);
  
  Composer::setLineNumber(context, mLine);
  CheckForNullAndThrowFunction::call(context, arrayStructPointer);
  Value* pointer = getArrayElement(context, arrayStructPointer, mArrayIndexExpresion, mLine);
  
  if (arrayType->getNumberOfDimensions() > 1) {
    Type* resultType = getType(context)->getLLVMType(context);
    return IRWriter::newBitCastInst(context, pointer, resultType);
  }
  
  const IType* elementType = arrayType->getElementType();
  Type* resultLLVMType = elementType->getLLVMType(context);
  Value* resultStore = IRWriter::newBitCastInst(context, pointer, resultLLVMType->getPointerTo());
  Value* result = IRWriter::newLoadInst(context, resultStore, "");
  
  if (assignToType->isOwner()) {
    assert(elementType->isOwner() || (elementType->isReference() && elementType->isNative()));
    Value* null = ConstantPointerNull::get((llvm::PointerType* ) resultLLVMType);
    IRWriter::newStoreInst(context, null, resultStore);
  }
  
  return result;
}

Value* ArrayElementExpression::getArrayElement(IRGenerationContext &context,
                                               Value* arrayStructPointer,
                                               const IExpression* indexExpression,
                                               int line) {
  LLVMContext& llvmContext = context.getLLVMContext();
  const IType* arrayIndexExpressionType = indexExpression->getType(context);
  if (arrayIndexExpressionType != PrimitiveTypes::INT &&
      arrayIndexExpressionType != PrimitiveTypes::LONG) {
    context.reportError(line, "Array index should be integer type, but it is " +
                        arrayIndexExpressionType->getTypeName());
    exit(1);
  }

  Value* indexValue = indexExpression->generateIR(context, PrimitiveTypes::VOID);
  Value* indexValueCast = arrayIndexExpressionType->castTo(context,
                                                           indexValue,
                                                           PrimitiveTypes::LONG,
                                                           0);

  Value* index[2];
  index[0] = ConstantInt::get(llvm::Type::getInt64Ty(llvmContext), 0);
  index[1] = ConstantInt::get(llvm::Type::getInt32Ty(llvmContext), 1);
  Value* arraySizeStore = IRWriter::createGetElementPtrInst(context, arrayStructPointer, index);
  Value* arraySize = IRWriter::newLoadInst(context, arraySizeStore, "arraySize");
  CheckArrayIndexFunction::call(context, indexValueCast, arraySize);
  index[1] = ConstantInt::get(llvm::Type::getInt32Ty(llvmContext), 2);
  Value* elementSizeStore = IRWriter::createGetElementPtrInst(context, arrayStructPointer, index);
  Value* elementSize = IRWriter::newLoadInst(context, elementSizeStore, "elementSize");
  index[1] = ConstantInt::get(llvm::Type::getInt32Ty(llvmContext),
                              ArrayType::ARRAY_ELEMENTS_START_INDEX);
  Value* arrayPointer = IRWriter::createGetElementPtrInst(context, arrayStructPointer, index);

  Value* offset = IRWriter::createBinaryOperator(context,
                                                 Instruction::Mul,
                                                 elementSize,
                                                 indexValueCast,
                                                 "offset");
  
  llvm::PointerType* int8Pointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  Value* genericPointer = IRWriter::newBitCastInst(context, arrayPointer, int8Pointer);
  Value* idx[1];
  idx[0] = offset;
  
  return IRWriter::createGetElementPtrInst(context, genericPointer, idx);
}

Value* ArrayElementExpression::generateElementIR(IRGenerationContext& context,
                                                 const ArrayType* arrayType,
                                                 Value* arrayStructPointer,
                                                 vector<const IExpression*> arrayIndices,
                                                 int line) {
  LLVMContext& llvmContext = context.getLLVMContext();

  if (arrayType->getNumberOfDimensions() != arrayIndices.size()) {
    context.reportError(line, "Expression does not reference an array element");
    exit(1);
  }
  
  CheckForNullAndThrowFunction::call(context, arrayStructPointer);
  
  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), 0);

  Value* value = arrayStructPointer;
  while (arrayIndices.size()) {
    const IExpression* indexExpression = arrayIndices.back();
    arrayIndices.pop_back();
    
    Value* element = getArrayElement(context, value, indexExpression, line);
    const ArrayType* subArrayType = context.getArrayType(arrayType->getElementType(),
                                                         arrayIndices.size());
    value = IRWriter::newBitCastInst(context, element, subArrayType->getLLVMType(context));
  }
  
  return value;
}

IVariable* ArrayElementExpression::getVariable(IRGenerationContext& context,
                                               vector<const IExpression*>& arrayIndices) const {
  if (!mArrayExpression->isAssignable()) {
    context.reportError(mLine, "Expression is not assignable");
    exit(1);
  }
  arrayIndices.push_back(mArrayIndexExpresion);
  return ((const IExpressionAssignable*) mArrayExpression)->getVariable(context, arrayIndices);
}

bool ArrayElementExpression::isConstant() const {
  return false;
}

bool ArrayElementExpression::isAssignable() const {
  return true;
}

const IType* ArrayElementExpression::getType(IRGenerationContext& context) const {
  const IType* arrayExpressionType = mArrayExpression->getType(context);
  
  if (!arrayExpressionType->isArray()) {
    reportErrorArrayType(arrayExpressionType->getTypeName());
    exit(1);
  }
  
  const ArrayType* arrayType = arrayExpressionType->getArrayType(context);
  const IType* elementType = arrayType->getElementType();

  unsigned long numberOfDimensions = arrayType->getNumberOfDimensions();
  if (numberOfDimensions == 1u) {
    return elementType;
  }
  
  wisey::ArrayType* subArrayType = context.getArrayType(elementType, numberOfDimensions - 1);
  if (arrayExpressionType->isImmutable()) {
    return subArrayType->getImmutable();
  }
  
  return subArrayType;
}

void ArrayElementExpression::printToStream(IRGenerationContext& context, iostream& stream) const {
  mArrayExpression->printToStream(context, stream);
  stream << "[";
  mArrayIndexExpresion->printToStream(context, stream);
  stream << "]";
}

void ArrayElementExpression::reportErrorArrayType(string typeName) {
  Log::e_deprecated("Expecting array type expression before [] but expression type is " + typeName);
}
