//
//  InterfaceOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/InterfaceOwner.hpp"
#include "wisey/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Interface* InterfaceOwner::getObject() const {
  return mInterface;
}

string InterfaceOwner::getName() const {
  return mInterface->getName() + '*';
}

PointerType* InterfaceOwner::getLLVMType(LLVMContext& llvmContext) const {
  return (PointerType*) mInterface->getLLVMType(llvmContext)->getPointerElementType();
}

TypeKind InterfaceOwner::getTypeKind() const {
  return INTERFACE_OWNER_TYPE;
}

bool InterfaceOwner::canCastTo(const IType* toType) const {
  if (toType == this) {
    return true;
  }
  
  if (IType::isOwnerType(toType)) {
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

  if (IType::isOwnerType(toType)) {
    return mInterface->castTo(context, fromValue, ((IObjectOwnerType*) toType)->getObject());
  }
  
  return mInterface->castTo(context, fromValue, toType);
}

void InterfaceOwner::free(IRGenerationContext &context, Value *value) const {
  Value* thisPointer = Interface::getOriginalObject(context, value);
  
  IRWriter::createFree(context, thisPointer);
}
