//
//  TypeComparisionExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/TypeComparisionExpression.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* TypeComparisionExpression::generateIR(IRGenerationContext& context) const {
  IType* expressionType = mExpression.getType(context);
  IType* type = mTypeSpecifier.getType(context);
  LLVMContext& llvmContext = context.getLLVMContext();
  ConstantInt* valueTrue = ConstantInt::get(Type::getInt1Ty(llvmContext), 1);
  ConstantInt* valueFalse = ConstantInt::get(Type::getInt1Ty(llvmContext), 0);
  
  if (type == expressionType) {
    return valueTrue;
  }
  if (type->getTypeKind() == PRIMITIVE_TYPE || expressionType->getTypeKind() == PRIMITIVE_TYPE) {
    return valueFalse;
  }
  if (expressionType->getTypeKind() == MODEL_TYPE && type->getTypeKind() == MODEL_TYPE) {
    return valueFalse;
  }
  if (expressionType->getTypeKind() == MODEL_TYPE && type->getTypeKind() == INTERFACE_TYPE) {
    Model* model = (Model*) expressionType;
    return model->doesImplmentInterface((Interface*) type) ? valueTrue : valueFalse;
  }
  Interface* interface = (Interface*) expressionType;
  if (type->getTypeKind() == INTERFACE_TYPE &&
      interface->doesExtendInterface((Interface*) type)) {
    return valueTrue;
  }
  return checkInterfaceImplemented(context);
}

Value* TypeComparisionExpression::checkInterfaceImplemented(IRGenerationContext& context) const {
  Value* expressionValue = mExpression.generateIR(context);
  IType* expressionType = mExpression.getType(context);
  Interface* interface = (Interface*) expressionType;
  ICallableObjectType* callableObjectType = (ICallableObjectType*) mTypeSpecifier.getType(context);
  
  return interface->callInstanceOf(context, expressionValue, callableObjectType);
}

IType* TypeComparisionExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}
