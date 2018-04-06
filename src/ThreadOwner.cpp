//
//  ThreadOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/FieldOwnerVariable.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/ParameterOwnerVariable.hpp"
#include "wisey/ThreadOwner.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ThreadOwner::ThreadOwner(Thread* thread) : mThread(thread) { }

ThreadOwner::~ThreadOwner() {
}

Thread* ThreadOwner::getReference() const {
  return mThread;
}

string ThreadOwner::getTypeName() const {
  return mThread->getTypeName() + '*';
}

llvm::PointerType* ThreadOwner::getLLVMType(IRGenerationContext& context) const {
  return mThread->getLLVMType(context);
}

bool ThreadOwner::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this || toType == mThread) {
    return true;
  }
  
  if (toType->isArray()) {
    return false;
  }
  
  if (toType->isOwner()) {
    return mThread->canCastTo(context, ((const IOwnerType*) toType)->getReference());
  }
  
  return mThread->canCastTo(context, toType);
}

bool ThreadOwner::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

Value* ThreadOwner::castTo(IRGenerationContext& context,
                               Value* fromValue,
                               const IType* toType,
                               int line) const {
  if (toType == this || toType == mThread) {
    return fromValue;
  }
  
  if (toType->isOwner()) {
    const IReferenceType* toReferenceType = ((const IOwnerType*) toType)->getReference();
    return mThread->castTo(context, fromValue, toReferenceType, line);
  }
  
  return mThread->castTo(context, fromValue, toType, line);
}

void ThreadOwner::free(IRGenerationContext& context, Value* value, int line) const {
  IConcreteObjectType::composeDestructorCall(context, value);
}

Function* ThreadOwner::getDestructorFunction(IRGenerationContext& context, int line) const {
  return IConcreteObjectType::getDestructorFunctionForObject(context, getReference(), line);
}

bool ThreadOwner::isPrimitive() const {
  return false;
}

bool ThreadOwner::isOwner() const {
  return true;
}

bool ThreadOwner::isReference() const {
  return false;
}

bool ThreadOwner::isArray() const {
  return false;
}

bool ThreadOwner::isFunction() const {
  return false;
}

bool ThreadOwner::isPackage() const {
  return false;
}

bool ThreadOwner::isController() const {
  return false;
}

bool ThreadOwner::isInterface() const {
  return false;
}

bool ThreadOwner::isModel() const {
  return false;
}

bool ThreadOwner::isNode() const {
  return false;
}

bool ThreadOwner::isThread() const {
  return true;
}

bool ThreadOwner::isNative() const {
  return false;
}

bool ThreadOwner::isPointer() const {
  return false;
}

void ThreadOwner::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void ThreadOwner::createLocalVariable(IRGenerationContext& context, string name) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "ownerDeclaration");
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalOwnerVariable(name, this, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

void ThreadOwner::createFieldVariable(IRGenerationContext& context,
                                          string name,
                                          const IConcreteObjectType* object) const {
  IVariable* variable = new FieldOwnerVariable(name, object);
  context.getScopes().setVariable(variable);
}

void ThreadOwner::createParameterVariable(IRGenerationContext& context,
                                              string name,
                                              Value* value) const {
  Type* llvmType = getLLVMType(context);
  Value* alloc = IRWriter::newAllocaInst(context, llvmType, "parameterObjectPointer");
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterOwnerVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* ThreadOwner::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}
