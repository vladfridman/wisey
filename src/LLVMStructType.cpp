//
//  LLVMStructType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/FieldNativeVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMStructType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string LLVMStructType::LLVM_STRUCT_PREFIX = "::llvm::struct::";

LLVMStructType::LLVMStructType(StructType* structType, bool isExternal) :
mStructType(structType),
mIsExternal(isExternal),
mPointerType(LLVMPointerType::create(this)) {
}

LLVMStructType::~LLVMStructType() {
  delete mPointerType;
}

LLVMStructType* LLVMStructType::newLLVMStructType(llvm::StructType* structType) {
  return new LLVMStructType(structType, false);
}

LLVMStructType* LLVMStructType::newExternalLLVMStructType(llvm::StructType* structType) {
  return new LLVMStructType(structType, true);
}

string LLVMStructType::getTypeName() const {
  return LLVM_STRUCT_PREFIX + mStructType->getName().str();
}

void LLVMStructType::setBodyTypes(IRGenerationContext& context,
                                  vector<const IType*> bodyTypes) {
  mBodyTypes = bodyTypes;
  vector<Type*> types;
  for (const IType* llvmType : mBodyTypes) {
    assert(llvmType->isNative());
    types.push_back(llvmType->getLLVMType(context));
  }
  mStructType->setBody(types);
}

bool LLVMStructType::isExternal() const {
  return mIsExternal;
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
  stream << "external ::llvm::struct " << mStructType->getName().str() << " {\n";
  for (const IType* llvmType : mBodyTypes) {
    stream << "  ";
    llvmType->printToStream(context, stream);
    stream << ",\n";
  }
  stream << "}\n";
}

void LLVMStructType::createLocalVariable(IRGenerationContext& context, string name) const {
  ILLVMType::createLocalVariable(context, this, name);
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
  ILLVMType::createParameterVariable(context, this, value, name);
}

const wisey::ArrayType* LLVMStructType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const LLVMPointerType* LLVMStructType::getPointerType() const {
  return mPointerType;
}

