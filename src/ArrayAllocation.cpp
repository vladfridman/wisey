//
//  ArrayAllocation.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/21/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayAllocation.hpp"
#include "wisey/ArrayOwnerType.hpp"
#include "wisey/IVariable.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalArrayOwnerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ArrayAllocation::ArrayAllocation(const ArraySpecificTypeSpecifier* arraySpecificTypeSpecifier) :
mArraySpecificTypeSpecifier(arraySpecificTypeSpecifier) {
}

ArrayAllocation::~ArrayAllocation() {
  delete mArraySpecificTypeSpecifier;
}

Value* ArrayAllocation::generateIR(IRGenerationContext &context, const IType* assignToType) const {
  ArraySpecificType* arraySpecificType = mArraySpecificTypeSpecifier->getType(context);
  Value* arrayPointer = allocateArray(context, arraySpecificType);
  
  if (assignToType->isOwner()) {
    return arrayPointer;
  }

  Value* alloc = IRWriter::newAllocaInst(context, arrayPointer->getType(), "");
  IRWriter::newStoreInst(context, arrayPointer, alloc);
  const ArrayOwnerType* arrayOwnerType = arraySpecificType->getArrayType(context)->getOwner();
  string variableName = IVariable::getTemporaryVariableName(this);
  IVariable* variable = new LocalArrayOwnerVariable(variableName, arrayOwnerType, alloc);
  context.getScopes().setVariable(variable);
  
  return arrayPointer;
}

Value* ArrayAllocation::allocateArray(IRGenerationContext& context,
                                      const ArraySpecificType* arraySpecificType) {
  PointerType* arrayPointerType = arraySpecificType->getLLVMType(context);
  StructType* structType = (StructType*) arrayPointerType->getPointerElementType();
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  Instruction* malloc = IRWriter::createMalloc(context, structType, allocSize, "newarray");
  IntrinsicFunctions::setMemoryToZero(context, malloc, structType);
  
  list<unsigned long> dimensions;
  for (unsigned long dimension : arraySpecificType->getDimensions()) {
    dimensions.push_back(dimension);
  }
  
  initializeEmptyArray(context, malloc, dimensions);
  
  return malloc;
}

void ArrayAllocation::initializeEmptyArray(IRGenerationContext& context,
                                           Value* arrayPointer,
                                           std::list<unsigned long> dimensions) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Value* sizeStore = IRWriter::createGetElementPtrInst(context, arrayPointer, index);
  unsigned long arraySize = dimensions.front();
  ConstantInt* sizeValue = ConstantInt::get(Type::getInt64Ty(llvmContext), arraySize);
  IRWriter::newStoreInst(context, sizeValue, sizeStore);
  
  if (dimensions.size() == 1) {
    return;
  }
  
  dimensions.pop_front();
  
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), ArrayType::ARRAY_ELEMENTS_START_INDEX);
  Value* arrayStore = IRWriter::createGetElementPtrInst(context, arrayPointer, index);

  for (unsigned int i = 0; i < arraySize; i++) {
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), i);
    Value* arrayElement = IRWriter::createGetElementPtrInst(context, arrayStore, index);
    initializeEmptyArray(context, arrayElement, dimensions);
  }
}

IVariable* ArrayAllocation::getVariable(IRGenerationContext &context,
                                        vector<const IExpression *> &arrayIndices) const {
  return NULL;
}

bool ArrayAllocation::isConstant() const {
  return false;
}

const IType* ArrayAllocation::getType(IRGenerationContext& context) const {
  return mArraySpecificTypeSpecifier->getType(context)->getArrayType(context)->getOwner();
}

void ArrayAllocation::printToStream(IRGenerationContext &context, iostream &stream) const {
  stream << "new ";
  mArraySpecificTypeSpecifier->printToStream(context, stream);
}
