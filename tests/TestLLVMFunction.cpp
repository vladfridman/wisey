//
//  TestLLVMFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMFunction}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct LLVMFunctionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  LLVMFunction* mLLVMFunction;
  
  LLVMFunctionTest() : mLLVMContext(mContext.getLLVMContext()) {
    LLVMFunctionArgument* llvmFunctionArgument =
    new LLVMFunctionArgument(LLVMPrimitiveTypes::I64, "input");
    vector<const LLVMFunctionArgument*> arguments;
    arguments.push_back(llvmFunctionArgument);
    Block* block = new Block();
    CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
    mLLVMFunction = new LLVMFunction("myfunction",
                                     LLVMPrimitiveTypes::I16,
                                     arguments,
                                     compoundStatement,
                                     0);
  }
};

TEST_F(LLVMFunctionTest, objectElementTypeTest) {
  EXPECT_FALSE(mLLVMFunction->isConstant());
  EXPECT_FALSE(mLLVMFunction->isField());
  EXPECT_FALSE(mLLVMFunction->isMethod());
  EXPECT_FALSE(mLLVMFunction->isStaticMethod());
  EXPECT_FALSE(mLLVMFunction->isMethodSignature());
  EXPECT_TRUE(mLLVMFunction->isLLVMFunction());
}

