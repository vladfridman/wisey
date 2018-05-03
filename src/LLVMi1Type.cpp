//
//  LLVMi1Type.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/FieldLLVMVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"
#include "wisey/LLVMi1Type.hpp"
#include "wisey/ParameterLLVMVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMi1Type::LLVMi1Type() {
  mPointerType = LLVMPointerType::create(this);
}

LLVMi1Type::~LLVMi1Type() {
  delete mPointerType;
}

string LLVMi1Type::getTypeName() const {
  return "::llvm::i1";
}

llvm::Type* LLVMi1Type::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt1Ty(context.getLLVMContext());
}

bool LLVMi1Type::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this || toType->getLLVMType(context) == getLLVMType(context);
}

bool LLVMi1Type::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this || toType->getLLVMType(context) == getLLVMType(context);
}

Value* LLVMi1Type::castTo(IRGenerationContext& context,
                           Value* fromValue,
                           const IType* toType,
                           int line) const {
  if (toType == this || toType->getLLVMType(context) == getLLVMType(context)) {
    return fromValue;
  }
  assert(false);
}

bool LLVMi1Type::isPrimitive() const {
  return false;
}

bool LLVMi1Type::isOwner() const {
  return false;
}

bool LLVMi1Type::isReference() const {
  return false;
}

bool LLVMi1Type::isArray() const {
  return false;
}

bool LLVMi1Type::isFunction() const {
  return false;
}

bool LLVMi1Type::isPackage() const {
  return false;
}

bool LLVMi1Type::isController() const {
  return false;
}

bool LLVMi1Type::isInterface() const {
  return false;
}

bool LLVMi1Type::isModel() const {
  return false;
}

bool LLVMi1Type::isNode() const {
  return false;
}

bool LLVMi1Type::isNative() const {
  return true;
}

bool LLVMi1Type::isPointer() const {
  return false;
}

bool LLVMi1Type::isImmutable() const {
  return false;
}

void LLVMi1Type::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMi1Type::createLocalVariable(IRGenerationContext& context,
                                     string name,
                                     int line) const {
  ILLVMType::createLocalLLVMVariable(context, this, name);
}

void LLVMi1Type::createFieldVariable(IRGenerationContext& context,
                                     string name,
                                     const IConcreteObjectType* object,
                                     int line) const {
  IVariable* variable = new FieldLLVMVariable(name, object);
  context.getScopes().setVariable(variable);
}

void LLVMi1Type::createParameterVariable(IRGenerationContext& context,
                                         string name,
                                         Value* value,
                                         int line) const {
  ParameterLLVMVariable* variable = new ParameterLLVMVariable(name, this, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* LLVMi1Type::getArrayType(IRGenerationContext& context, int line) const {
  ArrayType::reportNonArrayType(context, line);
  exit(1);
}

const LLVMPointerType* LLVMi1Type::getPointerType(IRGenerationContext& context, int line) const {
  return mPointerType;
}

const ILLVMTypeSpecifier* LLVMi1Type::newTypeSpecifier(int line) const {
  return new LLVMPrimitiveTypeSpecifier(this, line);
}

Instruction* LLVMi1Type::inject(IRGenerationContext& context,
                                const InjectionArgumentList injectionArgumentList,
                                int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
