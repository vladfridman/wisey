//
//  AdjustReferenceCounterForImmutableArrayFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCounterForImmutableArrayFunction.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* AdjustReferenceCounterForImmutableArrayFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

void AdjustReferenceCounterForImmutableArrayFunction::call(IRGenerationContext& context,
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

string AdjustReferenceCounterForImmutableArrayFunction::getName() {
  return "__adjustReferenceCounterForImmutableArray";
}

Function* AdjustReferenceCounterForImmutableArrayFunction::define(IRGenerationContext&
                                                                  context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* AdjustReferenceCounterForImmutableArrayFunction::
getLLVMFunctionType(IRGenerationContext& context) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType(context, 0));
  argumentTypes.push_back(LLVMPrimitiveTypes::I64);
  
  return context.getLLVMFunctionType(LLVMPrimitiveTypes::VOID, argumentTypes);
}

void AdjustReferenceCounterForImmutableArrayFunction::compose(IRGenerationContext& context,
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
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition, 0);
  
  context.setBasicBlock(ifNullBlock);
  IRWriter::createReturnInst(context, NULL, 0);
  
  context.setBasicBlock(ifNotNullBlock);
  Type* int64Pointer = Type::getInt64Ty(llvmContext)->getPointerTo();
  Value* counter = IRWriter::newBitCastInst(context, array, int64Pointer);
  new AtomicRMWInst(AtomicRMWInst::BinOp::Add,
                    counter,
                    adjustment,
                    AtomicOrdering::Monotonic,
                    SynchronizationScope::CrossThread,
                    ifNotNullBlock);
  IRWriter::createReturnInst(context, NULL, 0);
  
  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}
