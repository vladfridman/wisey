//
//  AdjustReferenceCountFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCountFunction.hpp"
#include "wisey/Environment.hpp"
#include "wisey/GetOriginalObjectFunction.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IsModelFunction.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* AdjustReferenceCountFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);

  return function;
}

void AdjustReferenceCountFunction::call(IRGenerationContext& context,
                                        Value* object,
                                        int adjustment) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Value* bitcast = object->getType() != int8PointerType
  ? IRWriter::newBitCastInst(context, object, int8PointerType)
  : object;
  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(bitcast);
  llvm::Constant* adjustmentValue = llvm::ConstantInt::get(Type::getInt64Ty(llvmContext),
                                                           adjustment);
  arguments.push_back(adjustmentValue);
  
  IRWriter::createCallInst(context, function, arguments, "");
}

string AdjustReferenceCountFunction::getName() {
  return "__adjustReferenceCounter";
}

Function* AdjustReferenceCountFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* AdjustReferenceCountFunction::getLLVMFunctionType(IRGenerationContext& context) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType(context, 0));
  argumentTypes.push_back(LLVMPrimitiveTypes::I64);
  
  return context.getLLVMFunctionType(LLVMPrimitiveTypes::VOID, argumentTypes);
}

void AdjustReferenceCountFunction::compose(IRGenerationContext& context, llvm::Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument* llvmArgument = &*llvmArguments;
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
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition, 0);
  
  context.setBasicBlock(ifNullBlock);
  IRWriter::createReturnInst(context, NULL, 0);
  
  context.setBasicBlock(ifNotNullBlock);
  Value* isModel = IsModelFunction::call(context, object);
  Value* original = GetOriginalObjectFunction::call(context, object);
  Type* int64PointerType = Type::getInt64Ty(llvmContext)->getPointerTo();
  Value* objectStart = IRWriter::newBitCastInst(context, original, int64PointerType);
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), -1);
  Value* counter = IRWriter::createGetElementPtrInst(context, objectStart, index);

  BasicBlock* ifModelBlock = BasicBlock::Create(llvmContext, "if.model", function);
  BasicBlock* ifNotModelBlock = BasicBlock::Create(llvmContext, "if.not.model", function);

  IRWriter::createConditionalBranch(context, ifModelBlock, ifNotModelBlock, isModel, 0);

  context.setBasicBlock(ifModelBlock);
  new AtomicRMWInst(AtomicRMWInst::BinOp::Add,
                    counter,
                    adjustment,
                    AtomicOrdering::Monotonic,
                    SynchronizationScope::CrossThread,
                    ifModelBlock);
  IRWriter::createReturnInst(context, NULL, 0);

  context.setBasicBlock(ifNotModelBlock);
  Value* count = IRWriter::newLoadInst(context, counter, "count");
  Value* sum = IRWriter::createBinaryOperator(context, Instruction::Add, count, adjustment, "", 0);
  IRWriter::newStoreInst(context, sum, counter);
  IRWriter::createReturnInst(context, NULL, 0);

  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}
