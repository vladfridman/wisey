//
//  IRWriter.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

ReturnInst* IRWriter::createReturnInst(IRGenerationContext& context, Value* returnValue) {
  if (context.getBasicBlock()->getTerminator()) {
    return NULL;
  }
  return ReturnInst::Create(context.getLLVMContext(), returnValue, context.getBasicBlock());
}

BranchInst* IRWriter::createBranch(IRGenerationContext& context, BasicBlock* toBlock) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return BranchInst::Create(toBlock, currentBlock);
}

BranchInst* IRWriter::createConditionalBranch(IRGenerationContext& context,
                                              BasicBlock* ifTrueBlock,
                                              BasicBlock* ifFalseBlock,
                                              Value* condition) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return BranchInst::Create(ifTrueBlock, ifFalseBlock, condition, currentBlock);
}

BinaryOperator* IRWriter::createBinaryOperator(IRGenerationContext& context,
                                               Instruction::BinaryOps instruction,
                                               Value* leftValue,
                                               Value* rightValue,
                                               string llvmVariableName) {
  BasicBlock* currentBlock = context.getBasicBlock();

  if(currentBlock->getTerminator()) {
    return NULL;
  }

  return BinaryOperator::Create(instruction, leftValue, rightValue, llvmVariableName, currentBlock);
}

CallInst* IRWriter::createCallInst(IRGenerationContext& context,
                                   Function* function,
                                   vector<Value*> arguments,
                                   string resultName) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return CallInst::Create(function, arguments, resultName, currentBlock);
}

Instruction* IRWriter::createMalloc(IRGenerationContext& context,
                                    Type* structType,
                                    Value* allocSize,
                                    string variableName) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }

  Type* pointerType = Type::getInt32Ty(context.getLLVMContext());
  Instruction* malloc = CallInst::CreateMalloc(currentBlock,
                                               pointerType,
                                               structType,
                                               allocSize,
                                               nullptr,
                                               nullptr,
                                               variableName);
  currentBlock->getInstList().push_back(malloc);
  
  return malloc;
}

Instruction* IRWriter::createFree(IRGenerationContext& context, Value* value) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }

  Instruction* instruction = CallInst::CreateFree(value, currentBlock);
  currentBlock->getInstList().push_back(instruction);
  
  return instruction;
}

GetElementPtrInst* IRWriter::createGetElementPtrInst(IRGenerationContext& context,
                                                     Value* value,
                                                     ArrayRef<Value *> index) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }

  return GetElementPtrInst::Create(value->getType()->getPointerElementType(),
                                   value,
                                   index,
                                   "",
                                   currentBlock);
}

BitCastInst* IRWriter::newBitCastInst(IRGenerationContext& context, Value* value, Type* type) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new BitCastInst(value, type, "", currentBlock);
}

StoreInst* IRWriter::newStoreInst(IRGenerationContext& context, Value* value, Value* pointer) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new StoreInst(value, pointer, currentBlock);
}

AllocaInst* IRWriter::newAllocaInst(IRGenerationContext& context, Type* type, string variableName) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new AllocaInst(type, variableName, currentBlock);
}

LoadInst* IRWriter::newLoadInst(IRGenerationContext& context,
                                Value* pointer,
                                string variableName) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new LoadInst(pointer, variableName, currentBlock);
}

CastInst* IRWriter::createZExtOrBitCast(IRGenerationContext& context,
                                        Value* fromValue,
                                        Type* toLLVMType) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return CastInst::CreateZExtOrBitCast(fromValue, toLLVMType, "conv", currentBlock);
}

TruncInst* IRWriter::newTruncInst(IRGenerationContext& context,
                                  Value* fromValue,
                                  Type* toLLVMType) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new TruncInst(fromValue, toLLVMType, "conv", currentBlock);
}

FPTruncInst* IRWriter::newFPTruncInst(IRGenerationContext& context,
                                    Value* fromValue,
                                    Type* toLLVMType) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new FPTruncInst(fromValue, toLLVMType, "conv", currentBlock);
}
