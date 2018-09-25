//
//  TypeComparisionExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "InstanceOfFunction.hpp"
#include "IRWriter.hpp"
#include "Log.hpp"
#include "PrimitiveTypes.hpp"
#include "TypeComparisionExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

TypeComparisionExpression::TypeComparisionExpression(const IExpression* expression,
                                                     const ITypeSpecifier* typeSpecifier,
                                                     int line) :
mExpression(expression), mTypeSpecifier(typeSpecifier), mLine(line) { }

TypeComparisionExpression::~TypeComparisionExpression() {
  delete mExpression;
  delete mTypeSpecifier;
}

int TypeComparisionExpression::getLine() const {
  return mLine;
}

Value* TypeComparisionExpression::generateIR(IRGenerationContext& context,
                                             const IType* assignToType) const {
  const IType* expressionType = mExpression->getType(context);
  const IType* type = mTypeSpecifier->getType(context);
  LLVMContext& llvmContext = context.getLLVMContext();
  ConstantInt* valueFalse = ConstantInt::get(Type::getInt1Ty(llvmContext), 0);
  
  if (type->isPrimitive() || expressionType->isPrimitive()) {
    return valueFalse;
  }
  if (type->isArray() || expressionType->isArray()) {
    context.reportError(mLine, "Operator instanceof does not work with arrays");
    throw 1;
  }
  if (!expressionType->getLLVMType(context)->isPointerTy()) {
    context.reportError(mLine, "instanceof is not supported with type " +
                        expressionType->getTypeName());
    throw 1;
  }
  if (expressionType->isOwner() && type->isOwner()) {
    return generateIRforOwnerTypes(context,
                                   (const IObjectOwnerType*) expressionType,
                                   (const IObjectOwnerType*) type);
  }
  if (expressionType->isReference() && type->isReference()) {
    return generateIRforReferenceTypes(context,
                                       (const IObjectType*) expressionType,
                                       (const IObjectType*) type);
  }
  return valueFalse;
}

Value* TypeComparisionExpression::generateIRforOwnerTypes(IRGenerationContext& context,
                                                          const IObjectOwnerType* expressionType,
                                                          const IObjectOwnerType* type) const {
  return generateIRforReferenceTypes(context,
                                     (const IObjectType*) expressionType->getReference(),
                                     (const IObjectType*) type->getReference());
}

Value* TypeComparisionExpression::generateIRforReferenceTypes(IRGenerationContext& context,
                                                              const IObjectType* expressionType,
                                                              const IObjectType* type) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  ConstantInt* valueTrue = ConstantInt::get(Type::getInt1Ty(llvmContext), 1);
  ConstantInt* valueFalse = ConstantInt::get(Type::getInt1Ty(llvmContext), 0);
  
  if (IType::isConcreteObjectType(expressionType) && IType::isConcreteObjectType(type)) {
    return valueFalse;
  }
  if (IType::isConcreteObjectType(expressionType) && type->isInterface()) {
    const IConcreteObjectType* concreteObjectType = (const IConcreteObjectType*) expressionType;
    return IConcreteObjectType::getInterfaceIndex(concreteObjectType, (const Interface*) type) >= 0
    ? valueTrue : valueFalse;
  }
  const Interface* interface = (const Interface*) expressionType;
  if (type->isInterface() && interface->doesExtendInterface((const Interface*) type)) {
    return valueTrue;
  }
  return checkInterfaceImplemented(context, type);
}

Value* TypeComparisionExpression::checkInterfaceImplemented(IRGenerationContext& context,
                                                            const IObjectType* objectType) const {
  Value* expressionValue = mExpression->generateIR(context, PrimitiveTypes::VOID);
  
  llvm::Constant* namePointer = IObjectType::getObjectNamePointer(objectType, context);
  Value* interfaceIndex = InstanceOfFunction::call(context, expressionValue, namePointer);
  ConstantInt* zero = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
  
  return IRWriter::newICmpInst(context, ICmpInst::ICMP_SGE, interfaceIndex, zero, "instanceof");
}

const IType* TypeComparisionExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN;
}

bool TypeComparisionExpression::isConstant() const {
  return false;
}

bool TypeComparisionExpression::isAssignable() const {
  return false;
}

void TypeComparisionExpression::printToStream(IRGenerationContext& context,
                                              std::iostream& stream) const {
  mExpression->printToStream(context, stream);
  stream << " instanceof ";
  mTypeSpecifier->printToStream(context, stream);
}

