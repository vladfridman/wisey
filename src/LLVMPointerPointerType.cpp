//
//  LLVMPointerPointerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/FieldNativeVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPointerPointerType.hpp"
#include "wisey/LocalNativeVariable.hpp"

using namespace std;
using namespace wisey;

LLVMPointerPointerType::LLVMPointerPointerType(const LLVMPointerType* baseType) {
  mBaseType = baseType;
}

LLVMPointerPointerType::~LLVMPointerPointerType() {
}

string LLVMPointerPointerType::getTypeName() const {
  return mBaseType->getTypeName() + "*";
}

llvm::PointerType* LLVMPointerPointerType::getLLVMType(IRGenerationContext& context) const {
  return mBaseType->getLLVMType(context)->getPointerTo();
}

bool LLVMPointerPointerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isNative() && toType->isReference();
}

bool LLVMPointerPointerType::canAutoCastTo(IRGenerationContext& context,
                                           const IType* toType) const {
  return toType->isNative() && toType->isReference();
}

llvm::Value* LLVMPointerPointerType::castTo(IRGenerationContext& context,
                                     llvm::Value* fromValue,
                                     const IType* toType,
                                     int line) const {
  if (toType->isNative() && toType->isReference()) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  assert(false);
}

bool LLVMPointerPointerType::isPrimitive() const {
  return false;
}

bool LLVMPointerPointerType::isOwner() const {
  return false;
}

bool LLVMPointerPointerType::isReference() const {
  return true;
}

bool LLVMPointerPointerType::isArray() const {
  return false;
}

bool LLVMPointerPointerType::isFunction() const {
  return false;
}

bool LLVMPointerPointerType::isPackage() const {
  return false;
}

bool LLVMPointerPointerType::isController() const {
  return false;
}

bool LLVMPointerPointerType::isInterface() const {
  return false;
}

bool LLVMPointerPointerType::isModel() const {
  return false;
}

bool LLVMPointerPointerType::isNode() const {
  return false;
}

bool LLVMPointerPointerType::isThread() const {
  return false;
}

bool LLVMPointerPointerType::isNative() const {
  return true;
}

void LLVMPointerPointerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMPointerPointerType::createLocalVariable(IRGenerationContext& context, string name) const {
  llvm::Value* alloca = IRWriter::newAllocaInst(context, getLLVMType(context), name);
  IVariable* variable = new LocalNativeVariable(name, this, alloca);
  context.getScopes().setVariable(variable);
}

void LLVMPointerPointerType::createFieldVariable(IRGenerationContext& context,
                                          string name,
                                          const IConcreteObjectType* object) const {
  IVariable* variable = new FieldNativeVariable(name, object);
  context.getScopes().setVariable(variable);
}

void LLVMPointerPointerType::createParameterVariable(IRGenerationContext& context,
                                              string name,
                                              llvm::Value* value) const {
  assert(false);
}

const wisey::ArrayType* LLVMPointerPointerType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IObjectType* LLVMPointerPointerType::getObjectType() const {
  return NULL;
}

const ILLVMType* LLVMPointerPointerType::getPointerType() const {
  assert(false);
}

const ILLVMType* LLVMPointerPointerType::getDereferenceType() const {
  return mBaseType;
}
