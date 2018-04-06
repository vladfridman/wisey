//
//  ControllerOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ControllerOwner.hpp"
#include "wisey/FieldOwnerVariable.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/ParameterOwnerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ControllerOwner::ControllerOwner(Controller* controller) :
mController(controller) {
}

ControllerOwner::~ControllerOwner() {
}

Controller* ControllerOwner::getReference() const {
  return mController;
}

string ControllerOwner::getTypeName() const {
  return mController->getTypeName() + '*';
}

llvm::PointerType* ControllerOwner::getLLVMType(IRGenerationContext& context) const {
  return mController->getLLVMType(context);
}

bool ControllerOwner::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this || toType == mController) {
    return true;
  }
  
  if (toType->isArray()) {
    return false;
  }
  
  if (toType->isOwner()) {
    return mController->canCastTo(context, ((const IOwnerType*) toType)->getReference());
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

  if (toType->isOwner()) {
    const IReferenceType* toReferenceType = ((const IOwnerType*) toType)->getReference();
    return mController->castTo(context, fromValue, toReferenceType, line);
  }
  
  return mController->castTo(context, fromValue, toType, line);
}

void ControllerOwner::free(IRGenerationContext& context, Value* value, int line) const {
  IConcreteObjectType::composeDestructorCall(context, value);
}

Function* ControllerOwner::getDestructorFunction(IRGenerationContext& context, int line) const {
  return IConcreteObjectType::getDestructorFunctionForObject(context, getReference(), line);
}

bool ControllerOwner::isPrimitive() const {
  return false;
}

bool ControllerOwner::isOwner() const {
  return true;
}

bool ControllerOwner::isReference() const {
  return false;
}

bool ControllerOwner::isArray() const {
  return false;
}

bool ControllerOwner::isFunction() const {
  return false;
}

bool ControllerOwner::isPackage() const {
  return false;
}

bool ControllerOwner::isController() const {
  return true;
}

bool ControllerOwner::isInterface() const {
  return false;
}

bool ControllerOwner::isModel() const {
  return false;
}

bool ControllerOwner::isNode() const {
  return false;
}

bool ControllerOwner::isThread() const {
  return false;
}

bool ControllerOwner::isNative() const {
  return false;
}

bool ControllerOwner::isPointer() const {
  return false;
}

void ControllerOwner::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void ControllerOwner::createLocalVariable(IRGenerationContext& context, string name) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "ownerDeclaration");
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalOwnerVariable(name, this, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

void ControllerOwner::createFieldVariable(IRGenerationContext& context,
                                          string name,
                                          const IConcreteObjectType* object) const {
  IVariable* variable = new FieldOwnerVariable(name, object);
  context.getScopes().setVariable(variable);
}

void ControllerOwner::createParameterVariable(IRGenerationContext& context,
                                              string name,
                                              Value* value) const {
  Type* llvmType = getLLVMType(context);
  Value* alloc = IRWriter::newAllocaInst(context, llvmType, "parameterObjectPointer");
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterOwnerVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* ControllerOwner::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}
