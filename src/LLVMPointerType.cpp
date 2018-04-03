//
//  LLVMPointerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/FieldPointerVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMObjectOwnerType.hpp"
#include "wisey/LLVMPointerType.hpp"
#include "wisey/LocalPointerVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/ParameterPointerVariable.hpp"

using namespace std;
using namespace wisey;

unsigned long LLVMPointerType::LLVM_POINTER_MAX_DEGREE = 3;

LLVMPointerType::LLVMPointerType(const ILLVMType* baseType, unsigned long degree) {
  mBaseType = baseType;
  mPointerType = degree < LLVM_POINTER_MAX_DEGREE ? new LLVMPointerType(this, degree + 1u) : NULL;
}

LLVMPointerType::~LLVMPointerType() {
  delete mPointerType;
}

LLVMPointerType* LLVMPointerType::create(const ILLVMType* baseType) {
  return new LLVMPointerType(baseType, 0);
}

string LLVMPointerType::getTypeName() const {
  return mBaseType->getTypeName() + "::pointer";
}

llvm::PointerType* LLVMPointerType::getLLVMType(IRGenerationContext& context) const {
  return mBaseType->getLLVMType(context)->getPointerTo();
}

bool LLVMPointerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isReference();
}

bool LLVMPointerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isReference();
}

llvm::Value* LLVMPointerType::castTo(IRGenerationContext& context,
                                 llvm::Value* fromValue,
                                 const IType* toType,
                                 int line) const {
  if (toType->isReference()) {
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
  llvm::PointerType* llvmType = getLLVMType(context);
  
  llvm::Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* variable = new LocalPointerVariable(name, this, alloca);
  context.getScopes().setVariable(variable);
}

void LLVMPointerType::createFieldVariable(IRGenerationContext& context,
                                          string name,
                                          const IConcreteObjectType* object) const {
  IVariable* variable = new FieldPointerVariable(name, object);
  context.getScopes().setVariable(variable);
}

void LLVMPointerType::createParameterVariable(IRGenerationContext& context,
                                              string name,
                                              llvm::Value* value) const {
  IVariable* variable = new ParameterPointerVariable(name, this, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* LLVMPointerType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const LLVMPointerType* LLVMPointerType::getPointerType() const {
  if (mPointerType) {
    return mPointerType;
  }
  Log::e("Three and more degree llvm pointers are not supported");
  exit(1);
}
