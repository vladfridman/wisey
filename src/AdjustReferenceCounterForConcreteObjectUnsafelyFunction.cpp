//
//  AdjustReferenceCounterForConcreteObjectUnsafelyFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCounterForConcreteObjectUnsafelyFunction.hpp"
#include "wisey/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* AdjustReferenceCounterForConcreteObjectUnsafelyFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

void AdjustReferenceCounterForConcreteObjectUnsafelyFunction::call(IRGenerationContext& context,
                                                                   Value* object,
                                                                   int adjustment) {
  Value* counterPointer = IObjectType::getReferenceCounterPointer(context, object);
  
  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(counterPointer);
  llvm::Constant* value = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), adjustment);
  arguments.push_back(value);
  
  IRWriter::createCallInst(context, function, arguments, "");
}

string AdjustReferenceCounterForConcreteObjectUnsafelyFunction::getName() {
  return "__adjustReferenceCounterForConcreteObjectUnsafely";
}

Function* AdjustReferenceCounterForConcreteObjectUnsafelyFunction::define(IRGenerationContext&
                                                                          context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Type*> argumentTypes;
  argumentTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  argumentTypes.push_back(Type::getInt64Ty(llvmContext));
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  FunctionType* ftype = FunctionType::get(llvmReturnType, argumentTypes, false);
  
  return Function::Create(ftype, GlobalValue::InternalLinkage, getName(), context.getModule());
}

void AdjustReferenceCounterForConcreteObjectUnsafelyFunction::compose(IRGenerationContext& context,
                                                                      Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument *llvmArgument = &*llvmArguments;
  llvmArgument->setName("object");
  Value* object = llvmArgument;
  llvmArguments++;
  llvmArgument = &*llvmArguments;
  llvmArgument->setName("adjustment");
  Value* adjustment = llvmArgument;
  
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* ifNullBlock = BasicBlock::Create(llvmContext, "if.null", function);
  BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.notnull", function);
  
  context.setBasicBlock(entryBlock);
  Value* null = ConstantPointerNull::get(Type::getInt8Ty(llvmContext)->getPointerTo());
  Value* condition = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, object, null, "");
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition);
  
  context.setBasicBlock(ifNullBlock);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifNotNullBlock);
  Value* counter = IRWriter::newBitCastInst(context,
                                            object,
                                            Type::getInt64Ty(llvmContext)->getPointerTo());
  Value* count = IRWriter::newLoadInst(context, counter, "count");
  Value* sum = IRWriter::createBinaryOperator(context, Instruction::Add, count, adjustment, "");
  IRWriter::newStoreInst(context, sum, counter);
  IRWriter::createReturnInst(context, NULL);
}
