//
//  FieldDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/Field.hpp"
#include "wisey/FieldDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

FieldDeclaration::FieldDeclaration(FieldKind fieldKind,
                                   ITypeSpecifier* typeSpecifier,
                                   string name,
                                   InjectionArgumentList injectionArguments) :
mFieldKind(fieldKind),
mTypeSpecifier(typeSpecifier),
mName(name),
mInjectionArgumentList(injectionArguments) { }

FieldDeclaration::~FieldDeclaration() {
  delete mTypeSpecifier;
  for (InjectionArgument* injectionArgument : mInjectionArgumentList) {
    delete injectionArgument;
  }
  mInjectionArgumentList.clear();
}

Field* FieldDeclaration::declare(IRGenerationContext& context,
                                 const IObjectType* objectType) const {
  const IType* fieldType = mTypeSpecifier->getType(context);
  
  if (mFieldKind == INJECTED_FIELD && fieldType->isInterface() && fieldType->isOwner()) {
    Interface* interface = (Interface*) fieldType->getObjectType();
    fieldType = context.getBoundController(interface)->getOwner();
  }
  
  const IType* injectedType = NULL;
  if (mFieldKind == INJECTED_FIELD && fieldType->isArray()) {
    injectedType = fieldType;
    fieldType = ((const ArraySpecificOwnerType*) fieldType)->getArraySpecificType()
    ->getArrayType(context)->getOwner();
  }
  
  return new Field(mFieldKind, fieldType, injectedType, mName, mInjectionArgumentList);
}

