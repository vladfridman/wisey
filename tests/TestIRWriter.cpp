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
  BasicBlock* mBasicBlock;
  
  IRWriterTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mContext.setMainFunction(function);
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
