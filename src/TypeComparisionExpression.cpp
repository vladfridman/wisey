//
//  TypeComparisionExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/Log.hpp"
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
  
  Value* interfaceIndex = interface->callInstanceOf(context, expressionValue, callableObjectType);
  ConstantInt* zero = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
  BasicBlock* basicBlock = context.getBasicBlock();
  
  return new ICmpInst(*basicBlock, ICmpInst::ICMP_SGE, interfaceIndex, zero, "instanceof");
}

IType* TypeComparisionExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}

void TypeComparisionExpression::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of an instanceof operation result, it is not a heap pointer");
  exit(1);
}
