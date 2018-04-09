//
//  WiseyModelOwnerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/FieldOwnerVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/WiseyModelOwnerType.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/ParameterOwnerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

WiseyModelOwnerType* WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE = new WiseyModelOwnerType();

WiseyModelOwnerType::WiseyModelOwnerType() {
}

WiseyModelOwnerType::~WiseyModelOwnerType() {
}

string WiseyModelOwnerType::getTypeName() const {
  return getReference()->getTypeName() + "*";
}

llvm::PointerType* WiseyModelOwnerType::getLLVMType(IRGenerationContext& context) const {
  return getReference()->getLLVMType(context);
}

bool WiseyModelOwnerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isReference() || toType->isOwner() || toType->isPointer();
}

bool WiseyModelOwnerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isReference() || toType->isOwner() || toType->isPointer();
}

llvm::Value* WiseyModelOwnerType::castTo(IRGenerationContext& context,
                                         llvm::Value* fromValue,
                                         const IType* toType,
                                         int line) const {
  if (toType->isReference() || toType->isOwner() || toType->isPointer()) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  assert(false);
}

bool WiseyModelOwnerType::isPrimitive() const {
  return false;
}

bool WiseyModelOwnerType::isOwner() const {
  return true;
}

bool WiseyModelOwnerType::isReference() const {
  return false;
}

bool WiseyModelOwnerType::isArray() const {
  return false;
}

bool WiseyModelOwnerType::isFunction() const {
  return false;
}

bool WiseyModelOwnerType::isPackage() const {
  return false;
}

bool WiseyModelOwnerType::isController() const {
  return false;
}

bool WiseyModelOwnerType::isInterface() const {
  return false;
}

bool WiseyModelOwnerType::isModel() const {
  return false;
}

bool WiseyModelOwnerType::isNode() const {
  return false;
}

bool WiseyModelOwnerType::isThread() const {
  return false;
}

bool WiseyModelOwnerType::isNative() const {
  return true;
}

bool WiseyModelOwnerType::isPointer() const {
  return false;
}

void WiseyModelOwnerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void WiseyModelOwnerType::createLocalVariable(IRGenerationContext& context, string name) const {
  PointerType* llvmType = getLLVMType(context);
  llvm::Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  IVariable* variable = new LocalOwnerVariable(name, this, alloca);
  context.getScopes().setVariable(variable);
}

void WiseyModelOwnerType::createFieldVariable(IRGenerationContext& context,
                                              string name,
                                              const IConcreteObjectType* object) const {
  IVariable* variable = new FieldOwnerVariable(name, object);
  context.getScopes().setVariable(variable);
}

void WiseyModelOwnerType::createParameterVariable(IRGenerationContext& context,
                                                  string name,
                                                  llvm::Value* value) const {
  llvm::PointerType::Type* llvmType = getLLVMType(context);
  Value* alloc = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterOwnerVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* WiseyModelOwnerType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const WiseyModelType* WiseyModelOwnerType::getReference() const {
  return WiseyModelType::WISEY_MODEL_TYPE;
}

void WiseyModelOwnerType::free(IRGenerationContext& context, Value* value, int line) const {
  IConcreteObjectType::composeDestructorCall(context, value);
}