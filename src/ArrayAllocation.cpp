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
  
  LLVMContext& llvmContext = context.getLLVMContext();
  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Value* dimensionsStore = IRWriter::createGetElementPtrInst(context, malloc, index);
  ConstantInt* dimensionsValue = ConstantInt::get(Type::getInt64Ty(llvmContext),
                                                  arraySpecificType->getDimentionsSize());
  IRWriter::newStoreInst(context, dimensionsValue, dimensionsStore);
  
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 2);
  Value* linearSizeStore = IRWriter::createGetElementPtrInst(context, malloc, index);
  ConstantInt* linearSizeValue = ConstantInt::get(Type::getInt64Ty(llvmContext),
                                                  arraySpecificType->getLinearSize());
  IRWriter::newStoreInst(context, linearSizeValue, linearSizeStore);
  
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
  Value* sizesStructurePointer = IRWriter::createGetElementPtrInst(context, malloc, index);
  
  unsigned long dimensionIndex = 0;
  for (unsigned long dimension : arraySpecificType->getDimensions()) {
    index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), 0);
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), dimensionIndex);
    Value* sizeStore = IRWriter::createGetElementPtrInst(context, sizesStructurePointer, index);
    ConstantInt* sizeValue = ConstantInt::get(Type::getInt64Ty(llvmContext), dimension);
    IRWriter::newStoreInst(context, sizeValue, sizeStore);
    dimensionIndex++;
  }
  
  return malloc;
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
