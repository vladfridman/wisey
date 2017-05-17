//
//  TypeComparisionExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/TypeComparisionExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

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
    Model* model = dynamic_cast<Model*>(expressionType);
    return model->doesImplmentInterface(dynamic_cast<Interface*>(type)) ? valueTrue : valueFalse;
  }
  Interface* interface = dynamic_cast<Interface*>(expressionType);
  if (type->getTypeKind() == INTERFACE_TYPE &&
      interface->doesExtendInterface(dynamic_cast<Interface*>(type))) {
    return valueTrue;
  }
  return checkInterfaceImplemented(context);
}

Value* TypeComparisionExpression::checkInterfaceImplemented(IRGenerationContext& context) const {
  Value* expressionValue = mExpression.generateIR(context);
  IType* expressionType = mExpression.getType(context);
  Interface* interface = dynamic_cast<Interface*>(expressionType);
  IObjectWithMethodsType* objectWithMethodsType =
    dynamic_cast<IObjectWithMethodsType*>(mTypeSpecifier.getType(context));
  
  Value* interfaceIndex =
    interface->callInstanceOf(context, expressionValue, objectWithMethodsType);
  ConstantInt* zero = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
  
  return IRWriter::newICmpInst(context, ICmpInst::ICMP_SGE, interfaceIndex, zero, "instanceof");
}

IType* TypeComparisionExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}

void TypeComparisionExpression::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of an instanceof operation result, it is not a heap pointer");
  exit(1);
}
