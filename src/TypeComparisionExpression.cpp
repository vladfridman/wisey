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
                                   (const IObjectType*) expressionType->getReference(),
                                   (const IObjectType*) type->getReference());
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
    const IConcreteObjectType* concreteObjectType = (const IConcreteObjectType*) expressionType;
    return IConcreteObjectType::getInterfaceIndex(concreteObjectType, (const Interface*) type) >= 0
    ? valueTrue : valueFalse;
  }
  const Interface* interface = (const Interface*) expressionType;
  if (type->isInterface() && interface->doesExtendInterface((const Interface*) type)) {
    return valueTrue;
  }
  return checkInterfaceImplemented(context, expressionType, type);
}

Value* TypeComparisionExpression::checkInterfaceImplemented(IRGenerationContext& context,
                                                            const IType* expressionType,
                                                            const IObjectType* objectType) const {
  Value* expressionValue = mExpression->generateIR(context, PrimitiveTypes::VOID_TYPE);
  const Interface* interface = (const Interface*) expressionType;
  
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

