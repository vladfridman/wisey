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
  const IType* arrayExpressionType = mArrayExpression->getType(context);
  const IType* arrayIndexExpressionType = mArrayIndexExpresion->getType(context);
  
  if (arrayExpressionType->getTypeKind() != ARRAY_TYPE) {
    reportErrorArrayType(arrayExpressionType->getTypeName());
    exit(1);
  }
  
  if (arrayIndexExpressionType != PrimitiveTypes::INT_TYPE &&
      arrayIndexExpressionType != PrimitiveTypes::LONG_TYPE) {
    Log::e("Array index should be integer type, but it is " +
           arrayIndexExpressionType->getTypeName());
    exit(1);
  }
  
  Value* arrayExpressionValue = mArrayExpression->generateIR(context, flag);
  Value* arrayIndexValue = mArrayIndexExpresion->generateIR(context, flag);
  Type* indexType = arrayIndexExpressionType->getLLVMType(context);
  Value* index[2];
  index[0] = ConstantInt::get(indexType, 0);
  index[1] = arrayIndexValue;
  Value* pointer = IRWriter::createGetElementPtrInst(context, arrayExpressionValue, index);
  const wisey::ArrayType* arrayType = (const wisey::ArrayType*) arrayExpressionType;
  if (arrayType->getBaseType()->getTypeKind() == PRIMITIVE_TYPE) {
    return IRWriter::newLoadInst(context, pointer, "");
  }
  return pointer;
}

IVariable* ArrayElementExpression::getVariable(IRGenerationContext& context) const {
  return NULL;
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
