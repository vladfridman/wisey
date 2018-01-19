//
//  DestroyReferenceArrayFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCounterForConcreteObjectSafelyFunction.hpp"
#include "wisey/DestroyReferenceArrayFunction.hpp"
#include "wisey/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* DestroyReferenceArrayFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

void DestroyReferenceArrayFunction::call(IRGenerationContext& context,
                                         Value* array,
                                         unsigned long size) {
  Function* function = DestroyReferenceArrayFunction::get(context);
  vector<Value*> arguments;
  arguments.push_back(array);
  llvm::Constant* value = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), size);
  arguments.push_back(value);
  
  IRWriter::createCallInst(context, function, arguments, "");
}

string DestroyReferenceArrayFunction::getName() {
  return "__destroyReferenceArrayFunction";
}

Function* DestroyReferenceArrayFunction::define(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  PointerType* genericPointer = llvm::Type::getInt64Ty(context.getLLVMContext())->getPointerTo();

  vector<Type*> argumentTypes;
  argumentTypes.push_back(genericPointer);
  argumentTypes.push_back(Type::getInt64Ty(llvmContext));
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  FunctionType* ftype = FunctionType::get(llvmReturnType, argTypesArray, false);
  
  return Function::Create(ftype, GlobalValue::InternalLinkage, getName(), context.getModule());
}

void DestroyReferenceArrayFunction::compose(IRGenerationContext& context, Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  PointerType* genericPointer = llvm::Type::getInt64Ty(context.getLLVMContext())->getPointerTo();

  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument *llvmArgument = &*llvmArguments;
  llvmArgument->setName("arrayPointer");
  Value* arrayPointer = llvmArgument;
  llvmArguments++;
  llvmArgument = &*llvmArguments;
  llvmArgument->setName("size");
  Value* size = llvmArgument;
  llvmArguments++;
  
  llvm::BasicBlock* entry = llvm::BasicBlock::Create(llvmContext, "entry", function);
  llvm::BasicBlock* ifNull = llvm::BasicBlock::Create(llvmContext, "if.null", function);
  llvm::BasicBlock* ifNotNull = llvm::BasicBlock::Create(llvmContext, "if.not.null", function);
  llvm::BasicBlock* forCond = llvm::BasicBlock::Create(llvmContext, "for.cond", function);
  llvm::BasicBlock* forBody = llvm::BasicBlock::Create(llvmContext, "for.body", function);
  llvm::BasicBlock* freeArray = llvm::BasicBlock::Create(llvmContext, "free.array", function);
  
  context.setBasicBlock(entry);
  
  Value* null = ConstantPointerNull::get(genericPointer);
  Value* isNull = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, arrayPointer, null, "");
  IRWriter::createConditionalBranch(context, ifNull, ifNotNull, isNull);
  
  context.setBasicBlock(ifNull);
  
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifNotNull);
  
  llvm::Type* int64type = llvm::Type::getInt64Ty(llvmContext);
  llvm::Constant* one = ConstantInt::get(int64type, 1);
  llvm::Constant* two = ConstantInt::get(int64type, 2);
  Value* index[1];
  index[0] = ConstantInt::get(int64type, 1);
  Value* dimensionsStore = IRWriter::createGetElementPtrInst(context, arrayPointer, index);
  Value* dimensions = IRWriter::newLoadInst(context, dimensionsStore, "dimensions");
  Value* offset = IRWriter::createBinaryOperator(context,
                                                 llvm::Instruction::Add,
                                                 dimensions,
                                                 two,
                                                 "offset");

  index[0] = dimensions;
  Value* arrayStore = IRWriter::createGetElementPtrInst(context, dimensionsStore, offset);
  Value* array = IRWriter::newBitCastInst(context,
                                          arrayStore,
                                          ArrayType::getGenericArrayType(context));

  llvm::Value* indexStore = IRWriter::newAllocaInst(context, int64type, "");
  llvm::Constant* int64zero = llvm::ConstantInt::get(int64type, 0);
  IRWriter::newStoreInst(context, int64zero, indexStore);
  IRWriter::createBranch(context, forCond);
  
  context.setBasicBlock(forCond);
  
  llvm::Value* indexValue = IRWriter::newLoadInst(context, indexStore, "");
  llvm::Value* compare = IRWriter::newICmpInst(context,
                                               ICmpInst::ICMP_SLT,
                                               indexValue,
                                               size,
                                               "cmp");
  IRWriter::createConditionalBranch(context, forBody, freeArray, compare);
  
  context.setBasicBlock(forBody);

  Value* idx[2];
  idx[0] = llvm::ConstantInt::get(int64type, 0);
  idx[1] = indexValue;
  llvm::Value* elementStore = IRWriter::createGetElementPtrInst(context, array, idx);
  llvm::Value* elementPointer = IRWriter::newLoadInst(context, elementStore, "");
  AdjustReferenceCounterForConcreteObjectSafelyFunction::call(context, elementPointer, -1);
  
  llvm::Value* newIndexValue = IRWriter::createBinaryOperator(context,
                                                              llvm::Instruction::Add,
                                                              indexValue,
                                                              one,
                                                              "");
  IRWriter::newStoreInst(context, newIndexValue, indexStore);
  
  IRWriter::createBranch(context, forCond);
  
  context.setBasicBlock(freeArray);
  
  IRWriter::createFree(context, arrayPointer);
  IRWriter::createReturnInst(context, NULL);
}
