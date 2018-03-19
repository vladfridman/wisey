//
//  LLVMStructType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/FieldNativeVariable.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMStructType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMStructType::LLVMStructType(StructType* structType) :
mStructType(structType),
mPointerType(new LLVMPointerType(this)) {
}

LLVMStructType::~LLVMStructType() {
  delete mPointerType;
}

string LLVMStructType::getTypeName() const {
  return mStructType->getName().str();
}

llvm::StructType* LLVMStructType::getLLVMType(IRGenerationContext& context) const {
  return mStructType;
}

bool LLVMStructType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this || toType->getLLVMType(context) == getLLVMType(context);
}

bool LLVMStructType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this || toType->getLLVMType(context) == getLLVMType(context);
}

Value* LLVMStructType::castTo(IRGenerationContext& context,
                              Value* fromValue,
                              const IType* toType,
                              int line) const {
  if (toType == this || toType->getLLVMType(context) == getLLVMType(context)) {
    return fromValue;
  }
  
  assert(false);
}

bool LLVMStructType::isPrimitive() const {
  return false;
}

bool LLVMStructType::isOwner() const {
  return false;
}

bool LLVMStructType::isReference() const {
  return false;
}

bool LLVMStructType::isArray() const {
  return false;
}

bool LLVMStructType::isFunction() const {
  return false;
}

bool LLVMStructType::isPackage() const {
  return false;
}

bool LLVMStructType::isController() const {
  return false;
}

bool LLVMStructType::isInterface() const {
  return false;
}

bool LLVMStructType::isModel() const {
  return false;
}

bool LLVMStructType::isNode() const {
  return false;
}

bool LLVMStructType::isThread() const {
  return false;
}

bool LLVMStructType::isNative() const {
  return true;
}

void LLVMStructType::printToStream(IRGenerationContext &context, iostream& stream) const {
  assert(false);
}

void LLVMStructType::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void LLVMStructType::createFieldVariable(IRGenerationContext& context,
                                         string name,
                                         const IConcreteObjectType* object) const {
  IVariable* variable = new FieldNativeVariable(name, object);
  context.getScopes().setVariable(variable);
}

void LLVMStructType::createParameterVariable(IRGenerationContext& context,
                                             string name,
                                             Value* value) const {
  assert(false);
}

const wisey::ArrayType* LLVMStructType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IObjectType* LLVMStructType::getObjectType() const {
  return NULL;
}

const LLVMPointerType* LLVMStructType::getPointerType() const {
  return mPointerType;
}

const IType* LLVMStructType::getDereferenceType() const {
  assert(false);
}

