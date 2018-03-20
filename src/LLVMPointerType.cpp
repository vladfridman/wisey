//
//  LLVMPointerType.cpp
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
#include "wisey/LLVMPointerType.hpp"
#include "wisey/LocalNativeVariable.hpp"
#include "wisey/PointerType.hpp"

using namespace std;
using namespace wisey;

LLVMPointerType::LLVMPointerType(const ILLVMType* baseType) {
  mBaseType = baseType;
  mPointerType = new LLVMPointerPointerType(this);
}

LLVMPointerType::~LLVMPointerType() {
  delete mPointerType;
}

string LLVMPointerType::getTypeName() const {
  return mBaseType->getTypeName() + "*";
}

llvm::PointerType* LLVMPointerType::getLLVMType(IRGenerationContext& context) const {
  return mBaseType->getLLVMType(context)->getPointerTo();
}

bool LLVMPointerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isNative() && toType->isReference();
}

bool LLVMPointerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isNative() && toType->isReference();
}

llvm::Value* LLVMPointerType::castTo(IRGenerationContext& context,
                                 llvm::Value* fromValue,
                                 const IType* toType,
                                 int line) const {
  if (toType->isNative() && toType->isReference()) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  assert(false);
}

bool LLVMPointerType::isPrimitive() const {
  return false;
}

bool LLVMPointerType::isOwner() const {
  return false;
}

bool LLVMPointerType::isReference() const {
  return true;
}

bool LLVMPointerType::isArray() const {
  return false;
}

bool LLVMPointerType::isFunction() const {
  return false;
}

bool LLVMPointerType::isPackage() const {
  return false;
}

bool LLVMPointerType::isController() const {
  return false;
}

bool LLVMPointerType::isInterface() const {
  return false;
}

bool LLVMPointerType::isModel() const {
  return false;
}

bool LLVMPointerType::isNode() const {
  return false;
}

bool LLVMPointerType::isThread() const {
  return false;
}

bool LLVMPointerType::isNative() const {
  return true;
}

void LLVMPointerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMPointerType::createLocalVariable(IRGenerationContext& context, string name) const {
  llvm::Value* alloca = IRWriter::newAllocaInst(context, getLLVMType(context), name);
  IVariable* variable = new LocalNativeVariable(name, this, alloca);
  context.getScopes().setVariable(variable);
}

void LLVMPointerType::createFieldVariable(IRGenerationContext& context,
                                          string name,
                                          const IConcreteObjectType* object) const {
  IVariable* variable = new FieldNativeVariable(name, object);
  context.getScopes().setVariable(variable);
}

void LLVMPointerType::createParameterVariable(IRGenerationContext& context,
                                          string name,
                                          llvm::Value* value) const {
  assert(false);
}

const wisey::ArrayType* LLVMPointerType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IObjectType* LLVMPointerType::getObjectType() const {
  return NULL;
}

const ILLVMType* LLVMPointerType::getPointerType() const {
  return mPointerType;
}

const ILLVMType* LLVMPointerType::getDereferenceType() const {
  return mBaseType;
}