//
//  ImmutableImmutableArrayOwnerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/DestroyOwnerArrayFunction.hpp"
#include "wisey/DestroyPrimitiveArrayFunction.hpp"
#include "wisey/DestroyReferenceArrayFunction.hpp"
#include "wisey/FieldImmutableArrayOwnerVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ImmutableArrayOwnerType.hpp"
#include "wisey/ImmutableArrayType.hpp"
#include "wisey/LocalImmutableArrayOwnerVariable.hpp"
#include "wisey/ParameterImmutableArrayOwnerVariable.hpp"

using namespace std;
using namespace wisey;

ImmutableArrayOwnerType::ImmutableArrayOwnerType(const ImmutableArrayType* immutableArrayType) :
mImmutableArrayType(immutableArrayType) {
}

ImmutableArrayOwnerType::~ImmutableArrayOwnerType() {
}

const ArrayType* ImmutableArrayOwnerType::getArrayType(IRGenerationContext& context, int line) const {
  return mImmutableArrayType->getArrayType(context, line);
}

string ImmutableArrayOwnerType::getTypeName() const {
  return mImmutableArrayType->getTypeName() + "*";
}

llvm::PointerType* ImmutableArrayOwnerType::getLLVMType(IRGenerationContext& context) const {
  return mImmutableArrayType->getLLVMType(context);
}

bool ImmutableArrayOwnerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this || toType == mImmutableArrayType) {
    return true;
  }
  if (toType->isPointer() && toType->isNative()) {
    return true;
  }

  return false;
}

bool ImmutableArrayOwnerType::canAutoCastTo(IRGenerationContext& context,
                                            const IType* toType) const {
  return canCastTo(context, toType);
}

llvm::Value* ImmutableArrayOwnerType::castTo(IRGenerationContext &context,
                                             llvm::Value* fromValue,
                                             const IType* toType,
                                             int line) const {
  if (toType == this || toType == mImmutableArrayType) {
    return fromValue;
  }
  if (toType->isPointer() && toType->isNative()) {
    llvm::Value* arrayStart = ArrayType::extractLLVMArray(context, fromValue);
    return IRWriter::newBitCastInst(context, arrayStart, toType->getLLVMType(context));
  }
  return NULL;
}

void ImmutableArrayOwnerType::free(IRGenerationContext& context,
                                   llvm::Value* arrayPointer,
                                   int line) const {
  const ArrayType* arrayType = getArrayType(context, line);
  
  const IType* elementType = arrayType->getElementType();
  llvm::Type* genericPointer = llvm::Type::getInt64Ty(context.getLLVMContext())->getPointerTo();
  llvm::Value* arrayBitcast = IRWriter::newBitCastInst(context, arrayPointer, genericPointer);
  long dimensions = arrayType->getNumberOfDimensions();
  llvm::Value* arrayNamePointer = getArrayNamePointer(context);
  
  if (elementType->isOwner()) {
    DestroyOwnerArrayFunction::call(context, arrayBitcast, dimensions, arrayNamePointer);
  } else if (elementType->isReference()) {
    DestroyReferenceArrayFunction::call(context, arrayBitcast, dimensions, arrayNamePointer);
  } else {
    assert(elementType->isPrimitive());
    DestroyPrimitiveArrayFunction::call(context, arrayBitcast, dimensions, arrayNamePointer);
  }
}

bool ImmutableArrayOwnerType::isPrimitive() const {
  return false;
}

bool ImmutableArrayOwnerType::isOwner() const {
  return true;
}

bool ImmutableArrayOwnerType::isReference() const {
  return false;
}

bool ImmutableArrayOwnerType::isArray() const {
  return true;
}

bool ImmutableArrayOwnerType::isFunction() const {
  return false;
}

bool ImmutableArrayOwnerType::isPackage() const {
  return false;
}

bool ImmutableArrayOwnerType::isController() const {
  return false;
}

bool ImmutableArrayOwnerType::isInterface() const {
  return false;
}

bool ImmutableArrayOwnerType::isModel() const {
  return false;
}

bool ImmutableArrayOwnerType::isNode() const {
  return false;
}

bool ImmutableArrayOwnerType::isNative() const {
  return false;
}

bool ImmutableArrayOwnerType::isPointer() const {
  return false;
}

bool ImmutableArrayOwnerType::isImmutable() const {
  return true;
}

void ImmutableArrayOwnerType::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getTypeName();
}

void ImmutableArrayOwnerType::createLocalVariable(IRGenerationContext& context,
                                                  string name,
                                                  int line) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  llvm::AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloc);
  
  IVariable* variable = new LocalImmutableArrayOwnerVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
}

void ImmutableArrayOwnerType::createFieldVariable(IRGenerationContext& context,
                                                  string name,
                                                  const IConcreteObjectType* object,
                                                  int line) const {
  IVariable* variable = new FieldImmutableArrayOwnerVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void ImmutableArrayOwnerType::createParameterVariable(IRGenerationContext& context,
                                                      string name,
                                                      llvm::Value* value,
                                                      int line) const {
  llvm::Type* llvmType = getLLVMType(context);
  llvm::Value* alloc = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterImmutableArrayOwnerVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
}

const IReferenceType* ImmutableArrayOwnerType::getReference() const {
  return NULL;
}

llvm::Instruction* ImmutableArrayOwnerType::inject(IRGenerationContext& context,
                                                   const InjectionArgumentList injectionArguments,
                                                   int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}

llvm::Value* ImmutableArrayOwnerType::getArrayNamePointer(IRGenerationContext& context) const {
  llvm::LLVMContext& llvmContext = context.getLLVMContext();
  string typeName = getTypeName();
  
  llvm::GlobalVariable* nameGlobal = context.getModule()->getNamedGlobal(typeName);
  if (!nameGlobal) {
    llvm::Constant* stringConstant = llvm::ConstantDataArray::getString(llvmContext, typeName);
    
    llvm::Type* charArrayType = llvm::ArrayType::get(llvm::Type::getInt8Ty(llvmContext),
                                                     typeName.length() + 1);
    nameGlobal = new llvm::GlobalVariable(*context.getModule(),
                                          charArrayType,
                                          true,
                                          llvm::GlobalValue::LinkageTypes::ExternalLinkage,
                                          stringConstant,
                                          typeName);
  }
  llvm::ConstantInt* zeroInt32 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvmContext), 0);
  llvm::Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  llvm::Type* elementType = nameGlobal->getType()->getPointerElementType();
  
  return llvm::ConstantExpr::getGetElementPtr(elementType, nameGlobal, Idx);
}
