//
//  ArrayExactType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/ArrayExactType.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/LocalArrayOwnerVariable.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace wisey;

ArrayExactType::ArrayExactType(const IType* elementType, list<unsigned long> dimensions) :
mElementType(elementType), mDimensions(dimensions) {
  assert(dimensions.size());
}

ArrayExactType::~ArrayExactType() {
}

list<unsigned long> ArrayExactType::getDimensions() const {
  return mDimensions;
}

const IType* ArrayExactType::getElementType() const {
  return mElementType;
}

string ArrayExactType::getTypeName() const {
  string name = mElementType->getTypeName();
  for (unsigned long dimension : mDimensions) {
    name = name + "[" + to_string(dimension) + "]";
  }
  return name;
}

llvm::StructType* ArrayExactType::getLLVMType(IRGenerationContext& context) const {
  llvm::LLVMContext& llvmContext = context.getLLVMContext();
  
  llvm::Type* elementType = mElementType->getLLVMType(context);
  llvm::StructType* structType = NULL;
  list<unsigned long> dimensionsReversed = mDimensions;
  dimensionsReversed.reverse();
  for (unsigned long dimension : dimensionsReversed) {
    llvm::ArrayType* arrayType = llvm::ArrayType::get(structType ? structType : elementType,
                                                      dimension);
    vector<llvm::Type*> types;
    types.push_back(llvm::Type::getInt64Ty(llvmContext));
    types.push_back(llvm::Type::getInt64Ty(llvmContext));
    types.push_back(llvm::Type::getInt64Ty(llvmContext));
    types.push_back(arrayType);
    structType = llvm::StructType::get(llvmContext, types);
  }
  
  return structType;
}

bool ArrayExactType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this ||
  getArrayType(context, 0) == toType ||
  getArrayType(context, 0)->getOwner() == toType;
}

bool ArrayExactType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

llvm::Value* ArrayExactType::castTo(IRGenerationContext &context,
                                    llvm::Value* fromValue,
                                    const IType* toType,
                                    int line) const {
  if (toType == this) {
    return fromValue;
  }
  
  
  llvm::Value* malloc = allocateArray(context, fromValue, toType);
  
  if (toType->isOwner()) {
    return malloc;
  }
  
  string variableName = IVariable::getTemporaryVariableName(this);
  const ArrayOwnerType* arrayOwnerType = toType->getArrayType(context, line)->getOwner();
  llvm::Value* alloc = IRWriter::newAllocaInst(context, arrayOwnerType->getLLVMType(context), "");
  IRWriter::newStoreInst(context, malloc, alloc);
  IVariable* variable = new LocalArrayOwnerVariable(variableName,
                                                    toType->getArrayType(context, line)->getOwner(),
                                                    alloc,
                                                    line);
  context.getScopes().setVariable(context, variable);
  return malloc;
}

llvm::Value* ArrayExactType::allocateArray(IRGenerationContext& context,
                                           llvm::Value* staticArray,
                                           const IType* toType) const {
  llvm::StructType* structType = getLLVMType(context);
  llvm::Constant* allocSize = llvm::ConstantExpr::getSizeOf(structType);
  llvm::Constant* one = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.getLLVMContext()), 1);
  llvm::Instruction* malloc = IRWriter::createMalloc(context, structType, allocSize, one, "arr");
  IRWriter::newStoreInst(context, staticArray, malloc);

  return IRWriter::newBitCastInst(context, malloc, toType->getLLVMType(context));
}

unsigned long ArrayExactType::getNumberOfDimensions() const {
  return mDimensions.size();
}

bool ArrayExactType::isPrimitive() const {
  return false;
}

bool ArrayExactType::isOwner() const {
  return false;
}

bool ArrayExactType::isReference() const {
  return true;
}

bool ArrayExactType::isArray() const {
  return true;
}

bool ArrayExactType::isFunction() const {
  return false;
}

bool ArrayExactType::isPackage() const {
  return false;
}

bool ArrayExactType::isController() const {
  return false;
}

bool ArrayExactType::isInterface() const {
  return false;
}

bool ArrayExactType::isModel() const {
  return false;
}

bool ArrayExactType::isNode() const {
  return false;
}

bool ArrayExactType::isNative() const {
  return false;
}

bool ArrayExactType::isPointer() const {
  return false;
}

bool ArrayExactType::isImmutable() const {
  return false;
}

void ArrayExactType::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getTypeName();
}

void ArrayExactType::createLocalVariable(IRGenerationContext& context,
                                         string name,
                                         int line) const {
  assert(false);
}

void ArrayExactType::createFieldVariable(IRGenerationContext& context,
                                         string name,
                                         const IConcreteObjectType* object,
                                         int line) const {
  assert(false);
}

void ArrayExactType::createParameterVariable(IRGenerationContext& context,
                                             string name,
                                             llvm::Value* value,
                                             int line) const {
  assert(false);
}

const wisey::ArrayType* ArrayExactType::getArrayType(IRGenerationContext& context, int line) const {
  return context.getArrayType(mElementType, getNumberOfDimensions());
}

llvm::Instruction* ArrayExactType::inject(IRGenerationContext& context,
                                          const InjectionArgumentList injectionArgumentList,
                                          int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
