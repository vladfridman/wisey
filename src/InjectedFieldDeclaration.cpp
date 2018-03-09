//
//  InjectedFieldDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/InjectedFieldDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

InjectedFieldDeclaration::InjectedFieldDeclaration(ITypeSpecifier* typeSpecifier,
                                                   string name,
                                                   InjectionArgumentList injectionArguments) :
mTypeSpecifier(typeSpecifier),
mName(name),
mInjectionArgumentList(injectionArguments) { }

InjectedFieldDeclaration::~InjectedFieldDeclaration() {
  delete mTypeSpecifier;
  for (InjectionArgument* injectionArgument : mInjectionArgumentList) {
    delete injectionArgument;
  }
  mInjectionArgumentList.clear();
}

Field* InjectedFieldDeclaration::declare(IRGenerationContext& context,
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
  
  return new Field(INJECTED_FIELD, fieldType, injectedType, mName, mInjectionArgumentList);
}

bool InjectedFieldDeclaration::isConstant() const {
  return false;
}

bool InjectedFieldDeclaration::isField() const {
  return true;
}

bool InjectedFieldDeclaration::isMethod() const {
  return false;
}

bool InjectedFieldDeclaration::isStaticMethod() const {
  return false;
}

bool InjectedFieldDeclaration::isMethodSignature() const {
  return false;
}
