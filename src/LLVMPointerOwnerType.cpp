//
//  LLVMPointerOwnerType.cpp
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
#include "wisey/LLVMPointerOwnerType.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/ParameterOwnerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMPointerOwnerType::LLVMPointerOwnerType(const ILLVMPointerType* referenceType) {
  mReferenceType = referenceType;
}

LLVMPointerOwnerType::~LLVMPointerOwnerType() {
}

string LLVMPointerOwnerType::getTypeName() const {
  return mReferenceType->getTypeName() + "*";
}

llvm::PointerType* LLVMPointerOwnerType::getLLVMType(IRGenerationContext& context) const {
  return mReferenceType->getLLVMType(context);
}

bool LLVMPointerOwnerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isReference() || toType->isOwner();
}

bool LLVMPointerOwnerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isReference() || toType->isOwner();
}

llvm::Value* LLVMPointerOwnerType::castTo(IRGenerationContext& context,
                                          llvm::Value* fromValue,
                                          const IType* toType,
                                          int line) const {
  if (toType->isReference() || toType->isOwner()) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  assert(false);
}

bool LLVMPointerOwnerType::isPrimitive() const {
  return false;
}

bool LLVMPointerOwnerType::isOwner() const {
  return true;
}

bool LLVMPointerOwnerType::isReference() const {
  return false;
}

bool LLVMPointerOwnerType::isArray() const {
  return false;
}

bool LLVMPointerOwnerType::isFunction() const {
  return false;
}

bool LLVMPointerOwnerType::isPackage() const {
  return false;
}

bool LLVMPointerOwnerType::isController() const {
  return false;
}

bool LLVMPointerOwnerType::isInterface() const {
  return false;
}

bool LLVMPointerOwnerType::isModel() const {
  return false;
}

bool LLVMPointerOwnerType::isNode() const {
  return false;
}

bool LLVMPointerOwnerType::isThread() const {
  return false;
}

bool LLVMPointerOwnerType::isNative() const {
  return true;
}

void LLVMPointerOwnerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMPointerOwnerType::createLocalVariable(IRGenerationContext& context, string name) const {
  PointerType* llvmType = getLLVMType(context);
  llvm::Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  IVariable* variable = new LocalOwnerVariable(name, this, alloca);
  context.getScopes().setVariable(variable);
}

void LLVMPointerOwnerType::createFieldVariable(IRGenerationContext& context,
                                               string name,
                                               const IConcreteObjectType* object) const {
  IVariable* variable = new FieldOwnerVariable(name, object);
  context.getScopes().setVariable(variable);
}

void LLVMPointerOwnerType::createParameterVariable(IRGenerationContext& context,
                                                   string name,
                                                   llvm::Value* value) const {
  llvm::PointerType::Type* llvmType = getLLVMType(context);
  Value* alloc = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterOwnerVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* LLVMPointerOwnerType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const ILLVMPointerType* LLVMPointerOwnerType::getReferenceType() const {
  return mReferenceType;
}

const ILLVMPointerType* LLVMPointerOwnerType::getPointerType() const {
  assert(false);
}

void LLVMPointerOwnerType::free(IRGenerationContext& context, Value *value) const {
  IConcreteObjectType::composeDestructorCall(context, value);
}
