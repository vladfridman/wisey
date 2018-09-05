//
//  LLVMImmutablePointerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/FieldPointerVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMImmutablePointerType.hpp"
#include "wisey/LLVMImmutablePointerOwnerType.hpp"
#include "wisey/LLVMPointerOwnerType.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/LocalPointerVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/ParameterPointerVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

LLVMImmutablePointerType::LLVMImmutablePointerType(const LLVMPointerType* basePointerType) {
  mBasePointerType = basePointerType;
  mImmutablePointerOwnerType = new LLVMImmutablePointerOwnerType(this);
}

LLVMImmutablePointerType::~LLVMImmutablePointerType() {
  delete mImmutablePointerOwnerType;
}

const LLVMImmutablePointerOwnerType* LLVMImmutablePointerType::getOwner() const {
  return mImmutablePointerOwnerType;
}

const ILLVMType* LLVMImmutablePointerType::getBaseType() const {
  return mBasePointerType->getBaseType();
}

string LLVMImmutablePointerType::getTypeName() const {
  return mBasePointerType->getTypeName() + "::immutable";
}

llvm::PointerType* LLVMImmutablePointerType::getLLVMType(IRGenerationContext& context) const {
  return mBasePointerType->getLLVMType(context);
}

bool LLVMImmutablePointerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this) {
    return true;
  }
  return false;
}

bool LLVMImmutablePointerType::canAutoCastTo(IRGenerationContext& context,
                                             const IType* toType) const {
  return canCastTo(context, toType);
}

llvm::Value* LLVMImmutablePointerType::castTo(IRGenerationContext& context,
                                              llvm::Value* fromValue,
                                              const IType* toType,
                                              int line) const {
  if (toType == this) {
    return fromValue;
  }
  assert(false && "Casting immutable pointer type to another type is not allowed");
}

bool LLVMImmutablePointerType::isPrimitive() const {
  return false;
}

bool LLVMImmutablePointerType::isOwner() const {
  return false;
}

bool LLVMImmutablePointerType::isReference() const {
  return false;
}

bool LLVMImmutablePointerType::isArray() const {
  return false;
}

bool LLVMImmutablePointerType::isFunction() const {
  return false;
}

bool LLVMImmutablePointerType::isPackage() const {
  return false;
}

bool LLVMImmutablePointerType::isController() const {
  return false;
}

bool LLVMImmutablePointerType::isInterface() const {
  return false;
}

bool LLVMImmutablePointerType::isModel() const {
  return false;
}

bool LLVMImmutablePointerType::isNode() const {
  return false;
}

bool LLVMImmutablePointerType::isNative() const {
  return true;
}

bool LLVMImmutablePointerType::isPointer() const {
  return true;
}

bool LLVMImmutablePointerType::isImmutable() const {
  return true;
}

void LLVMImmutablePointerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMImmutablePointerType::createLocalVariable(IRGenerationContext& context,
                                                   string name,
                                                   int line) const {
  assert(false && "Local varaibles of immutable pointer type are not supported");
}

void LLVMImmutablePointerType::createFieldVariable(IRGenerationContext& context,
                                                   string name,
                                                   const IConcreteObjectType* object,
                                                   int line) const {
  IVariable* variable = new FieldPointerVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void LLVMImmutablePointerType::createParameterVariable(IRGenerationContext& context,
                                                       string name,
                                                       llvm::Value* value,
                                                       int line) const {
  assert(false && "Parameter varaibles of immutable pointer type are not supported");
}

const wisey::ArrayType* LLVMImmutablePointerType::getArrayType(IRGenerationContext& context,
                                                               int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

const LLVMPointerType* LLVMImmutablePointerType::getPointerType(IRGenerationContext& context,
                                                                int line) const {
  context.reportError(line, "Two and more degree llvm immutable pointers are not supported");
  throw 1;
}

llvm::Instruction* LLVMImmutablePointerType::inject(IRGenerationContext& context,
                                                    const InjectionArgumentList
                                                    injectionArgumentList,
                                                    int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}

