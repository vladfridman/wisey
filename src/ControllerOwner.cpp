//
//  ControllerOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "ControllerOwner.hpp"
#include "FieldOwnerVariable.hpp"
#include "IRWriter.hpp"
#include "LocalOwnerVariable.hpp"
#include "ParameterOwnerVariable.hpp"

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

void ControllerOwner::free(IRGenerationContext& context,
                           Value* value,
                           Value* exception,
                           const LLVMFunction* customDestructor,
                           int line) const {
  IConcreteObjectType::composeDestructorCall(context, mController, value, exception, line);
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

bool ControllerOwner::isNative() const {
  return false;
}

bool ControllerOwner::isPointer() const {
  return false;
}

bool ControllerOwner::isImmutable() const {
  return false;
}

void ControllerOwner::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void ControllerOwner::createLocalVariable(IRGenerationContext& context,
                                          string name,
                                          int line) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalOwnerVariable(name, this, alloca, line);
  context.getScopes().setVariable(context, uninitializedVariable);
}

void ControllerOwner::createFieldVariable(IRGenerationContext& context,
                                          string name,
                                          const IConcreteObjectType* object,
                                          int line) const {
  IVariable* variable = new FieldOwnerVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void ControllerOwner::createParameterVariable(IRGenerationContext& context,
                                              string name,
                                              Value* value,
                                              int line) const {
  Type* llvmType = getLLVMType(context);
  Value* alloc = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterOwnerVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* ControllerOwner::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

Instruction* ControllerOwner::inject(IRGenerationContext &context,
                                     const InjectionArgumentList injectionArgumentList,
                                     int line) const {
  return mController->inject(context, injectionArgumentList, line);
}
