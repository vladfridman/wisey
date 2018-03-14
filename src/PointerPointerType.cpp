//
//  PointerPointerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/PointerType.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalNativeVariable.hpp"
#include "wisey/PointerPointerType.hpp"

using namespace std;
using namespace wisey;

PointerPointerType::PointerPointerType(const PointerType* baseType) {
  mBaseType = baseType;
}

PointerPointerType::~PointerPointerType() {
}

string PointerPointerType::getTypeName() const {
  return "pointerpointer";
}

llvm::PointerType* PointerPointerType::getLLVMType(IRGenerationContext& context) const {
  return mBaseType->getLLVMType(context)->getPointerTo();
}

bool PointerPointerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isNative() && toType->isReference();
}

bool PointerPointerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isNative() && toType->isReference();
}

llvm::Value* PointerPointerType::castTo(IRGenerationContext& context,
                                 llvm::Value* fromValue,
                                 const IType* toType,
                                 int line) const {
  if (toType->isNative() && toType->isReference()) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  assert(false);
}

bool PointerPointerType::isPrimitive() const {
  return false;
}

bool PointerPointerType::isOwner() const {
  return false;
}

bool PointerPointerType::isReference() const {
  return true;
}

bool PointerPointerType::isArray() const {
  return false;
}

bool PointerPointerType::isFunction() const {
  return false;
}

bool PointerPointerType::isPackage() const {
  return false;
}

bool PointerPointerType::isController() const {
  return false;
}

bool PointerPointerType::isInterface() const {
  return false;
}

bool PointerPointerType::isModel() const {
  return false;
}

bool PointerPointerType::isNode() const {
  return false;
}

bool PointerPointerType::isThread() const {
  return false;
}

bool PointerPointerType::isNative() const {
  return true;
}

void PointerPointerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void PointerPointerType::createLocalVariable(IRGenerationContext& context, string name) const {
  llvm::Value* alloca = IRWriter::newAllocaInst(context, getLLVMType(context), name);
  IVariable* variable = new LocalNativeVariable(name, this, alloca);
  context.getScopes().setVariable(variable);
}

void PointerPointerType::createFieldVariable(IRGenerationContext& context,
                                      string name,
                                      const IConcreteObjectType* object) const {
  assert(false);
}

void PointerPointerType::createParameterVariable(IRGenerationContext& context,
                                          string name,
                                          llvm::Value* value) const {
  assert(false);
}

const wisey::ArrayType* PointerPointerType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IObjectType* PointerPointerType::getObjectType() const {
  return NULL;
}

const PointerType* PointerPointerType::getPointerType() const {
  assert(false);
}

const IType* PointerPointerType::getDereferenceType() const {
  return mBaseType;
}
