//
//  InterfaceOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/InterfaceOwner.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Interface* InterfaceOwner::getObject() const {
  return mInterface;
}

string InterfaceOwner::getName() const {
  return mInterface->getName() + '*';
}

Type* InterfaceOwner::getLLVMType(LLVMContext& llvmContext) const {
  return mInterface->getLLVMType(llvmContext);
}

TypeKind InterfaceOwner::getTypeKind() const {
  return INTERFACE_OWNER_TYPE;
}

bool InterfaceOwner::canCastTo(const IType* toType) const {
  if (toType == this) {
    return true;
  }
  
  TypeKind typeKind = toType->getTypeKind();
  if (typeKind == INTERFACE_OWNER_TYPE ||
      typeKind == CONTROLLER_OWNER_TYPE ||
      typeKind == MODEL_OWNER_TYPE) {
    return mInterface->canCastTo(((IObjectOwnerType*) toType)->getObject());
  }
  
  return mInterface->canCastTo(toType);
}

bool InterfaceOwner::canAutoCastTo(const IType* toType) const {
  return canCastTo(toType);
}

Value* InterfaceOwner::castTo(IRGenerationContext& context,
                              Value* fromValue,
                              const IType* toType) const {
  if (toType == this) {
    return fromValue;
  }
  
  TypeKind typeKind = toType->getTypeKind();
  if (typeKind == INTERFACE_OWNER_TYPE ||
      typeKind == CONTROLLER_OWNER_TYPE ||
      typeKind == MODEL_OWNER_TYPE) {
    return mInterface->castTo(context, fromValue, ((IObjectOwnerType*) toType)->getObject());
  }
  
  return mInterface->castTo(context, fromValue, toType);
}
