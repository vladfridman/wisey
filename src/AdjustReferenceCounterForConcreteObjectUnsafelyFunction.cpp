//
//  AdjustReferenceCounterForConcreteObjectUnsafelyFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCounterForConcreteObjectUnsafelyFunction.hpp"
#include "wisey/Environment.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

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
  Type* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();

  Function* function = get(context);
  vector<Value*> arguments;
  if (object->getType() == int8Pointer) {
    arguments.push_back(object);
  } else {
    arguments.push_back(IRWriter::newBitCastInst(context, object, int8Pointer));
  }
  llvm::Constant* value = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), adjustment);
  arguments.push_back(value);
  
  IRWriter::createCallInst(context, function, arguments, "");
}

string AdjustReferenceCounterForConcreteObjectUnsafelyFunction::getName() {
  return "__adjustReferenceCounterForConcreteObjectUnsafely";
}

Function* AdjustReferenceCounterForConcreteObjectUnsafelyFunction::define(IRGenerationContext&
                                                                          context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* AdjustReferenceCounterForConcreteObjectUnsafelyFunction::
getLLVMFunctionType(IRGenerationContext& context) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType());
  argumentTypes.push_back(LLVMPrimitiveTypes::I64);
  
  return context.getLLVMFunctionType(LLVMPrimitiveTypes::VOID, argumentTypes);
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
  Type* int64Pointer = Type::getInt64Ty(llvmContext)->getPointerTo();
  Value* objectStart = IRWriter::newBitCastInst(context, object, int64Pointer);
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), -1);
  Value* counter = IRWriter::createGetElementPtrInst(context, objectStart, index);
  Value* count = IRWriter::newLoadInst(context, counter, "count");
  Value* sum = IRWriter::createBinaryOperator(context, Instruction::Add, count, adjustment, "");
  IRWriter::newStoreInst(context, sum, counter);
  IRWriter::createReturnInst(context, NULL);

  context.registerLLVMFunctionNamedType(getName(), getLLVMFunctionType(context));
}

