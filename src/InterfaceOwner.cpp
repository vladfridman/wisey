//
//  InterfaceOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/DestroyObjectOwnerFunction.hpp"
#include "wisey/Environment.hpp"
#include "wisey/FieldOwnerVariable.hpp"
#include "wisey/InterfaceOwner.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/ParameterOwnerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

InterfaceOwner::InterfaceOwner(Interface* interface) :
mInterface(interface) {
}

InterfaceOwner::~InterfaceOwner() {
}

Interface* InterfaceOwner::getReference() const {
  return mInterface;
}

string InterfaceOwner::getTypeName() const {
  return mInterface->getTypeName() + '*';
}

llvm::PointerType* InterfaceOwner::getLLVMType(IRGenerationContext& context) const {
  return mInterface->getLLVMType(context);
}

bool InterfaceOwner::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this || toType == mInterface) {
    return true;
  }
  
  if (toType->isArray()) {
    return false;
  }
  
  if (toType->isOwner()) {
    return mInterface->canCastTo(context, ((const IOwnerType*) toType)->getReference());
  }
  
  return mInterface->canCastTo(context, toType);
}

bool InterfaceOwner::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

Value* InterfaceOwner::castTo(IRGenerationContext& context,
                              Value* fromValue,
                              const IType* toType,
                              int line) const {
  if (toType == this || toType == mInterface) {
    return fromValue;
  }

  if (toType->isOwner()) {
    const IReferenceType* toReferenceType = ((const IOwnerType*) toType)->getReference();
    return mInterface->castTo(context, fromValue, toReferenceType, line);
  }
  
  return mInterface->castTo(context, fromValue, toType, line);
}

void InterfaceOwner::free(IRGenerationContext& context,
                          Value* value,
                          Value* exception,
                          int line) const {
  Type* int8pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* bitcast = IRWriter::newBitCastInst(context, value, int8pointer);

  Function* destructor = getDestructorFunction(context, line);
  vector<Value*> arguments;
  arguments.push_back(bitcast);
  arguments.push_back(exception);

  IRWriter::createCallInst(context, destructor, arguments, "");
}

Function* InterfaceOwner::getDestructorFunction(IRGenerationContext& context, int line) const {
  return DestroyObjectOwnerFunction::get(context);
}

bool InterfaceOwner::isPrimitive() const {
  return false;
}

bool InterfaceOwner::isOwner() const {
  return true;
}

bool InterfaceOwner::isReference() const {
  return false;
}

bool InterfaceOwner::isArray() const {
  return false;
}

bool InterfaceOwner::isFunction() const {
  return false;
}

bool InterfaceOwner::isPackage() const {
  return false;
}

bool InterfaceOwner::isController() const {
  return false;
}

bool InterfaceOwner::isInterface() const {
  return true;
}

bool InterfaceOwner::isModel() const {
  return false;
}

bool InterfaceOwner::isNode() const {
  return false;
}

bool InterfaceOwner::isNative() const {
  return false;
}

bool InterfaceOwner::isPointer() const {
  return false;
}

bool InterfaceOwner::isImmutable() const {
  return false;
}

void InterfaceOwner::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void InterfaceOwner::createLocalVariable(IRGenerationContext& context,
                                         string name,
                                         int line) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalOwnerVariable(name, this, alloca, line);
  context.getScopes().setVariable(context, uninitializedVariable);
}

void InterfaceOwner::createFieldVariable(IRGenerationContext& context,
                                         string name,
                                         const IConcreteObjectType* object,
                                         int line) const {
  IVariable* variable = new FieldOwnerVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void InterfaceOwner::createParameterVariable(IRGenerationContext& context,
                                             string name,
                                             Value* value,
                                             int line) const {
  Type* llvmType = getLLVMType(context);
  Value* alloc = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterOwnerVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* InterfaceOwner::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

Value* InterfaceOwner::inject(IRGenerationContext &context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const {
  return mInterface->inject(context, injectionArgumentList, line);
}
