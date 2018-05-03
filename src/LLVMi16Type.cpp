//
//  LLVMi16Type.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/FieldLLVMVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"
#include "wisey/LLVMi16Type.hpp"
#include "wisey/ParameterLLVMVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMi16Type::LLVMi16Type() {
  mPointerType = LLVMPointerType::create(this);
}

LLVMi16Type::~LLVMi16Type() {
  delete mPointerType;
}

string LLVMi16Type::getTypeName() const {
  return "::llvm::i16";
}

llvm::Type* LLVMi16Type::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt16Ty(context.getLLVMContext());
}

bool LLVMi16Type::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this || toType->getLLVMType(context) == getLLVMType(context);
}

bool LLVMi16Type::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this || toType->getLLVMType(context) == getLLVMType(context);
}

Value* LLVMi16Type::castTo(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType,
                          int line) const {
  if (toType == this || toType->getLLVMType(context) == getLLVMType(context)) {
    return fromValue;
  }
  assert(false);
}

bool LLVMi16Type::isPrimitive() const {
  return false;
}

bool LLVMi16Type::isOwner() const {
  return false;
}

bool LLVMi16Type::isReference() const {
  return false;
}

bool LLVMi16Type::isArray() const {
  return false;
}

bool LLVMi16Type::isFunction() const {
  return false;
}

bool LLVMi16Type::isPackage() const {
  return false;
}

bool LLVMi16Type::isController() const {
  return false;
}

bool LLVMi16Type::isInterface() const {
  return false;
}

bool LLVMi16Type::isModel() const {
  return false;
}

bool LLVMi16Type::isNode() const {
  return false;
}

bool LLVMi16Type::isNative() const {
  return true;
}

bool LLVMi16Type::isPointer() const {
  return false;
}

bool LLVMi16Type::isImmutable() const {
  return false;
}

void LLVMi16Type::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMi16Type::createLocalVariable(IRGenerationContext& context, string name) const {
  ILLVMType::createLocalLLVMVariable(context, this, name);
}

void LLVMi16Type::createFieldVariable(IRGenerationContext& context,
                                      string name,
                                      const IConcreteObjectType* object) const {
  IVariable* variable = new FieldLLVMVariable(name, object);
  context.getScopes().setVariable(variable);
}

void LLVMi16Type::createParameterVariable(IRGenerationContext& context,
                                          string name,
                                          Value* value) const {
  ParameterLLVMVariable* variable = new ParameterLLVMVariable(name, this, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* LLVMi16Type::getArrayType(IRGenerationContext& context, int line) const {
  ArrayType::reportNonArrayType(context, line);
  exit(1);
}

const LLVMPointerType* LLVMi16Type::getPointerType() const {
  return mPointerType;
}

const ILLVMTypeSpecifier* LLVMi16Type::newTypeSpecifier(int line) const {
  return new LLVMPrimitiveTypeSpecifier(this, line);
}

Instruction* LLVMi16Type::inject(IRGenerationContext& context,
                                 const InjectionArgumentList injectionArgumentList,
                                 int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
