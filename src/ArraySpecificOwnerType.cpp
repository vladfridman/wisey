//
//  ArraySpecificOwnerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "ArrayAllocation.hpp"
#include "ArraySpecificOwnerType.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"

using namespace std;
using namespace wisey;

ArraySpecificOwnerType::ArraySpecificOwnerType(const ArraySpecificType* arraySpecificType) :
mArraySpecificType(arraySpecificType) {
}

ArraySpecificOwnerType::~ArraySpecificOwnerType() {
}

const ArraySpecificType* ArraySpecificOwnerType::getArraySpecificType() const {
  return mArraySpecificType;
}

string ArraySpecificOwnerType::getTypeName() const {
  return mArraySpecificType->getTypeName() + "*";
}

llvm::PointerType* ArraySpecificOwnerType::getLLVMType(IRGenerationContext& context) const {
  return mArraySpecificType->getLLVMType(context);
}

bool ArraySpecificOwnerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this) {
    return true;
  }
  
  if (!toType->isArray()) {
    return false;
  }
  
  const ArrayType* toArrayType = toType->getArrayType(context, 0);
  
  return toArrayType->getElementType() == mArraySpecificType->getElementType() &&
  toArrayType->getNumberOfDimensions() == mArraySpecificType->getNumberOfDimensions();
}

bool ArraySpecificOwnerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

llvm::Value* ArraySpecificOwnerType::castTo(IRGenerationContext &context,
                                            llvm::Value* fromValue,
                                            const IType* toType,
                                            int line) const {
  if (toType == this) {
    return fromValue;
  }
  
  return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
}

void ArraySpecificOwnerType::free(IRGenerationContext& context,
                                  llvm::Value* arrayPointer,
                                  llvm::Value* exception,
                                  const LLVMFunction* customDestructor,
                                  int line) const {
  // This should never be called
  assert(false);
}

bool ArraySpecificOwnerType::isPrimitive() const {
  return false;
}

bool ArraySpecificOwnerType::isOwner() const {
  return true;
}

bool ArraySpecificOwnerType::isReference() const {
  return false;
}

bool ArraySpecificOwnerType::isArray() const {
  return true;
}

bool ArraySpecificOwnerType::isFunction() const {
  return false;
}

bool ArraySpecificOwnerType::isPackage() const {
  return false;
}

bool ArraySpecificOwnerType::isController() const {
  return false;
}

bool ArraySpecificOwnerType::isInterface() const {
  return false;
}

bool ArraySpecificOwnerType::isModel() const {
  return false;
}

bool ArraySpecificOwnerType::isNode() const {
  return false;
}

bool ArraySpecificOwnerType::isNative() const {
  return false;
}

bool ArraySpecificOwnerType::isPointer() const {
  return false;
}

bool ArraySpecificOwnerType::isImmutable() const {
  return false;
}

void ArraySpecificOwnerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  mArraySpecificType->printToStream(context, stream);
  stream << "*";
}

void ArraySpecificOwnerType::createLocalVariable(IRGenerationContext& context,
                                                 string name,
                                                 int line) const {
  assert(false);
}

void ArraySpecificOwnerType::createFieldVariable(IRGenerationContext& context,
                                                 string name,
                                                 const IConcreteObjectType* object,
                                                 int line) const {
  assert(false);
}

void ArraySpecificOwnerType::createParameterVariable(IRGenerationContext& context,
                                                     string name,
                                                     llvm::Value* value,
                                                     int line) const {
  assert(false);
}

const ArrayType* ArraySpecificOwnerType::getArrayType(IRGenerationContext& context, int line) const {
  return mArraySpecificType->getArrayType(context, line);
}

const IReferenceType* ArraySpecificOwnerType::getReference() const {
  return NULL;
}

llvm::Instruction* ArraySpecificOwnerType::inject(IRGenerationContext& context,
                                                  const InjectionArgumentList injectionArgumentList,
                                                  int line) const {
  llvm::Value* arrayPointer = ArrayAllocation::allocateArray(context, mArraySpecificType, line);
  llvm::Type* arrayLLVMType = mArraySpecificType->getArrayType(context, line)->getLLVMType(context);
  return IRWriter::newBitCastInst(context, arrayPointer, arrayLLVMType);
}
