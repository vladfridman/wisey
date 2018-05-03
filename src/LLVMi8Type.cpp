//
//  LLVMi8Type.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/FieldLLVMVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"
#include "wisey/LLVMi8Type.hpp"
#include "wisey/ParameterLLVMVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMi8Type::LLVMi8Type() {
  mPointerType = LLVMPointerType::create(this);
}

LLVMi8Type::~LLVMi8Type() {
  delete mPointerType;
}

string LLVMi8Type::getTypeName() const {
  return "::llvm::i8";
}

llvm::Type* LLVMi8Type::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt8Ty(context.getLLVMContext());
}

bool LLVMi8Type::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this || toType->getLLVMType(context) == getLLVMType(context);
}

bool LLVMi8Type::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this || toType->getLLVMType(context) == getLLVMType(context);
}

Value* LLVMi8Type::castTo(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType,
                          int line) const {
  if (toType == this || toType->getLLVMType(context) == getLLVMType(context)) {
    return fromValue;
  }
  assert(false);
}

bool LLVMi8Type::isPrimitive() const {
  return false;
}

bool LLVMi8Type::isOwner() const {
  return false;
}

bool LLVMi8Type::isReference() const {
  return false;
}

bool LLVMi8Type::isArray() const {
  return false;
}

bool LLVMi8Type::isFunction() const {
  return false;
}

bool LLVMi8Type::isPackage() const {
  return false;
}

bool LLVMi8Type::isController() const {
  return false;
}

bool LLVMi8Type::isInterface() const {
  return false;
}

bool LLVMi8Type::isModel() const {
  return false;
}

bool LLVMi8Type::isNode() const {
  return false;
}

bool LLVMi8Type::isNative() const {
  return true;
}

bool LLVMi8Type::isPointer() const {
  return false;
}

bool LLVMi8Type::isImmutable() const {
  return false;
}

void LLVMi8Type::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMi8Type::createLocalVariable(IRGenerationContext& context,
                                     string name,
                                     int line) const {
  ILLVMType::createLocalLLVMVariable(context, this, name, line);
}

void LLVMi8Type::createFieldVariable(IRGenerationContext& context,
                                     string name,
                                     const IConcreteObjectType* object,
                                     int line) const {
  IVariable* variable = new FieldLLVMVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void LLVMi8Type::createParameterVariable(IRGenerationContext& context,
                                         string name,
                                         Value* value,
                                         int line) const {
  ParameterLLVMVariable* variable = new ParameterLLVMVariable(name, this, value, line);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* LLVMi8Type::getArrayType(IRGenerationContext& context, int line) const {
  ArrayType::reportNonArrayType(context, line);
  exit(1);
}

const LLVMPointerType* LLVMi8Type::getPointerType(IRGenerationContext& context, int line) const {
  return mPointerType;
}

const ILLVMTypeSpecifier* LLVMi8Type::newTypeSpecifier(int line) const {
  return new LLVMPrimitiveTypeSpecifier(this, line);
}

Instruction* LLVMi8Type::inject(IRGenerationContext& context,
                                const InjectionArgumentList injectionArgumentList,
                                int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
