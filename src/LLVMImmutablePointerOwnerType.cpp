//
//  LLVMImmutablePointerOwnerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/DestroyNativeObjectFunction.hpp"
#include "wisey/FieldOwnerVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LLVMImmutablePointerOwnerType.hpp"
#include "wisey/LLVMImmutablePointerType.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/ParameterOwnerVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMImmutablePointerOwnerType::LLVMImmutablePointerOwnerType(const LLVMImmutablePointerType*
                                                             immutablePointerType) :
mImmutablePointerType(immutablePointerType) {
}

LLVMImmutablePointerOwnerType::~LLVMImmutablePointerOwnerType() {
}

string LLVMImmutablePointerOwnerType::getTypeName() const {
  return mImmutablePointerType->getTypeName() + "*";
}

llvm::PointerType* LLVMImmutablePointerOwnerType::getLLVMType(IRGenerationContext& context) const {
  return mImmutablePointerType->getLLVMType(context);
}

bool LLVMImmutablePointerOwnerType::canCastTo(IRGenerationContext& context,
                                              const IType* toType) const {
  if (toType == this) {
    return true;
  }
  if (toType->isPointer() && toType->isNative() && toType->isImmutable() && !toType->isOwner()) {
    const LLVMImmutablePointerType* pointer = (const LLVMImmutablePointerType*) toType;
    return pointer->getBaseType() == mImmutablePointerType->getBaseType();
  }
  return false;
}

bool LLVMImmutablePointerOwnerType::canAutoCastTo(IRGenerationContext& context,
                                                  const IType* toType) const {
  return canCastTo(context, toType);
}

llvm::Value* LLVMImmutablePointerOwnerType::castTo(IRGenerationContext& context,
                                                   llvm::Value* fromValue,
                                                   const IType* toType,
                                                   int line) const {
  if (toType == this) {
    return fromValue;
  }
  if (toType->isPointer() && toType->isNative() && toType->isImmutable() && !toType->isOwner()) {
    return fromValue;
  }
  assert(false && "Cast of immutable pointer owner type is impossible");
}

bool LLVMImmutablePointerOwnerType::isPrimitive() const {
  return false;
}

bool LLVMImmutablePointerOwnerType::isOwner() const {
  return true;
}

bool LLVMImmutablePointerOwnerType::isReference() const {
  return false;
}

bool LLVMImmutablePointerOwnerType::isArray() const {
  return false;
}

bool LLVMImmutablePointerOwnerType::isFunction() const {
  return false;
}

bool LLVMImmutablePointerOwnerType::isPackage() const {
  return false;
}

bool LLVMImmutablePointerOwnerType::isController() const {
  return false;
}

bool LLVMImmutablePointerOwnerType::isInterface() const {
  return false;
}

bool LLVMImmutablePointerOwnerType::isModel() const {
  return false;
}

bool LLVMImmutablePointerOwnerType::isNode() const {
  return false;
}

bool LLVMImmutablePointerOwnerType::isNative() const {
  return true;
}

bool LLVMImmutablePointerOwnerType::isPointer() const {
  return true;
}

bool LLVMImmutablePointerOwnerType::isImmutable() const {
  return true;
}

void LLVMImmutablePointerOwnerType::printToStream(IRGenerationContext &context,
                                                  iostream& stream) const {
  stream << getTypeName();
}

void LLVMImmutablePointerOwnerType::createLocalVariable(IRGenerationContext& context,
                                                        string name,
                                                        int line) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  llvm::Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  IVariable* variable = new LocalOwnerVariable(name, this, alloca, line);
  context.getScopes().setVariable(context, variable);
}

void LLVMImmutablePointerOwnerType::createFieldVariable(IRGenerationContext& context,
                                                        string name,
                                                        const IConcreteObjectType* object,
                                                        int line) const {
  IVariable* variable = new FieldOwnerVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void LLVMImmutablePointerOwnerType::createParameterVariable(IRGenerationContext& context,
                                                            string name,
                                                            llvm::Value* value,
                                                            int line) const {
  llvm::PointerType::Type* llvmType = getLLVMType(context);
  Value* alloc = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterOwnerVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* LLVMImmutablePointerOwnerType::getArrayType(IRGenerationContext& context,
                                                                    int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

const IReferenceType* LLVMImmutablePointerOwnerType::getReference() const {
  return NULL;
}

void LLVMImmutablePointerOwnerType::free(IRGenerationContext& context,
                                         Value* value,
                                         llvm::Value* exception,
                                         const LLVMFunction* customDestructor,
                                         int line) const {
  if (!customDestructor) {
    DestroyNativeObjectFunction::call(context, value);
    return;
  }
  
  vector<Value*> arguments;
  arguments.push_back(value);
  Function* destructorFunction = customDestructor->getLLVMFunction(context);
  IRWriter::createCallInst(context, destructorFunction, arguments, "");
}

Instruction* LLVMImmutablePointerOwnerType::inject(IRGenerationContext& context,
                                                   const InjectionArgumentList
                                                   injectionArgumentList,
                                                   int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
