//
//  ArrayAllocation.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/21/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayAllocation.hpp"
#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/IVariable.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalArrayOwnerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ArrayAllocation::ArrayAllocation(const ArraySpecificTypeSpecifier* arraySpecificTypeSpecifier,
                                 int line) :
mArraySpecificTypeSpecifier(arraySpecificTypeSpecifier),
mLine(line) {
}

ArrayAllocation::~ArrayAllocation() {
  delete mArraySpecificTypeSpecifier;
}

int ArrayAllocation::getLine() const {
  return mLine;
}

Value* ArrayAllocation::generateIR(IRGenerationContext &context, const IType* assignToType) const {
  ArraySpecificType* arraySpecificType = mArraySpecificTypeSpecifier->getType(context);
  Value* arrayPointer = allocateArray(context, arraySpecificType, mLine);
  Value* arrayCast = IRWriter::newBitCastInst(context, arrayPointer, arraySpecificType->
                                              getArrayType(context, mLine)->getLLVMType(context));

  if (assignToType->isOwner()) {
    return arrayCast;
  }

  Value* alloc = IRWriter::newAllocaInst(context, arrayCast->getType(), "");
  IRWriter::newStoreInst(context, arrayCast, alloc);
  const ArrayOwnerType* arrayOwnerType =
  arraySpecificType->getArrayType(context, mLine)->getOwner();
  string variableName = IVariable::getTemporaryVariableName(this);
  IVariable* variable = new LocalArrayOwnerVariable(variableName, arrayOwnerType, alloc, mLine);
  context.getScopes().setVariable(context, variable);

  delete arraySpecificType;
  
  return arrayCast;
}

Value* ArrayAllocation::allocateArray(IRGenerationContext& context,
                                      const ArraySpecificType* arraySpecificType,
                                      int line) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* byteType = Type::getInt8Ty(llvmContext);
  Type* int64Type = Type::getInt64Ty(llvmContext);
  Value* one = ConstantInt::get(int64Type, 1);

  list<tuple<Value*, Value*>> arrayData = arraySpecificType->computeArrayAllocData(context, line);
  Value* allocSize = get<1>(arrayData.front());
  Instruction* malloc = IRWriter::createMalloc(context, byteType, one, allocSize, "newarray", line);
  IntrinsicFunctions::setMemoryToZero(context, malloc, allocSize);

  arrayData.pop_front();
  initializeEmptyArray(context, malloc, arrayData);
  
  return malloc;
}

void ArrayAllocation::initializeEmptyArray(IRGenerationContext& context,
                                           Value* arrayStructPointer,
                                           list<tuple<Value*, Value*>> arrayData) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);
  Type* int64Type = Type::getInt64Ty(llvmContext);
  Value* zero = ConstantInt::get(int64Type, 0);
  Value* one = ConstantInt::get(int64Type, 1);
  Value* two = ConstantInt::get(int64Type, 2);
  Value* index[2];
  index[0] = zero;
  index[1] = one;
  Type* int8ArrayType = llvm::ArrayType::get(int8Type, 0)->getPointerTo();
  Type* int64ArrayType = llvm::ArrayType::get(int64Type, 0)->getPointerTo();
  Value* int64ArrayPointer = IRWriter::newBitCastInst(context, arrayStructPointer, int64ArrayType);
  Value* sizeStore = IRWriter::createGetElementPtrInst(context, int64ArrayPointer, index, 0);
  Value* sizeValue = get<0>(arrayData.front());
  IRWriter::newStoreInst(context, sizeValue, sizeStore);
  
  Value* elementSize = get<1>(arrayData.front());
  index[1] = two;
  Value* elementSizeStore = IRWriter::createGetElementPtrInst(context, int64ArrayPointer, index, 0);
  IRWriter::newStoreInst(context, elementSize, elementSizeStore);

  if (arrayData.size() == 1) {
    arrayData.pop_front();
    return;
  }
  
  Function* function = context.getBasicBlock()->getParent();
  BasicBlock* forCond = BasicBlock::Create(context.getLLVMContext(), "for.cond", function);
  BasicBlock* forBody = BasicBlock::Create(context.getLLVMContext(), "for.body", function);
  BasicBlock* forEnd = BasicBlock::Create(context.getLLVMContext(), "for.end", function);

  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), ArrayType::ARRAY_ELEMENTS_START_INDEX);
  Value* arrayPointer = IRWriter::createGetElementPtrInst(context, int64ArrayPointer, index, 0);
  Value* int8ArrayPointer = IRWriter::newBitCastInst(context, arrayPointer, int8ArrayType);
  Value* indexStore = IRWriter::newAllocaInst(context, int64Type, "index");
  IRWriter::newStoreInst(context, zero, indexStore);
  IRWriter::createBranch(context, forCond, 0);
  
  context.setBasicBlock(forCond);
  
  Value* indexValue = IRWriter::newLoadInst(context, indexStore, "index");
  Value* compare = IRWriter::newICmpInst(context, ICmpInst::ICMP_SLT, indexValue, sizeValue, "cmp");
  IRWriter::createConditionalBranch(context, forBody, forEnd, compare, 0);
  
  context.setBasicBlock(forBody);
  
  Value* offset = IRWriter::createBinaryOperator(context,
                                                 Instruction::Mul,
                                                 indexValue,
                                                 elementSize,
                                                 "",
                                                 0);
  index[1] = offset;
  Value* arrayElement = IRWriter::createGetElementPtrInst(context, int8ArrayPointer, index, 0);
  arrayData.pop_front();
  initializeEmptyArray(context, arrayElement, arrayData);
  Value* newIndex = IRWriter::createBinaryOperator(context,
                                                   Instruction::Add,
                                                   indexValue,
                                                   one,
                                                   "indexIncrement",
                                                   0);
  IRWriter::newStoreInst(context, newIndex, indexStore);
  IRWriter::createBranch(context, forCond, 0);
  
  context.setBasicBlock(forEnd);
}

bool ArrayAllocation::isConstant() const {
  return false;
}

bool ArrayAllocation::isAssignable() const {
  return false;
}

const IType* ArrayAllocation::getType(IRGenerationContext& context) const {
  const ArraySpecificType* arraySpecificType = mArraySpecificTypeSpecifier->getType(context);
  const ArrayOwnerType* arrayOwnerType =
  arraySpecificType->getArrayType(context, mLine)->getOwner();
  delete arraySpecificType;
  return arrayOwnerType;
}

void ArrayAllocation::printToStream(IRGenerationContext &context, iostream &stream) const {
  stream << "new ";
  mArraySpecificTypeSpecifier->printToStream(context, stream);
}
