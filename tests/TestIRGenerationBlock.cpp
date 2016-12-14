//
//  TestIRGenerationBlock.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IRGenerationBlock}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "yazyk/IRGenerationBlock.hpp"
#include "yazyk/IRGenerationContext.hpp"

using namespace llvm;
using namespace yazyk;

using ::testing::Test;

struct IRGenerationBlockTest : public Test {
  IRGenerationContext mContext;
  BasicBlock* mBlock = BasicBlock::Create(mContext.getLLVMContext(), "entry");
  IRGenerationBlock mIRBlock;
 
public:
  
  IRGenerationBlockTest() : mIRBlock(IRGenerationBlock(mBlock)) { }
  
  ~IRGenerationBlockTest() {
    delete mBlock;
  }
};

TEST_F(IRGenerationBlockTest, CreationTest) {
  EXPECT_EQ(mIRBlock.getBlock(), mBlock);
}

TEST_F(IRGenerationBlockTest, SetBlockTest) {
  BasicBlock* anotherBlock = BasicBlock::Create(mContext.getLLVMContext(), "another");
  mIRBlock.setBlock(anotherBlock);
  
  EXPECT_EQ(mIRBlock.getBlock(), anotherBlock);
}

TEST_F(IRGenerationBlockTest, LocalsTest) {
  LLVMContext& llvmContext = mContext.getLLVMContext();
  BasicBlock* anotherBlock = BasicBlock::Create(llvmContext, "another");
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);
  
  mIRBlock.getLocals()["foo"] = fooValue;
  mIRBlock.setBlock(anotherBlock);
  
  EXPECT_EQ(mIRBlock.getLocals()["foo"], fooValue);
  EXPECT_EQ(mIRBlock.getLocals()["bar"] == NULL, true);
}
