//
//  LLVMObjectType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCountFunction.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMObjectType.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/ParameterReferenceVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMObjectType* LLVMObjectType::LLVM_OBJECT_TYPE = new LLVMObjectType();

LLVMObjectType::LLVMObjectType() {
}

LLVMObjectType::~LLVMObjectType() {
}

string LLVMObjectType::getTypeName() const {
  return "::llvm::object";
}

PointerType* LLVMObjectType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
}

bool LLVMObjectType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isReference();
}

bool LLVMObjectType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isReference();
}

Value* LLVMObjectType::castTo(IRGenerationContext& context,
                              Value* fromValue,
                              const IType* toType,
                              int line) const {
  if (toType->isReference()) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  assert(false);
}

bool LLVMObjectType::isPrimitive() const {
  return false;
}

bool LLVMObjectType::isOwner() const {
  return false;
}

bool LLVMObjectType::isReference() const {
  return true;
}

bool LLVMObjectType::isArray() const {
  return false;
}

bool LLVMObjectType::isFunction() const {
  return false;
}

bool LLVMObjectType::isPackage() const {
  return false;
}

bool LLVMObjectType::isController() const {
  return false;
}

bool LLVMObjectType::isInterface() const {
  return false;
}

bool LLVMObjectType::isModel() const {
  return false;
}

bool LLVMObjectType::isNode() const {
  return false;
}

bool LLVMObjectType::isThread() const {
  return false;
}

bool LLVMObjectType::isNative() const {
  return true;
}

void LLVMObjectType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMObjectType::createLocalVariable(IRGenerationContext& context, string name) const {
  PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalReferenceVariable(name, this, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

void LLVMObjectType::createFieldVariable(IRGenerationContext& context,
                                         string name,
                                         const IConcreteObjectType* object) const {
  IVariable* variable = new FieldReferenceVariable(name, object);
  context.getScopes().setVariable(variable);
}

void LLVMObjectType::createParameterVariable(IRGenerationContext& context,
                                             string name,
                                             Value* value) const {
  IVariable* variable = new ParameterReferenceVariable(name, this, value);
  incrementReferenceCount(context, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* LLVMObjectType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IReferenceType* LLVMObjectType::getReferenceType() const {
  return NULL;
}

const ILLVMPointerType* LLVMObjectType::getPointerType() const {
  assert(false);
}

void LLVMObjectType::incrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCountFunction::call(context, object, 1);
}

void LLVMObjectType::decrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCountFunction::call(context, object, -1);
}
