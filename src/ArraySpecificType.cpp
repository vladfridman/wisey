//
//  ArraySpecificType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/ArraySpecificType.hpp"
#include "wisey/IRWriter.hpp"

using namespace std;
using namespace wisey;

ArraySpecificType::ArraySpecificType(const IType* elementType, list<unsigned long> dimensions) :
mElementType(elementType), mDimensions(dimensions) {
  assert(dimensions.size());
  mArraySpecificOwnerType = new ArraySpecificOwnerType(this);
}

ArraySpecificType::~ArraySpecificType() {
  delete mArraySpecificOwnerType;
}

const ArraySpecificOwnerType* ArraySpecificType::getOwner() const {
  return mArraySpecificOwnerType;
}

wisey::ArrayType* ArraySpecificType::getArrayType(IRGenerationContext& context) const {
  return context.getArrayType(mElementType, mDimensions.size());
}

const IType* ArraySpecificType::getElementType() const {
  return mElementType;
}

string ArraySpecificType::getTypeName() const {
  string name = mElementType->getTypeName();
  for (unsigned long dimension : mDimensions) {
    name = name + "[" + to_string(dimension) + "]";
  }
  return name;
}

llvm::PointerType* ArraySpecificType::getLLVMType(IRGenerationContext& context) const {
  return getArrayType(context)->getLLVMType(context);
}

list<tuple<llvm::Value*, llvm::Value*>>
ArraySpecificType::computeArrayAllocData(IRGenerationContext& context) const {
  llvm::LLVMContext& llvmContext = context.getLLVMContext();
  llvm::Type* int64Type = llvm::Type::getInt64Ty(llvmContext);
  llvm::Value* headerSize = llvm::ConstantInt::get(int64Type, 8 * 3);
  list<tuple<llvm::Value*, llvm::Value*>> result;
  
  llvm::Value* elementSize = llvm::ConstantExpr::getSizeOf(mElementType->getLLVMType(context));
  llvm::Value* sizeStore = IRWriter::newAllocaInst(context, int64Type, "arraySize");
  IRWriter::newStoreInst(context, elementSize, sizeStore);
  list<unsigned long> dimensionsReversed = mDimensions;
  dimensionsReversed.reverse();
  for (unsigned long dimension : dimensionsReversed) {
    llvm::Value* dimensionValue = llvm::ConstantInt::get(int64Type, dimension);
    llvm::Value* sizeValue = IRWriter::newLoadInst(context, sizeStore, "size");
    result.push_front(make_tuple(dimensionValue, sizeValue));
    llvm::Value* newSize = IRWriter::createBinaryOperator(context,
                                                          llvm::Instruction::Mul,
                                                          dimensionValue,
                                                          sizeValue,
                                                          "");
    llvm::Value* withHeader = IRWriter::createBinaryOperator(context,
                                                             llvm::Instruction::Add,
                                                             newSize,
                                                             headerSize,
                                                             "");
    IRWriter::newStoreInst(context, withHeader, sizeStore);
  }
  llvm::Value* arraySize = IRWriter::newLoadInst(context, sizeStore, "");
  result.push_front(make_tuple(llvm::ConstantInt::get(int64Type, 1), arraySize));
  return result;
}

TypeKind ArraySpecificType::getTypeKind() const {
  return ARRAY_TYPE;
}

bool ArraySpecificType::canCastTo(const IType *toType) const {
  return toType == this;
}

bool ArraySpecificType::canAutoCastTo(const IType *toType) const {
  return toType == this;
}

llvm::Value* ArraySpecificType::castTo(IRGenerationContext &context,
                                       llvm::Value* fromValue,
                                       const IType* toType,
                                       int line) const {
  if (toType == this) {
    return fromValue;
  }
  
  return NULL;
}

unsigned long ArraySpecificType::getNumberOfDimensions() const {
  return mDimensions.size();
}

bool ArraySpecificType::isOwner() const {
  return false;
}

