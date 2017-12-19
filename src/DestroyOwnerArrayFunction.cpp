//
//  DestroyOwnerArrayFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/DestroyOwnerArrayFunction.hpp"
#include "wisey/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* DestroyOwnerArrayFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

void DestroyOwnerArrayFunction::call(IRGenerationContext& context,
                                     Value* array,
                                     unsigned long size,
                                     Function* destructor) {
  Function* function = DestroyOwnerArrayFunction::get(context);
  vector<Value*> arguments;
  arguments.push_back(array);
  llvm::Constant* value = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), size);
  arguments.push_back(value);
  arguments.push_back(destructor);
  
  IRWriter::createCallInst(context, function, arguments, "");
}

string DestroyOwnerArrayFunction::getName() {
  return "__destroyOwnerArrayFunction";
}

Function* DestroyOwnerArrayFunction::define(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();

  Type* genericPointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  Type* genericArray = llvm::ArrayType::get(genericPointer, 0)->getPointerTo();

  vector<Type*> destructorArgumentTypes;
  destructorArgumentTypes.push_back(genericPointer);
  ArrayRef<Type*> destructorArgTypesArray = ArrayRef<Type*>(destructorArgumentTypes);
  Type* detructorLlvmReturnType = Type::getVoidTy(llvmContext);
  FunctionType* destructorFunctionType = FunctionType::get(detructorLlvmReturnType,
                                                           destructorArgTypesArray,
                                                           false);

  vector<Type*> argumentTypes;
  argumentTypes.push_back(genericArray);
  argumentTypes.push_back(Type::getInt64Ty(llvmContext));
  argumentTypes.push_back(destructorFunctionType->getPointerTo());
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  FunctionType* ftype = FunctionType::get(llvmReturnType, argTypesArray, false);
  
  return Function::Create(ftype, GlobalValue::InternalLinkage, getName(), context.getModule());
}

void DestroyOwnerArrayFunction::compose(IRGenerationContext& context, Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();

  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument *llvmArgument = &*llvmArguments;
  llvmArgument->setName("arrayBitcast");
  Value* arrayBitcast = llvmArgument;
  llvmArguments++;
  llvmArgument = &*llvmArguments;
  llvmArgument->setName("size");
  Value* size = llvmArgument;
  llvmArguments++;
  llvmArgument = &*llvmArguments;
  llvmArgument->setName("destructor");
  Value* destructor = llvmArgument;

  llvm::BasicBlock* entry = llvm::BasicBlock::Create(llvmContext, "entry", function);
  llvm::BasicBlock* forCond = llvm::BasicBlock::Create(llvmContext, "for.cond", function);
  llvm::BasicBlock* forBody = llvm::BasicBlock::Create(llvmContext, "for.body", function);
  llvm::BasicBlock* forEnd = llvm::BasicBlock::Create(llvmContext, "for.end", function);
  
  context.setBasicBlock(entry);
  
  llvm::Type* int64type = llvm::Type::getInt64Ty(llvmContext);
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
  IRWriter::createConditionalBranch(context, forBody, forEnd, compare);
  
  context.setBasicBlock(forBody);
  llvm::Value* index[2];
  index[0] = llvm::ConstantInt::get(int64type, 0);
  index[1] = indexValue;
  llvm::Value* elementStore = IRWriter::createGetElementPtrInst(context, arrayBitcast, index);
  llvm::Value* elementPointer = IRWriter::newLoadInst(context, elementStore, "");
  vector<Value*> arguments;
  arguments.push_back(elementPointer);
  IRWriter::createCallInst(context, (Function*) destructor, arguments, "");

  arguments.push_back(elementPointer);
  llvm::Constant* one = llvm::ConstantInt::get(int64type, 1);
  llvm::Value* newIndexValue = IRWriter::createBinaryOperator(context,
                                                              llvm::Instruction::Add,
                                                              indexValue,
                                                              one,
                                                              "");
  IRWriter::newStoreInst(context, newIndexValue, indexStore);
  
  IRWriter::createBranch(context, forCond);
  
  context.setBasicBlock(forEnd);

  
  IRWriter::createReturnInst(context, NULL);
}


