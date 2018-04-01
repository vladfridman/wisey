//
//  AdjustReferenceCounterForArrayFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/31/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCounterForArrayFunction.hpp"
#include "wisey/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* AdjustReferenceCounterForArrayFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

void AdjustReferenceCounterForArrayFunction::call(IRGenerationContext& context,
                                                  Value* array,
                                                  int adjustment) {
  Type* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();

  Function* function = get(context);
  vector<Value*> arguments;
  if (array->getType() == int8Pointer) {
    arguments.push_back(array);
  } else {
    arguments.push_back(IRWriter::newBitCastInst(context, array, int8Pointer));
  }
  llvm::Constant* value = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), adjustment);
  arguments.push_back(value);
  
  IRWriter::createCallInst(context, function, arguments, "");
}

string AdjustReferenceCounterForArrayFunction::getName() {
  return "__adjustReferenceCounterForArrays";
}

Function* AdjustReferenceCounterForArrayFunction::define(IRGenerationContext&
                                                                        context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Type*> argumentTypes;
  argumentTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  argumentTypes.push_back(Type::getInt64Ty(llvmContext));
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  FunctionType* ftype = FunctionType::get(llvmReturnType, argumentTypes, false);
  
  return Function::Create(ftype, GlobalValue::InternalLinkage, getName(), context.getModule());
}

void AdjustReferenceCounterForArrayFunction::compose(IRGenerationContext& context,
                                                                    Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument *llvmArgument = &*llvmArguments;
  llvmArgument->setName("array");
  Value* array = llvmArgument;
  llvmArguments++;
  llvmArgument = &*llvmArguments;
  llvmArgument->setName("adjustment");
  Value* adjustment = llvmArgument;
  
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* ifNullBlock = BasicBlock::Create(llvmContext, "if.null", function);
  BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.notnull", function);
  
  context.setBasicBlock(entryBlock);
  Value* null = ConstantPointerNull::get(Type::getInt8Ty(llvmContext)->getPointerTo());
  Value* condition = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, array, null, "");
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition);
  
  context.setBasicBlock(ifNullBlock);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifNotNullBlock);
  Type* int64Pointer = Type::getInt64Ty(llvmContext)->getPointerTo();
  Value* counter = IRWriter::newBitCastInst(context, array, int64Pointer);
  new AtomicRMWInst(AtomicRMWInst::BinOp::Add,
                    counter,
                    adjustment,
                    AtomicOrdering::Monotonic,
                    SynchronizationScope::CrossThread,
                    ifNotNullBlock);
  IRWriter::createReturnInst(context, NULL);
}
