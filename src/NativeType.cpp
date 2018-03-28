//
//  NativeType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/FieldNativeVariable.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalNativeVariable.hpp"
#include "wisey/NativeType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

NativeType::NativeType(Type* type) : mType(type), mPointerType(new LLVMPointerType(this)) {
}

NativeType::~NativeType() {
  delete mPointerType;
}

string NativeType::getTypeName() const {
  return "native";
}

llvm::Type* NativeType::getLLVMType(IRGenerationContext& context) const {
  return mType;
}

bool NativeType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this || toType->getLLVMType(context) == getLLVMType(context);
}

bool NativeType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this || toType->getLLVMType(context) == getLLVMType(context);
}

Value* NativeType::castTo(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType,
                          int line) const {
  if (toType == this || toType->getLLVMType(context) == getLLVMType(context)) {
    return fromValue;
  }
  
  assert(false);
}

bool NativeType::isPrimitive() const {
  return false;
}

bool NativeType::isOwner() const {
  return false;
}

bool NativeType::isReference() const {
  return mType->isPointerTy();
}

bool NativeType::isArray() const {
  return false;
}

bool NativeType::isFunction() const {
  return false;
}

bool NativeType::isPackage() const {
  return false;
}

bool NativeType::isController() const {
  return false;
}

bool NativeType::isInterface() const {
  return false;
}

bool NativeType::isModel() const {
  return false;
}

bool NativeType::isNode() const {
  return false;
}

bool NativeType::isThread() const {
  return false;
}

bool NativeType::isNative() const {
  return true;
}

void NativeType::printToStream(IRGenerationContext &context, iostream& stream) const {
  assert(false);
}

void NativeType::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void NativeType::createFieldVariable(IRGenerationContext& context,
                                     string name,
                                     const IConcreteObjectType* object) const {
  IVariable* variable = new FieldNativeVariable(name, object);
  context.getScopes().setVariable(variable);
}

void NativeType::createParameterVariable(IRGenerationContext& context,
                                         string name,
                                         Value* value) const {
  assert(false);
}

const wisey::ArrayType* NativeType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IReferenceType* NativeType::getReferenceType() const {
  return NULL;
}

const ILLVMPointerType* NativeType::getPointerType() const {
  return mPointerType;
}
