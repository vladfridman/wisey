//
//  TestIRWriter.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IRWriter}

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>

#include "yazyk/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct IRWriterTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Function* mMainFunction;
  BasicBlock* mBasicBlock;
  
  IRWriterTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
    mMainFunction = Function::Create(functionType,
                                     GlobalValue::InternalLinkage,
                                     "main",
                                     mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mMainFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mContext.setMainFunction(mMainFunction);
  }
  
  ~IRWriterTest() {}
};

TEST_F(IRWriterTest, createReturnInstTest) {
  Value* value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  ReturnInst* returnInst = IRWriter::createReturnInst(mContext, value);
  
  EXPECT_EQ(mBasicBlock->size(), 1u);

  value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 2);
  IRWriter::createReturnInst(mContext, value);

  EXPECT_EQ(mBasicBlock->size(), 1u);
  EXPECT_EQ(mBasicBlock->getTerminator(), returnInst);
}

TEST_F(IRWriterTest, createBranchTest) {
  BasicBlock* block1 = BasicBlock::Create(mLLVMContext, "block1");
  BasicBlock* block2 = BasicBlock::Create(mLLVMContext, "block2");
  
  EXPECT_NE(IRWriter::createBranch(mContext, block1), nullptr);
  EXPECT_EQ(IRWriter::createBranch(mContext, block2), nullptr);
}

TEST_F(IRWriterTest, createConditionalBranchTest) {
  BasicBlock* block1 = BasicBlock::Create(mLLVMContext, "block1");
  BasicBlock* block2 = BasicBlock::Create(mLLVMContext, "block2");
  Value* conditionValue = ConstantInt::get(Type::getInt1Ty(mLLVMContext), 1);
  
  BranchInst* branch1 = IRWriter::createConditionalBranch(mContext,
                                                          block1,
                                                          block2,
                                                          conditionValue);
  BranchInst* branch2 = IRWriter::createConditionalBranch(mContext,
                                                          block1,
                                                          block2,
                                                          conditionValue);
  EXPECT_NE(branch1, nullptr);
  EXPECT_EQ(branch2, nullptr);
}

TEST_F(IRWriterTest, createBinaryOperator) {
  Value* value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  IRWriter::createBinaryOperator(mContext, Instruction::Add, value, value, "");

  EXPECT_EQ(mBasicBlock->size(), 1u);

  IRWriter::createReturnInst(mContext, value);

  EXPECT_EQ(mBasicBlock->size(), 2u);

  IRWriter::createBinaryOperator(mContext, Instruction::Add, value, value, "");

  EXPECT_EQ(mBasicBlock->size(), 2u);
}

TEST_F(IRWriterTest, createCallInstTest) {
  vector<Value*> arguments;
  IRWriter::createCallInst(mContext, mMainFunction, arguments, "");

  EXPECT_EQ(mBasicBlock->size(), 1u);

  Value* value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  IRWriter::createReturnInst(mContext, value);
  
  EXPECT_EQ(mBasicBlock->size(), 2u);

  IRWriter::createCallInst(mContext, mMainFunction, arguments, "");

  EXPECT_EQ(mBasicBlock->size(), 2u);
}
