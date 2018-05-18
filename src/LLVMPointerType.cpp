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
#include "wisey/LLVMPointerOwnerType.hpp"
#include "wisey/LLVMPointerType.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/LocalPointerVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/ParameterPointerVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

unsigned long LLVMPointerType::LLVM_POINTER_MAX_DEGREE = 3;

LLVMPointerType::LLVMPointerType(const ILLVMType* baseType, unsigned long degree) {
  mBaseType = baseType;
  mPointerType = degree < LLVM_POINTER_MAX_DEGREE ? new LLVMPointerType(this, degree + 1u) : NULL;
  mPointerOwnerType = new LLVMPointerOwnerType(this);
}

LLVMPointerType::~LLVMPointerType() {
  delete mPointerType;
}

const LLVMPointerOwnerType* LLVMPointerType::getOwner() const {
  return mPointerOwnerType;
}

LLVMPointerType* LLVMPointerType::create(const ILLVMType* baseType) {
  return new LLVMPointerType(baseType, 2);
}

string LLVMPointerType::getTypeName() const {
  return mBaseType->getTypeName() + "::pointer";
}

llvm::PointerType* LLVMPointerType::getLLVMType(IRGenerationContext& context) const {
  return mBaseType->getLLVMType(context)->getPointerTo();
}

bool LLVMPointerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == PrimitiveTypes::BOOLEAN || toType == PrimitiveTypes::LONG) {
    return true;
  }
  if (toType == PrimitiveTypes::STRING && mBaseType == LLVMPrimitiveTypes::I8) {
    return true;
  }
  return toType->isReference() || toType->isPointer();
}

bool LLVMPointerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

llvm::Value* LLVMPointerType::castTo(IRGenerationContext& context,
                                 llvm::Value* fromValue,
                                 const IType* toType,
                                 int line) const {
  if (toType->isReference() || toType->isPointer()) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  if (toType == PrimitiveTypes::LONG) {
    return IRWriter::newPtrToIntInst(context, fromValue, toType->getLLVMType(context), "");
  }
  if (toType == PrimitiveTypes::STRING && mBaseType == LLVMPrimitiveTypes::I8) {
    return fromValue;
  }
  if (toType == PrimitiveTypes::BOOLEAN) {
    return IRWriter::newICmpInst(context,
                                 llvm::ICmpInst::ICMP_NE,
                                 fromValue,
                                 llvm::ConstantPointerNull::get(getLLVMType(context)),
                                 "");
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
  return false;
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

bool LLVMPointerType::isNative() const {
  return true;
}

bool LLVMPointerType::isPointer() const {
  return true;
}

bool LLVMPointerType::isImmutable() const {
  return false;
}

void LLVMPointerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMPointerType::createLocalVariable(IRGenerationContext& context,
                                          string name,
                                          int line) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  
  llvm::Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* variable = new LocalPointerVariable(name, this, alloca, line);
  context.getScopes().setVariable(context, variable);
}

void LLVMPointerType::createFieldVariable(IRGenerationContext& context,
                                          string name,
                                          const IConcreteObjectType* object,
                                          int line) const {
  IVariable* variable = new FieldPointerVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void LLVMPointerType::createParameterVariable(IRGenerationContext& context,
                                              string name,
                                              llvm::Value* value,
                                              int line) const {
  IVariable* variable = new ParameterPointerVariable(name, this, value, line);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* LLVMPointerType::getArrayType(IRGenerationContext& context,
                                                      int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

const LLVMPointerType* LLVMPointerType::getPointerType(IRGenerationContext& context,
                                                       int line) const {
  if (mPointerType) {
    return mPointerType;
  }
  context.reportError(line, "Three and more degree llvm pointers are not supported");
  throw 1;
}

llvm::Instruction* LLVMPointerType::inject(IRGenerationContext& context,
                                           const InjectionArgumentList injectionArgumentList,
                                           int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
