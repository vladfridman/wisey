//
//  TypeComparisionExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/InstanceOf.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/TypeComparisionExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

TypeComparisionExpression::~TypeComparisionExpression() {
  delete mExpression;
  delete mTypeSpecifier;
}

Value* TypeComparisionExpression::generateIR(IRGenerationContext& context) const {
  const IType* expressionType = mExpression->getType(context);
  const IType* type = mTypeSpecifier->getType(context);
  LLVMContext& llvmContext = context.getLLVMContext();
  ConstantInt* valueTrue = ConstantInt::get(Type::getInt1Ty(llvmContext), 1);
  ConstantInt* valueFalse = ConstantInt::get(Type::getInt1Ty(llvmContext), 0);
  
  if (IType::isOwnerType(expressionType)) {
    expressionType = ((IObjectOwnerType*) expressionType)->getObject();
  }
  
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
  if (type->getTypeKind() == INTERFACE_TYPE && interface->doesExtendInterface((Interface*) type)) {
    return valueTrue;
  }
  return checkInterfaceImplemented(context);
}

Value* TypeComparisionExpression::checkInterfaceImplemented(IRGenerationContext& context) const {
  Value* expressionValue = mExpression->generateIR(context);
  const IType* expressionType = mExpression->getType(context);
  Interface* interface = (Interface*) expressionType;
  IObjectType* objectWithMethodsType = (IObjectType*) mTypeSpecifier->getType(context);
  
  Value* interfaceIndex = InstanceOf::call(context,
                                           interface,
                                           expressionValue,
                                           objectWithMethodsType);
  ConstantInt* zero = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
  
  return IRWriter::newICmpInst(context, ICmpInst::ICMP_SGE, interfaceIndex, zero, "instanceof");
}

const IType* TypeComparisionExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}

void TypeComparisionExpression::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of an instanceof operation result, it is not a heap pointer");
  exit(1);
}

void TypeComparisionExpression::addReferenceToOwner(IRGenerationContext& context,
                                                    IVariable* reference) const {
  Log::e("Can not add a reference to an instanceof expression");
  exit(1);
}

bool TypeComparisionExpression::existsInOuterScope(IRGenerationContext& context) const {
  return false;
}
