//
//  ControllerOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ControllerOwner.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Controller* ControllerOwner::getObject() const {
  return mController;
}

string ControllerOwner::getName() const {
  return mController->getName() + '*';
}

PointerType* ControllerOwner::getLLVMType(LLVMContext& llvmContext) const {
  return mController->getLLVMType(llvmContext);
}

TypeKind ControllerOwner::getTypeKind() const {
  return CONTROLLER_OWNER_TYPE;
}

bool ControllerOwner::canCastTo(const IType* toType) const {
  if (toType == this) {
    return true;
  }
  
  if (IType::isOwnerType(toType)) {
    return mController->canCastTo(((IObjectOwnerType*) toType)->getObject());
  }
  
  return mController->canCastTo(toType);
}

bool ControllerOwner::canAutoCastTo(const IType* toType) const {
  return canCastTo(toType);
}

Value* ControllerOwner::castTo(IRGenerationContext& context,
                               Value* fromValue,
                               const IType* toType) const {
  if (toType == this) {
    return fromValue;
  }
  
  if (IType::isOwnerType(toType)) {
    return mController->castTo(context, fromValue, ((IObjectOwnerType*) toType)->getObject());
  }
  
  return mController->castTo(context, fromValue, toType);
}
