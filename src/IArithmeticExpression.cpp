//
//  IArithmeticExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/AutoCast.hpp"
#include "wisey/IArithmeticExpression.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

void IArithmeticExpression::checkTypes(IRGenerationContext& context,
                                       const IType* leftType,
                                       const IType* rightType,
                                       int operation,
                                       int line) {
  if (leftType == PrimitiveTypes::VOID || rightType == PrimitiveTypes::VOID) {
    context.reportError(line, "Can not use expressions of type VOID in a '" +
                        string(1, operation) + "' operation");
    throw 1;
  }
  
  if (isPointerArithmetic(leftType, rightType, operation)) {
    return;
  }
  
  if (operation == '+' && StringType::isStringVariation(context, leftType, line) &&
      rightType->isPrimitive()) {
    return;
  }
  
  if (operation == '+' && StringType::isStringVariation(context, rightType, line) &&
      leftType->isPrimitive()) {
    return;
  }
  
  if (operation == '+' && StringType::isStringVariation(context, leftType, line) &&
      StringType::isStringVariation(context, rightType, line)) {
    return;
  }
  
  if (!leftType->isPrimitive()  || !rightType->isPrimitive()) {
    context.reportError(line, "Can not do operation '" + string(1, operation) +
                        "' on non-primitive types");
    throw 1;
  }
  
  if (!leftType->canCastTo(context, rightType) && !rightType->canCastTo(context, leftType)) {
    context.reportError(line, "Incompatible types in '" + string(1, operation) + "' operation");
    throw 1;
  }
  
  if (!leftType->canAutoCastTo(context, rightType) && !rightType->canAutoCastTo(context, leftType)) {
    context.reportError(line, "Incompatible types in '" + string(1, operation) +
                        "' operation that require an explicit cast");
    throw 1;
  }
}

bool IArithmeticExpression::isPointerArithmetic(const IType* leftType,
                                                const IType* rightType,
                                                int operation) {
  if (leftType->isPointer() &&
      (rightType == PrimitiveTypes::INT || rightType == PrimitiveTypes::LONG)) {
    return operation == '+' || operation == '-' || operation == '%';
  }
  if (leftType->isPointer() && rightType == leftType && operation == '-') {
    return true;
  }
  
  return false;
}
