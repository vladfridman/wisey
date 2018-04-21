//
//  ArrayOwnerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/ArrayOwnerType.hpp"
#include "wisey/CheckArrayNotReferencedFunction.hpp"
#include "wisey/DestroyOwnerArrayFunction.hpp"
#include "wisey/DestroyPrimitiveArrayFunction.hpp"
#include "wisey/DestroyReferenceArrayFunction.hpp"
#include "wisey/FieldArrayOwnerVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ImmutableArrayType.hpp"
#include "wisey/LocalArrayOwnerVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/ParameterArrayOwnerVariable.hpp"

using namespace std;
using namespace wisey;

ArrayOwnerType::ArrayOwnerType(const ArrayType* arrayType) :
mArrayType(arrayType) {
}

ArrayOwnerType::~ArrayOwnerType() {
}

const ArrayType* ArrayOwnerType::getArrayType(IRGenerationContext& context) const {
  return mArrayType;
}

string ArrayOwnerType::getTypeName() const {
  return mArrayType->getTypeName() + "*";
}

llvm::PointerType* ArrayOwnerType::getLLVMType(IRGenerationContext& context) const {
  return mArrayType->getLLVMType(context);
}

bool ArrayOwnerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this || toType == mArrayType || toType == mArrayType->getImmutable()->getOwner()) {
    return true;
  }

  return false;
}

bool ArrayOwnerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

llvm::Value* ArrayOwnerType::castTo(IRGenerationContext &context,
                                    llvm::Value* fromValue,
                                    const IType* toType,
                                    int line) const {
  if (toType == this || toType == mArrayType) {
    return fromValue;
  }
  if (toType == mArrayType->getImmutable()->getOwner()) {
    CheckArrayNotReferencedFunction::callWithArrayType(context, fromValue, mArrayType);
    return fromValue;
  }

  return NULL;
}

void ArrayOwnerType::free(IRGenerationContext& context, llvm::Value* arrayPointer, int line) const {
  const IType* elementType = mArrayType->getElementType();
  llvm::Type* genericPointer = llvm::Type::getInt64Ty(context.getLLVMContext())->getPointerTo();
  llvm::Value* arrayBitcast = IRWriter::newBitCastInst(context, arrayPointer, genericPointer);
  
  if (elementType->isOwner()) {
    DestroyOwnerArrayFunction::call(context, arrayBitcast, mArrayType->getNumberOfDimensions());
  } else if (elementType->isReference()) {
    DestroyReferenceArrayFunction::call(context, arrayBitcast, mArrayType->getNumberOfDimensions());
  } else {
    assert(elementType->isPrimitive());
    DestroyPrimitiveArrayFunction::call(context,
                                        arrayBitcast,
                                        mArrayType->getNumberOfDimensions());
  }
}

bool ArrayOwnerType::isPrimitive() const {
  return false;
}

bool ArrayOwnerType::isOwner() const {
  return true;
}

bool ArrayOwnerType::isReference() const {
  return false;
}

bool ArrayOwnerType::isArray() const {
  return true;
}

bool ArrayOwnerType::isFunction() const {
  return false;
}

bool ArrayOwnerType::isPackage() const {
  return false;
}

bool ArrayOwnerType::isController() const {
  return false;
}

bool ArrayOwnerType::isInterface() const {
  return false;
}

bool ArrayOwnerType::isModel() const {
  return false;
}

bool ArrayOwnerType::isNode() const {
  return false;
}

bool ArrayOwnerType::isThread() const {
  return false;
}

bool ArrayOwnerType::isNative() const {
  return false;
}

bool ArrayOwnerType::isPointer() const {
  return false;
}

void ArrayOwnerType::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getTypeName();
}

void ArrayOwnerType::createLocalVariable(IRGenerationContext& context, string name) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  llvm::AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloc);
  
  IVariable* variable = new LocalArrayOwnerVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
}

void ArrayOwnerType::createFieldVariable(IRGenerationContext& context,
                                         string name,
                                         const IConcreteObjectType* object) const {
  IVariable* variable = new FieldArrayOwnerVariable(name, object);
  context.getScopes().setVariable(variable);
}

void ArrayOwnerType::createParameterVariable(IRGenerationContext& context,
                                             string name,
                                             llvm::Value* value) const {
  llvm::Type* llvmType = getLLVMType(context);
  llvm::Value* alloc = IRWriter::newAllocaInst(context, llvmType, "parameterArrayPointer");
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterArrayOwnerVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
}

const IReferenceType* ArrayOwnerType::getReference() const {
  return NULL;
}

llvm::Instruction* ArrayOwnerType::inject(IRGenerationContext& context,
                                          const InjectionArgumentList injectionArgumentList,
                                          int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
