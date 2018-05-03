//
//  LLVMPointerOwnerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/DestroyNativeObjectFunction.hpp"
#include "wisey/FieldOwnerVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPointerOwnerType.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/ParameterOwnerVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMPointerOwnerType::LLVMPointerOwnerType(const LLVMPointerType* pointerType) :
mPointerType(pointerType) {
}

LLVMPointerOwnerType::~LLVMPointerOwnerType() {
}

string LLVMPointerOwnerType::getTypeName() const {
  return mPointerType->getTypeName() + "*";
}

llvm::PointerType* LLVMPointerOwnerType::getLLVMType(IRGenerationContext& context) const {
  return mPointerType->getLLVMType(context);
}

bool LLVMPointerOwnerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == PrimitiveTypes::BOOLEAN) {
    return true;
  }
  if (toType->isPointer()) {
    return true;
  }
  if (toType->isReference() || toType->isOwner()) {
    return !toType->isController() && !toType->isModel() && !toType->isNode();
  }
  return false;
}

bool LLVMPointerOwnerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

llvm::Value* LLVMPointerOwnerType::castTo(IRGenerationContext& context,
                                          llvm::Value* fromValue,
                                          const IType* toType,
                                          int line) const {
  if (toType->isReference() || toType->isOwner() || toType->isPointer()) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  if (toType == PrimitiveTypes::BOOLEAN) {
    return IRWriter::newICmpInst(context,
                                 ICmpInst::ICMP_NE,
                                 fromValue,
                                 ConstantPointerNull::get(getLLVMType(context)),
                                 "");
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

bool LLVMPointerOwnerType::isNative() const {
  return true;
}

bool LLVMPointerOwnerType::isPointer() const {
  return true;
}

bool LLVMPointerOwnerType::isImmutable() const {
  return false;
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

const wisey::ArrayType* LLVMPointerOwnerType::getArrayType(IRGenerationContext& context, int line) const {
  ArrayType::reportNonArrayType(context, line);
  exit(1);
}

const IReferenceType* LLVMPointerOwnerType::getReference() const {
  return NULL;
}

void LLVMPointerOwnerType::free(IRGenerationContext& context, Value* value, int line) const {
  DestroyNativeObjectFunction::call(context, value);
}

Instruction* LLVMPointerOwnerType::inject(IRGenerationContext& context,
                                     const InjectionArgumentList injectionArgumentList,
                                     int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
