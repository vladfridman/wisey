//
//  InterfaceOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Environment.hpp"
#include "wisey/InterfaceOwner.hpp"
#include "wisey/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

InterfaceOwner::InterfaceOwner(Interface* interface) : mInterface(interface) { }

InterfaceOwner::~InterfaceOwner() { }

Interface* InterfaceOwner::getObject() const {
  return mInterface;
}

string InterfaceOwner::getTypeName() const {
  return mInterface->getTypeName() + '*';
}

PointerType* InterfaceOwner::getLLVMType(IRGenerationContext& context) const {
  return mInterface->getLLVMType(context);
}

TypeKind InterfaceOwner::getTypeKind() const {
  return INTERFACE_OWNER_TYPE;
}

bool InterfaceOwner::canCastTo(const IType* toType) const {
  if (toType == this || toType == mInterface) {
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
                              const IType* toType,
                              int line) const {
  if (toType == this || toType == mInterface) {
    return fromValue;
  }

  if (IType::isOwnerType(toType)) {
    return mInterface->castTo(context, fromValue, ((IObjectOwnerType*) toType)->getObject(), line);
  }
  
  return mInterface->castTo(context, fromValue, toType, line);
}

void InterfaceOwner::free(IRGenerationContext& context, Value* value) const {
  Function* destructor = getDestructorFunction(context);

  vector<Value*> arguments;
  arguments.push_back(value);

  IRWriter::createCallInst(context, destructor, arguments, "");
}

Function* InterfaceOwner::getDestructorFunction(IRGenerationContext& context) const {
  Function* destructor = context.getModule()->getFunction(mInterface->getDestructorFunctionName());
  
  return destructor ? destructor : mInterface->defineDestructorFunction(context);
}
