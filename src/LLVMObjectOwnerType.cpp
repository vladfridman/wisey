//
//  LLVMObjectOwnerType.cpp
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
#include "wisey/LLVMObjectOwnerType.hpp"
#include "wisey/LLVMObjectType.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/ParameterOwnerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMObjectOwnerType* LLVMObjectOwnerType::LLVM_OBJECT_OWNER_TYPE = new LLVMObjectOwnerType();

LLVMObjectOwnerType::LLVMObjectOwnerType() {
}

LLVMObjectOwnerType::~LLVMObjectOwnerType() {
}

string LLVMObjectOwnerType::getTypeName() const {
  return LLVMObjectType::LLVM_OBJECT_TYPE->getTypeName() + "*";
}

llvm::PointerType* LLVMObjectOwnerType::getLLVMType(IRGenerationContext& context) const {
  return LLVMObjectType::LLVM_OBJECT_TYPE->getLLVMType(context);
}

bool LLVMObjectOwnerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isReference() || toType->isOwner();
}

bool LLVMObjectOwnerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isReference() || toType->isOwner();
}

llvm::Value* LLVMObjectOwnerType::castTo(IRGenerationContext& context,
                                          llvm::Value* fromValue,
                                          const IType* toType,
                                          int line) const {
  if (toType->isReference() || toType->isOwner()) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  assert(false);
}

bool LLVMObjectOwnerType::isPrimitive() const {
  return false;
}

bool LLVMObjectOwnerType::isOwner() const {
  return true;
}

bool LLVMObjectOwnerType::isReference() const {
  return false;
}

bool LLVMObjectOwnerType::isArray() const {
  return false;
}

bool LLVMObjectOwnerType::isFunction() const {
  return false;
}

bool LLVMObjectOwnerType::isPackage() const {
  return false;
}

bool LLVMObjectOwnerType::isController() const {
  return false;
}

bool LLVMObjectOwnerType::isInterface() const {
  return false;
}

bool LLVMObjectOwnerType::isModel() const {
  return false;
}

bool LLVMObjectOwnerType::isNode() const {
  return false;
}

bool LLVMObjectOwnerType::isThread() const {
  return false;
}

bool LLVMObjectOwnerType::isNative() const {
  return true;
}

void LLVMObjectOwnerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMObjectOwnerType::createLocalVariable(IRGenerationContext& context, string name) const {
  PointerType* llvmType = getLLVMType(context);
  llvm::Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  IVariable* variable = new LocalOwnerVariable(name, this, alloca);
  context.getScopes().setVariable(variable);
}

void LLVMObjectOwnerType::createFieldVariable(IRGenerationContext& context,
                                               string name,
                                               const IConcreteObjectType* object) const {
  IVariable* variable = new FieldOwnerVariable(name, object);
  context.getScopes().setVariable(variable);
}

void LLVMObjectOwnerType::createParameterVariable(IRGenerationContext& context,
                                                   string name,
                                                   llvm::Value* value) const {
  llvm::PointerType::Type* llvmType = getLLVMType(context);
  Value* alloc = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterOwnerVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* LLVMObjectOwnerType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IReferenceType* LLVMObjectOwnerType::getReferenceType() const {
  return LLVMObjectType::LLVM_OBJECT_TYPE;
}

const ILLVMPointerType* LLVMObjectOwnerType::getPointerType() const {
  assert(false);
}

void LLVMObjectOwnerType::free(IRGenerationContext& context, Value *value) const {
  IConcreteObjectType::composeDestructorCall(context, value);
}
