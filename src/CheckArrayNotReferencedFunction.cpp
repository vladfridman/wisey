//
//  CheckArrayNotReferencedFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/CheckArrayNotReferencedFunction.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/ThrowReferenceCountExceptionFunction.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* CheckArrayNotReferencedFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

void CheckArrayNotReferencedFunction::call(IRGenerationContext& context,
                                   Value* array,
                                   Value* numberOfDimensions) {
  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(array);
  arguments.push_back(numberOfDimensions);
  
  IRWriter::createCallInst(context, function, arguments, "");
}

string CheckArrayNotReferencedFunction::getName() {
  return "__checkArrayNotReferenced";
}

Function* CheckArrayNotReferencedFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* CheckArrayNotReferencedFunction::getLLVMFunctionType(IRGenerationContext& context) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I64->getPointerType());
  argumentTypes.push_back(LLVMPrimitiveTypes::I64);
  
  return context.getLLVMFunctionType(LLVMPrimitiveTypes::VOID, argumentTypes);
}

void CheckArrayNotReferencedFunction::compose(IRGenerationContext& context, Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int64type = Type::getInt64Ty(llvmContext);
  llvm::Constant* zero = ConstantInt::get(int64type, 0);
  llvm::Constant* one = ConstantInt::get(int64type, 1);
  llvm::Constant* two = ConstantInt::get(int64type, 2);
  llvm::PointerType* genericPointer = int64type->getPointerTo();
  llvm::PointerType* bytePointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  
  context.getScopes().pushScope();
  
  Function::arg_iterator llvmArguments = function->arg_begin();
  Value *llvmArgument = &*llvmArguments;
  llvmArgument->setName("arrayPointer");
  Value* arrayPointer = llvmArgument;
  llvmArguments++;
  llvmArgument = &*llvmArguments;
  llvmArgument->setName("noOfDimensions");
  Value* numberOfDimensions = llvmArgument;
  
  BasicBlock* entry = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* returnVoid = BasicBlock::Create(llvmContext, "return.void", function);
  BasicBlock* ifNotNull = BasicBlock::Create(llvmContext, "if.not.null", function);
  BasicBlock* refCountZeroBlock = BasicBlock::Create(llvmContext, "ref.count.zero", function);
  BasicBlock* refCountNotZeroBlock = BasicBlock::Create(llvmContext, "ref.count.notzero", function);
  BasicBlock* forCond = BasicBlock::Create(llvmContext, "for.cond", function);
  BasicBlock* forBody = BasicBlock::Create(llvmContext, "for.body", function);
  BasicBlock* multiDimensional = BasicBlock::Create(llvmContext, "multi.dimensional", function);
  
  context.setBasicBlock(entry);
  Value* null = ConstantPointerNull::get(genericPointer);
  Value* isNull = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, arrayPointer, null, "isNull");
  IRWriter::createConditionalBranch(context, returnVoid, ifNotNull, isNull);
  
  context.setBasicBlock(returnVoid);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifNotNull);
  Value* referenceCount = IRWriter::newLoadInst(context, arrayPointer, "refCount");
  Value* isZero = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, referenceCount, zero, "isZero");
  IRWriter::createConditionalBranch(context, refCountZeroBlock, refCountNotZeroBlock, isZero);
  
  context.setBasicBlock(refCountNotZeroBlock);
  ThrowReferenceCountExceptionFunction::call(context, referenceCount);
  IRWriter::newUnreachableInst(context);
  
  context.setBasicBlock(refCountZeroBlock);
  Value* isMultiDimensional = IRWriter::newICmpInst(context,
                                                    ICmpInst::ICMP_SGT,
                                                    numberOfDimensions,
                                                    one,
                                                    "cmp");
  IRWriter::createConditionalBranch(context, multiDimensional, returnVoid, isMultiDimensional);

  context.setBasicBlock(multiDimensional);
  Value* index[1];
  index[0] = one;
  Value* sizeStore = IRWriter::createGetElementPtrInst(context, arrayPointer, index);
  Value* size = IRWriter::newLoadInst(context, sizeStore, "size");
  index[0] = two;
  Value* elementSizeStore = IRWriter::createGetElementPtrInst(context, arrayPointer, index);
  Value* elementSize = IRWriter::newLoadInst(context, elementSizeStore, "elementSize");
  Value* numberOfDimensionsMinusOne = IRWriter::createBinaryOperator(context,
                                                                     llvm::Instruction::Sub,
                                                                     numberOfDimensions,
                                                                     one,
                                                                     "dimensionsMinusOne");
  index[0] = ConstantInt::get(int64type, ArrayType::ARRAY_ELEMENTS_START_INDEX);
  Value* arrayStore = IRWriter::createGetElementPtrInst(context, arrayPointer, index);
  Value* array = IRWriter::newBitCastInst(context, arrayStore, bytePointer);
  Value* indexStore = IRWriter::newAllocaInst(context, int64type, "indexStore");
  IRWriter::newStoreInst(context, zero, indexStore);
  Value* offsetStore = IRWriter::newAllocaInst(context, int64type, "offsetStore");
  IRWriter::newStoreInst(context, zero, offsetStore);
  IRWriter::createBranch(context, forCond);
  
  context.setBasicBlock(forCond);
  Value* offsetValue = IRWriter::newLoadInst(context, offsetStore, "offset");
  Value* indexValue = IRWriter::newLoadInst(context, indexStore, "index");
  Value* compare = IRWriter::newICmpInst(context, ICmpInst::ICMP_SLT, indexValue, size, "cmp");
  IRWriter::createConditionalBranch(context, forBody, returnVoid, compare);
  
  context.setBasicBlock(forBody);
  Value* idx[1];
  idx[0] = offsetValue;
  Value* elementStore = IRWriter::createGetElementPtrInst(context, array, idx);
  Value* newIndex = IRWriter::createBinaryOperator(context,
                                                   Instruction::Add,
                                                   indexValue,
                                                   one,
                                                   "newIndex");
  IRWriter::newStoreInst(context, newIndex, indexStore);
  Value* newOffset = IRWriter::createBinaryOperator(context,
                                                    Instruction::Add,
                                                    offsetValue,
                                                    elementSize,
                                                    "offsetIncrement");
  IRWriter::newStoreInst(context, newOffset, offsetStore);
  vector<Value*> recursiveCallArguments;
  recursiveCallArguments.push_back(IRWriter::newBitCastInst(context, elementStore, genericPointer));
  recursiveCallArguments.push_back(numberOfDimensionsMinusOne);
  IRWriter::createCallInst(context, function, recursiveCallArguments, "");
  IRWriter::createBranch(context, forCond);
  
  context.getScopes().popScope(context, 0);
  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context));
}

