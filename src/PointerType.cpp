//
//  PointerType.cpp
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
#include "wisey/PointerType.hpp"

using namespace std;
using namespace wisey;

PointerType::PointerType(const IType* baseType) {
  mBaseType = baseType;
}

PointerType::~PointerType() {
}

string PointerType::getTypeName() const {
  return "pointer";
}

llvm::PointerType* PointerType::getLLVMType(IRGenerationContext& context) const {
  return mBaseType->getLLVMType(context)->getPointerTo();
}

bool PointerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isNative() && toType->isReference();
}

bool PointerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isNative() && toType->isReference();
}

llvm::Value* PointerType::castTo(IRGenerationContext& context,
                                 llvm::Value* fromValue,
                                 const IType* toType,
                                 int line) const {
  if (toType->isNative() && toType->isReference()) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  assert(false);
}

bool PointerType::isPrimitive() const {
  return false;
}

bool PointerType::isOwner() const {
  return false;
}

bool PointerType::isReference() const {
  return true;
}

bool PointerType::isArray() const {
  return false;
}

bool PointerType::isFunction() const {
  return false;
}

bool PointerType::isPackage() const {
  return false;
}

bool PointerType::isController() const {
  return false;
}

bool PointerType::isInterface() const {
  return false;
}

bool PointerType::isModel() const {
  return false;
}

bool PointerType::isNode() const {
  return false;
}

bool PointerType::isThread() const {
  return false;
}

bool PointerType::isNative() const {
  return true;
}

void PointerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void PointerType::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void PointerType::createFieldVariable(IRGenerationContext& context,
                                      string name,
                                      const IConcreteObjectType* object) const {
  assert(false);
}

void PointerType::createParameterVariable(IRGenerationContext& context,
                                          string name,
                                          llvm::Value* value) const {
  assert(false);
}

const wisey::ArrayType* PointerType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IObjectType* PointerType::getObjectType() const {
  return NULL;
}

const PointerType* PointerType::getPointerType() const {
  assert(false);
}
