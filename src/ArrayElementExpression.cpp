//
//  ArrayElementExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayElementExpression.hpp"
#include "wisey/IType.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ArrayElementExpression::ArrayElementExpression(const IExpression* arrayExpression,
                                               const IExpression* arrayIndexExpresion) :
mArrayExpression(arrayExpression), mArrayIndexExpresion(arrayIndexExpresion) {
}

ArrayElementExpression::~ArrayElementExpression() {
  delete mArrayExpression;
  delete mArrayIndexExpresion;
}

Value* ArrayElementExpression::generateIR(IRGenerationContext& context,
                                          IRGenerationFlag flag) const {
  vector<const IExpression*> arrayIndices;
  IVariable* variable = getVariable(context, arrayIndices);
  assert(variable);
  Value* arrayPointer = variable->generateIdentifierIR(context);

  Value* pointer = generateElementIR(context, variable->getType(), arrayPointer, arrayIndices);
  Value* result = IRWriter::newLoadInst(context, pointer, "");
  
  if (flag == IR_GENERATION_RELEASE) {
    assert(variable->getType()->getTypeKind() == ARRAY_TYPE);
    const ArrayType* arrayType = (const ArrayType*) variable->getType();
    assert(IType::isOwnerType(arrayType->getScalarType()));
    PointerType* llvmType = (PointerType*) arrayType->getScalarType()->getLLVMType(context);
    IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), pointer);
  }
  
  return result;
}

Value* ArrayElementExpression::generateElementIR(IRGenerationContext& context,
                                                 const IType* arrayType,
                                                 Value* arrayPointer,
                                                 vector<const IExpression*> arrayIndices) {
  Value* arrayExpressionValue = arrayPointer;
  const IType* valueType = arrayType;
  for (const IExpression* indexExpression : arrayIndices) {
    if (valueType->getTypeKind() != ARRAY_TYPE) {
      Log::e("Expecting array type expression before [] but expression type is " +
             valueType->getTypeName());
      exit(1);
    }
    const wisey::ArrayType* arrayType = (const wisey::ArrayType*) valueType;
    valueType = arrayType->getBaseType();
    const IType* arrayIndexExpressionType = indexExpression->getType(context);
    if (arrayIndexExpressionType != PrimitiveTypes::INT_TYPE &&
        arrayIndexExpressionType != PrimitiveTypes::LONG_TYPE) {
      Log::e("Array index should be integer type, but it is " +
             arrayIndexExpressionType->getTypeName());
      exit(1);
    }
    
    Value* arrayIndexValue = indexExpression->generateIR(context, IR_GENERATION_NORMAL);
    Type* indexType = arrayIndexExpressionType->getLLVMType(context);
    Value* index[2];
    index[0] = ConstantInt::get(indexType, 0);
    index[1] = arrayIndexValue;
    
    arrayExpressionValue = IRWriter::createGetElementPtrInst(context, arrayExpressionValue, index);
  }
  
  if (valueType->getTypeKind() == ARRAY_TYPE) {
    Log::e("Expression does not reference an array element");
    exit(1);
  }
  
  return arrayExpressionValue;
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
  if (arrayExpressionType->getTypeKind() != ARRAY_TYPE) {
    reportErrorArrayType(arrayExpressionType->getTypeName());
    exit(1);
  }
  
  const wisey::ArrayType* arrayType = (const wisey::ArrayType*) arrayExpressionType;
  return arrayType->getBaseType();
}

void ArrayElementExpression::printToStream(IRGenerationContext& context, iostream& stream) const {
  mArrayExpression->printToStream(context, stream);
  stream << "[";
  mArrayIndexExpresion->printToStream(context, stream);
  stream << "]";
}

void ArrayElementExpression::reportErrorArrayType(string typeName) const {
  Log::e("Expecting array type expression before [] but expression type is " + typeName);
}
