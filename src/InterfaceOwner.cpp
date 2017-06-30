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
  return mInterface->getLLVMType(llvmContext);
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
  
  Value* object = IRWriter::newLoadInst(context, fromValue, "interfaceObject");
  
  if (!IType::isOwnerType(toType)) {
    return mInterface->castTo(context, object, toType);
  }
  
  Value* cast = mInterface->castTo(context, object, ((IObjectOwnerType*) toType)->getObject());
  Value* pointer = IRWriter::newAllocaInst(context, cast->getType(), "castedInterface");
  IRWriter::newStoreInst(context, cast, pointer);
  
  return pointer;
}
