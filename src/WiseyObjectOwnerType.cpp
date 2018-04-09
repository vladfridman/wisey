//
//  WiseyObjectOwnerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/FieldOwnerVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/ParameterOwnerVariable.hpp"
#include "wisey/WiseyObjectOwnerType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

WiseyObjectOwnerType* WiseyObjectOwnerType::LLVM_OBJECT_OWNER_TYPE = new WiseyObjectOwnerType();

WiseyObjectOwnerType::WiseyObjectOwnerType() {
}

WiseyObjectOwnerType::~WiseyObjectOwnerType() {
}

string WiseyObjectOwnerType::getTypeName() const {
  return getReference()->getTypeName() + "*";
}

llvm::PointerType* WiseyObjectOwnerType::getLLVMType(IRGenerationContext& context) const {
  return getReference()->getLLVMType(context);
}

bool WiseyObjectOwnerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isReference() || toType->isOwner() || toType->isPointer();
}

bool WiseyObjectOwnerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isReference() || toType->isOwner() || toType->isPointer();
}

llvm::Value* WiseyObjectOwnerType::castTo(IRGenerationContext& context,
                                          llvm::Value* fromValue,
                                          const IType* toType,
                                          int line) const {
  if (toType->isReference() || toType->isOwner() || toType->isPointer()) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  assert(false);
}

bool WiseyObjectOwnerType::isPrimitive() const {
  return false;
}

bool WiseyObjectOwnerType::isOwner() const {
  return true;
}

bool WiseyObjectOwnerType::isReference() const {
  return false;
}

bool WiseyObjectOwnerType::isArray() const {
  return false;
}

bool WiseyObjectOwnerType::isFunction() const {
  return false;
}

bool WiseyObjectOwnerType::isPackage() const {
  return false;
}

bool WiseyObjectOwnerType::isController() const {
  return false;
}

bool WiseyObjectOwnerType::isInterface() const {
  return false;
}

bool WiseyObjectOwnerType::isModel() const {
  return false;
}

bool WiseyObjectOwnerType::isNode() const {
  return false;
}

bool WiseyObjectOwnerType::isThread() const {
  return false;
}

bool WiseyObjectOwnerType::isNative() const {
  return true;
}

bool WiseyObjectOwnerType::isPointer() const {
  return false;
}

void WiseyObjectOwnerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void WiseyObjectOwnerType::createLocalVariable(IRGenerationContext& context, string name) const {
  PointerType* llvmType = getLLVMType(context);
  llvm::Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  IVariable* variable = new LocalOwnerVariable(name, this, alloca);
  context.getScopes().setVariable(variable);
}

void WiseyObjectOwnerType::createFieldVariable(IRGenerationContext& context,
                                               string name,
                                               const IConcreteObjectType* object) const {
  IVariable* variable = new FieldOwnerVariable(name, object);
  context.getScopes().setVariable(variable);
}

void WiseyObjectOwnerType::createParameterVariable(IRGenerationContext& context,
                                                   string name,
                                                   llvm::Value* value) const {
  llvm::PointerType::Type* llvmType = getLLVMType(context);
  Value* alloc = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterOwnerVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* WiseyObjectOwnerType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const WiseyObjectType* WiseyObjectOwnerType::getReference() const {
  return WiseyObjectType::LLVM_OBJECT_TYPE;
}

void WiseyObjectOwnerType::free(IRGenerationContext& context, Value* value, int line) const {
  IConcreteObjectType::composeDestructorCall(context, value);
}