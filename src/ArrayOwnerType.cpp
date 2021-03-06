//
//  ArrayOwnerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "ArrayType.hpp"
#include "ArrayOwnerType.hpp"
#include "CheckArrayNotReferencedFunction.hpp"
#include "DestroyOwnerArrayFunction.hpp"
#include "DestroyPrimitiveArrayFunction.hpp"
#include "DestroyReferenceArrayFunction.hpp"
#include "FieldArrayOwnerVariable.hpp"
#include "GlobalStringConstant.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "ImmutableArrayType.hpp"
#include "LocalArrayOwnerVariable.hpp"
#include "Log.hpp"
#include "ParameterArrayOwnerVariable.hpp"
#include "PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

ArrayOwnerType::ArrayOwnerType(const ArrayType* arrayType) :
mArrayType(arrayType) {
}

ArrayOwnerType::~ArrayOwnerType() {
}

const ArrayType* ArrayOwnerType::getArrayType(IRGenerationContext& context, int line) const {
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
  if (toType->isPointer() && toType->isNative()) {
    return true;
  }
  if (toType == PrimitiveTypes::BOOLEAN) {
    return true;
  }

  return false;
}

bool ArrayOwnerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

llvm::Value* ArrayOwnerType::castTo(IRGenerationContext& context,
                                    llvm::Value* fromValue,
                                    const IType* toType,
                                    int line) const {
  if (toType == this || toType == mArrayType) {
    return fromValue;
  }
  if (toType->isPointer() && toType->isNative()) {
    llvm::Value* arrayStart = ArrayType::extractLLVMArray(context, fromValue);
    return IRWriter::newBitCastInst(context, arrayStart, toType->getLLVMType(context));
  }
  if (toType == mArrayType->getImmutable()->getOwner()) {
    llvm::LLVMContext& llvmContext = context.getLLVMContext();
    llvm::Type* int64type = llvm::Type::getInt64Ty(llvmContext);
    long dimensions = mArrayType->getNumberOfDimensions();
    llvm::Value* dimensionsConstant = llvm::ConstantInt::get(int64type, dimensions);
    llvm::Value* arrayNamePointer = getArrayNamePointer(context);
    CheckArrayNotReferencedFunction::call(context,
                                          fromValue,
                                          dimensionsConstant,
                                          arrayNamePointer,
                                          NULL,
                                          line);
    return fromValue;
  }
  if (toType == PrimitiveTypes::BOOLEAN) {
    return IRWriter::newICmpInst(context,
                                 llvm::ICmpInst::ICMP_NE,
                                 fromValue,
                                 llvm::ConstantPointerNull::get(getLLVMType(context)),
                                 "");
  }

  return NULL;
}

void ArrayOwnerType::free(IRGenerationContext& context,
                          llvm::Value* arrayPointer,
                          llvm::Value* exception,
                          const LLVMFunction* customDestructor,
                          int line) const {
  const IType* elementType = mArrayType->getElementType();
  llvm::Type* genericPointer = llvm::Type::getInt64Ty(context.getLLVMContext())->getPointerTo();
  llvm::Value* arrayBitcast = IRWriter::newBitCastInst(context, arrayPointer, genericPointer);
  long dimensions = mArrayType->getNumberOfDimensions();
  llvm::Value* arrayNamePointer = getArrayNamePointer(context);

  if (elementType->isOwner()) {
    DestroyOwnerArrayFunction::call(context,
                                    arrayBitcast,
                                    dimensions,
                                    arrayNamePointer,
                                    exception,
                                    line);
  } else if (elementType->isReference()) {
    DestroyReferenceArrayFunction::call(context,
                                        arrayBitcast,
                                        dimensions,
                                        arrayNamePointer,
                                        exception,
                                        line);
  } else {
    assert(elementType->isPrimitive() || elementType->isPointer());
    DestroyPrimitiveArrayFunction::call(context,
                                        arrayBitcast,
                                        dimensions,
                                        arrayNamePointer,
                                        exception,
                                        line);
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

bool ArrayOwnerType::isNative() const {
  return false;
}

bool ArrayOwnerType::isPointer() const {
  return false;
}

bool ArrayOwnerType::isImmutable() const {
  return false;
}

void ArrayOwnerType::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getTypeName();
}

void ArrayOwnerType::createLocalVariable(IRGenerationContext& context,
                                         string name,
                                         int line) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  llvm::AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloc);
  
  IVariable* variable = new LocalArrayOwnerVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
}

void ArrayOwnerType::createFieldVariable(IRGenerationContext& context,
                                         string name,
                                         const IConcreteObjectType* object,
                                         int line) const {
  IVariable* variable = new FieldArrayOwnerVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void ArrayOwnerType::createParameterVariable(IRGenerationContext& context,
                                             string name,
                                             llvm::Value* value,
                                             int line) const {
  llvm::Type* llvmType = getLLVMType(context);
  llvm::Value* alloc = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterArrayOwnerVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
}

const IReferenceType* ArrayOwnerType::getReference() const {
  return NULL;
}

llvm::Instruction* ArrayOwnerType::inject(IRGenerationContext& context,
                                          const InjectionArgumentList injectionArgumentList,
                                          int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}

llvm::Value* ArrayOwnerType::getArrayNamePointer(IRGenerationContext& context) const {
  return GlobalStringConstant::get(context, getTypeName());
}
