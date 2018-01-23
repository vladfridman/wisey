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
  vector<const IExpression*> arrayIndices;
  IVariable* variable = getVariable(context, arrayIndices);
  assert(variable);
  const IType* variableType = variable->getType();
  if (!IType::isArrayType(variableType)) {
    reportErrorArrayType(variableType->getTypeName());
    exit(1);
  }
  const ArrayType* arrayType = variableType->getTypeKind() == ARRAY_TYPE
  ? (const ArrayType*) variableType
  : ((const ArrayOwnerType*) variableType)->getArrayType();
  Value* arrayPointer = variable->generateIdentifierIR(context);
  
  Composer::pushCallStack(context, mLine);
  Value* pointer = generateElementIR(context, arrayType, arrayPointer, arrayIndices);
  Composer::popCallStack(context);
  Value* result = IRWriter::newLoadInst(context, pointer, "");
  
  if (assignToType->isOwner()) {
    const IType* elementType = arrayType->getElementType();
    assert(IType::isOwnerType(elementType));
    PointerType* llvmType = (PointerType*) elementType->getLLVMType(context);
    IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), pointer);
  }
  
  return result;
}

Value* ArrayElementExpression::generateElementIR(IRGenerationContext& context,
                                                 const ArrayType* arrayType,
                                                 Value* arrayPointer,
                                                 vector<const IExpression*> arrayIndices) {
  CheckForNullAndThrowFunction::call(context, arrayPointer);
  
  LLVMContext& llvmContext = context.getLLVMContext();
  Value* index[2];
  index[0] = ConstantInt::get(llvm::Type::getInt64Ty(llvmContext), 0);
  index[1] = ConstantInt::get(llvm::Type::getInt32Ty(llvmContext), 4);
  Value* value = IRWriter::createGetElementPtrInst(context, arrayPointer, index);
  const IType* valueType = arrayType;
  for (const IExpression* indexExpression : arrayIndices) {
    if (valueType->getTypeKind() != ARRAY_TYPE) {
      reportErrorArrayType(valueType->getTypeName());
      exit(1);
    }
    
    valueType = ((const ArrayType*) valueType)->getBaseType();

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
    
    value = IRWriter::createGetElementPtrInst(context, value, index);
  }
  
  if (valueType->getTypeKind() == ARRAY_TYPE) {
    Log::e("Expression does not reference an array element");
    exit(1);
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

  return arrayType->getBaseType();
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
