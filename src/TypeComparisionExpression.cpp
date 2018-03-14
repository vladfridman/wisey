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

TypeComparisionExpression::TypeComparisionExpression(IExpression* expression,
                                                     ITypeSpecifier* typeSpecifier) :
mExpression(expression), mTypeSpecifier(typeSpecifier) { }

TypeComparisionExpression::~TypeComparisionExpression() {
  delete mExpression;
  delete mTypeSpecifier;
}

IVariable* TypeComparisionExpression::getVariable(IRGenerationContext& context,
                                                  vector<const IExpression*>& arrayIndices) const {
  return NULL;
}

Value* TypeComparisionExpression::generateIR(IRGenerationContext& context,
                                             const IType* assignToType) const {
  const IType* expressionType = mExpression->getType(context);
  const IType* type = mTypeSpecifier->getType(context);
  LLVMContext& llvmContext = context.getLLVMContext();
  ConstantInt* valueTrue = ConstantInt::get(Type::getInt1Ty(llvmContext), 1);
  ConstantInt* valueFalse = ConstantInt::get(Type::getInt1Ty(llvmContext), 0);
  
  if (type == expressionType) {
    return valueTrue;
  }
  if (type->isPrimitive() || expressionType->isPrimitive()) {
    return valueFalse;
  }
  if (expressionType->isOwner() && type->isOwner()) {
    return generateIRforOwnerTypes(context,
                                   (const IObjectOwnerType*) expressionType,
                                   (const IObjectOwnerType*) type);
  }
  if (expressionType->isReference() && type->isReference()) {
    return generateIRforPointerTypes(context,
                                       (const IObjectType*) expressionType,
                                       (const IObjectType*) type);
  }
  return valueFalse;
}

Value* TypeComparisionExpression::generateIRforOwnerTypes(IRGenerationContext& context,
                                                          const IObjectOwnerType* expressionType,
                                                          const IObjectOwnerType* type) const {
  return generateIRforPointerTypes(context,
                                     expressionType->getObjectType(),
                                     type->getObjectType());
}

Value* TypeComparisionExpression::generateIRforPointerTypes(IRGenerationContext& context,
                                                              const IObjectType* expressionType,
                                                              const IObjectType* type) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  ConstantInt* valueTrue = ConstantInt::get(Type::getInt1Ty(llvmContext), 1);
  ConstantInt* valueFalse = ConstantInt::get(Type::getInt1Ty(llvmContext), 0);
  
  if (IType::isConcreteObjectType(expressionType) && IType::isConcreteObjectType(type)) {
    return valueFalse;
  }
  if (IType::isConcreteObjectType(expressionType) && type->isInterface()) {
    IConcreteObjectType* concreteObjectType = (IConcreteObjectType*) expressionType;
    return IConcreteObjectType::getInterfaceIndex(concreteObjectType, (Interface*) type) >= 0
    ? valueTrue : valueFalse;
  }
  Interface* interface = (Interface*) expressionType;
  if (type->isInterface() && interface->doesExtendInterface((Interface*) type)) {
    return valueTrue;
  }
  return checkInterfaceImplemented(context, expressionType, type);
}

Value* TypeComparisionExpression::checkInterfaceImplemented(IRGenerationContext& context,
                                                            const IType* expressionType,
                                                            const IObjectType* objectType) const {
  Value* expressionValue = mExpression->generateIR(context, PrimitiveTypes::VOID_TYPE);
  Interface* interface = (Interface*) expressionType;
  
  Value* interfaceIndex = InstanceOf::call(context, interface, expressionValue, objectType);
  ConstantInt* zero = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
  
  return IRWriter::newICmpInst(context, ICmpInst::ICMP_SGE, interfaceIndex, zero, "instanceof");
}

const IType* TypeComparisionExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}

bool TypeComparisionExpression::isConstant() const {
  return false;
}

void TypeComparisionExpression::printToStream(IRGenerationContext& context,
                                              std::iostream& stream) const {
  mExpression->printToStream(context, stream);
  stream << " instanceof ";
  mTypeSpecifier->printToStream(context, stream);
}

