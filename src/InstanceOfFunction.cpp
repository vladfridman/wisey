//
//  InstanceOfFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Environment.hpp"
#include "wisey/GetOriginalObjectFunction.hpp"
#include "wisey/InstanceOfFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* InstanceOfFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

Value* InstanceOfFunction::call(IRGenerationContext& context,
                                llvm::Value* haystack,
                                llvm::Value* needle,
                                int line) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Value* bitcast = haystack->getType() != int8PointerType
  ? IRWriter::newBitCastInst(context, haystack, int8PointerType) : haystack;
  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(bitcast);
  arguments.push_back(needle);
  
  return IRWriter::createCallInst(context, function, arguments, "", line);
}

string InstanceOfFunction::getName() {
  return "__instanceOf";
}

Function* InstanceOfFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* InstanceOfFunction::getLLVMFunctionType(IRGenerationContext& context) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType(context, 0));
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType(context, 0));
  
  return context.getLLVMFunctionType(LLVMPrimitiveTypes::I32, argumentTypes);
}

void InstanceOfFunction::compose(IRGenerationContext& context, Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function, 0);
  BasicBlock* whileCond = BasicBlock::Create(llvmContext, "while.cond", function);
  BasicBlock* whileBody = BasicBlock::Create(llvmContext, "while.body", function);
  BasicBlock* returnNotFound = BasicBlock::Create(llvmContext, "return.notfound", function);
  BasicBlock* returnFound = BasicBlock::Create(llvmContext, "return.found", function);
  
  context.setBasicBlock(entryBlock);
  Value* iterator = IRWriter::newAllocaInst(context, Type::getInt32Ty(llvmContext), "iterator");
  ConstantInt* one = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  IRWriter::newStoreInst(context, one, iterator);
  
  Value* arrayOfStrings = composeEntryBlock(context, entryBlock, whileCond, function);
  
  Value* stringPointer = composeWhileConditionBlock(context,
                                                    whileCond,
                                                    whileBody,
                                                    returnNotFound,
                                                    iterator,
                                                    arrayOfStrings);
  composeWhileBodyBlock(context,
                        whileBody,
                        whileCond,
                        returnFound,
                        iterator,
                        stringPointer,
                        function);
  composeReturnFound(context, returnFound, iterator);
  composeReturnNotFound(context, returnNotFound);
  
  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}

BitCastInst* InstanceOfFunction::composeEntryBlock(IRGenerationContext& context,
                                                   BasicBlock* entryBlock,
                                                   BasicBlock* whileCond,
                                                   Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);
  
  context.setBasicBlock(entryBlock);
  
  Function::arg_iterator functionArguments = function->arg_begin();
  Value* haystackArgument = &*functionArguments;
  haystackArgument->setName("haystack");
  Value* originalObjectVTable = GetOriginalObjectFunction::call(context, haystackArgument, 0);
  
  Type* pointerToArrayOfStrings = int8Type->getPointerTo()->getPointerTo()->getPointerTo();
  
  BitCastInst* vTablePointer =
  IRWriter::newBitCastInst(context, originalObjectVTable, pointerToArrayOfStrings);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 1);
  GetElementPtrInst* typeArrayPointerI8 = IRWriter::createGetElementPtrInst(context, vTable, index);
  LoadInst* typeArrayI8 = IRWriter::newLoadInst(context, typeArrayPointerI8, "typeArrayI8");
  BitCastInst* arrayOfStrings =
  IRWriter::newBitCastInst(context, typeArrayI8, int8Type->getPointerTo()->getPointerTo());
  
  IRWriter::createBranch(context, whileCond, 0);
  
  return arrayOfStrings;
}

LoadInst* InstanceOfFunction::composeWhileConditionBlock(IRGenerationContext& context,
                                                         BasicBlock* whileCond,
                                                         BasicBlock* whileBody,
                                                         BasicBlock* returnFalse,
                                                         Value* iterator,
                                                         Value* arrayOfStrings) {
  Type* int8Type = Type::getInt8Ty(context.getLLVMContext());
  
  context.setBasicBlock(whileCond);
  
  LoadInst* iteratorLoaded = IRWriter::newLoadInst(context, iterator, "");
  Value* index[1];
  index[0] = iteratorLoaded;
  Value* stringPointerPointer = IRWriter::createGetElementPtrInst(context, arrayOfStrings, index);
  
  LoadInst* stringPointer = IRWriter::newLoadInst(context, stringPointerPointer, "stringPointer");
  
  Value* nullPointerValue = ConstantPointerNull::get(int8Type->getPointerTo());
  Value* checkStringIsNull =
  IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, stringPointer, nullPointerValue, "cmpnull");
  
  IRWriter::createConditionalBranch(context, returnFalse, whileBody, checkStringIsNull, 0);
  
  return stringPointer;
}

void InstanceOfFunction::composeWhileBodyBlock(IRGenerationContext& context,
                                               BasicBlock* whileBody,
                                               BasicBlock* whileCond,
                                               BasicBlock* returnTrue,
                                               Value* iterator,
                                               Value* stringPointer,
                                               Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  context.setBasicBlock(whileBody);
  
  LoadInst* iteratorLoaded = IRWriter::newLoadInst(context, iterator, "");
  ConstantInt* one =  ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Value* increment = IRWriter::createBinaryOperator(context,
                                                    Instruction::Add,
                                                    iteratorLoaded,
                                                    one,
                                                    "inc",
                                                    0);
  IRWriter::newStoreInst(context, increment, iterator);
  
  Function::arg_iterator functionArguments = function->arg_begin();
  functionArguments++;
  Value* needleArgument = &*functionArguments;
  needleArgument->setName("needle");
  
  Value* doesTypeMatch =
  IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, stringPointer, needleArgument, "cmp");
  IRWriter::createConditionalBranch(context, returnTrue, whileCond, doesTypeMatch, 0);
}

void InstanceOfFunction::composeReturnFound(IRGenerationContext& context,
                                            BasicBlock* returnFound,
                                            Value* iterator) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  context.setBasicBlock(returnFound);
  LoadInst* iteratorLoaded = IRWriter::newLoadInst(context, iterator, "");
  ConstantInt* two = ConstantInt::get(Type::getInt32Ty(llvmContext), 2);
  
  Value* decrement =
  IRWriter::createBinaryOperator(context, Instruction::Sub, iteratorLoaded, two, "dec", 0);
  IRWriter::createReturnInst(context, decrement, 0);
}

void InstanceOfFunction::composeReturnNotFound(IRGenerationContext& context,
                                               BasicBlock* returnNotFound) {
  LLVMContext& llvmContext = context.getLLVMContext();
  ConstantInt* negativeOne = ConstantInt::get(Type::getInt32Ty(llvmContext), -1);
  
  context.setBasicBlock(returnNotFound);
  IRWriter::createReturnInst(context, negativeOne, 0);
}
