//
//  TestSafeBranch.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/29/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link SafeBranch}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/SafeBranch.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace yazyk;

struct SafeBranchTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext = mContext.getLLVMContext();
  BasicBlock* mCurrentBlock = BasicBlock::Create(mLLVMContext, "current");
  BasicBlock* mBlock1 = BasicBlock::Create(mLLVMContext, "block1");
  BasicBlock* mBlock2 = BasicBlock::Create(mLLVMContext, "block2");
  Value* mConditionValue = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 1);

public:
  
  SafeBranchTest() {
    mContext.setBasicBlock(mCurrentBlock);
  }
  
  ~SafeBranchTest() {
    delete mCurrentBlock;
    delete mBlock1;
    delete mBlock2;
  }
};

TEST_F(SafeBranchTest, consecutiveBranchesNotCreated) {
  EXPECT_NE(SafeBranch::newBranch(mBlock1, mContext), nullptr);
  EXPECT_EQ(SafeBranch::newBranch(mBlock2, mContext), nullptr);
}

TEST_F(SafeBranchTest, consecutiveConditionalBranchesNotCreated) {
  BranchInst* branch1 = SafeBranch::newConditionalBranch(mBlock1,
                                                         mBlock2,
                                                         mConditionValue,
                                                         mContext);
  BranchInst* branch2 = SafeBranch::newConditionalBranch(mBlock1,
                                                         mBlock2,
                                                         mConditionValue,
                                                         mContext);
  EXPECT_NE(branch1, nullptr);
  EXPECT_EQ(branch2, nullptr);
}
