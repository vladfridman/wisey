//
//  ControllerOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ControllerOwner.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ControllerOwner::ControllerOwner(Controller* controller) : mController(controller) { }

ControllerOwner::~ControllerOwner() { }

Controller* ControllerOwner::getObject() const {
  return mController;
}

string ControllerOwner::getTypeName() const {
  return mController->getTypeName() + '*';
}

PointerType* ControllerOwner::getLLVMType(IRGenerationContext& context) const {
  return mController->getLLVMType(context);
}

TypeKind ControllerOwner::getTypeKind() const {
  return CONTROLLER_OWNER_TYPE;
}

bool ControllerOwner::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this || toType == mController) {
    return true;
  }
  
  if (IType::isOwnerType(toType)) {
    return mController->canCastTo(context, ((IObjectOwnerType*) toType)->getObject());
  }
  
  return mController->canCastTo(context, toType);
}

bool ControllerOwner::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

Value* ControllerOwner::castTo(IRGenerationContext& context,
                               Value* fromValue,
                               const IType* toType,
                               int line) const {
  if (toType == this || toType == mController) {
    return fromValue;
  }

  if (IType::isOwnerType(toType)) {
    return mController->castTo(context, fromValue, ((IObjectOwnerType*) toType)->getObject(), line);
  }
  
  return mController->castTo(context, fromValue, toType, line);
}

void ControllerOwner::free(IRGenerationContext& context, Value* value) const {
  IConcreteObjectType::composeDestructorCall(context, getObject(), value);
}

Function* ControllerOwner::getDestructorFunction(IRGenerationContext& context) const {
  return IConcreteObjectType::getDestructorFunctionForObject(context, getObject());
}

bool ControllerOwner::isOwner() const {
  return true;
}

void ControllerOwner::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void ControllerOwner::allocateVariable(IRGenerationContext& context, string name) const {
  PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "ownerDeclaration");
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalOwnerVariable(name, this, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

const wisey::ArrayType* ControllerOwner::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

