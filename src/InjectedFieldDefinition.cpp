//
//  InjectedFieldDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/InjectedField.hpp"
#include "wisey/InjectedFieldDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

InjectedFieldDefinition::InjectedFieldDefinition(const ITypeSpecifier* typeSpecifier,
                                                   string name,
                                                   InjectionArgumentList injectionArguments) :
mTypeSpecifier(typeSpecifier),
mName(name),
mInjectionArgumentList(injectionArguments) { }

InjectedFieldDefinition::~InjectedFieldDefinition() {
  delete mTypeSpecifier;
  for (InjectionArgument* injectionArgument : mInjectionArgumentList) {
    delete injectionArgument;
  }
  mInjectionArgumentList.clear();
}

IField* InjectedFieldDefinition::define(IRGenerationContext& context,
                                         const IObjectType* objectType) const {
  const IType* fieldType = mTypeSpecifier->getType(context);
  
  if (fieldType->isInterface() && fieldType->isOwner()) {
    Interface* interface = (Interface*) fieldType->getObjectType();
    fieldType = context.getBoundController(interface)->getOwner();
  }
  
  const IType* injectedType = NULL;
  if (fieldType->isArray() && fieldType->isOwner()) {
    injectedType = fieldType;
    fieldType = ((const ArraySpecificOwnerType*) fieldType)->getArraySpecificType()
    ->getArrayType(context)->getOwner();
  }
  
  return new InjectedField(fieldType, injectedType, mName, mInjectionArgumentList);
}

bool InjectedFieldDefinition::isConstant() const {
  return false;
}

bool InjectedFieldDefinition::isField() const {
  return true;
}

bool InjectedFieldDefinition::isMethod() const {
  return false;
}

bool InjectedFieldDefinition::isStaticMethod() const {
  return false;
}

bool InjectedFieldDefinition::isMethodSignature() const {
  return false;
}
