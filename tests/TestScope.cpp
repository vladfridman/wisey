//
//  TestScope.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Scope}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "yazyk/Scope.hpp"
#include "yazyk/IRGenerationContext.hpp"

using namespace llvm;
using namespace yazyk;

using ::testing::Test;

struct ScopeTest : public Test {
  IRGenerationContext mContext;
  BasicBlock* mBlock = BasicBlock::Create(mContext.getLLVMContext(), "entry");
  Scope mScope;
 
public:
  
  ScopeTest() : mScope(Scope()) { }
  
  ~ScopeTest() {
    delete mBlock;
  }
};

TEST_F(ScopeTest, LocalsTest) {
  LLVMContext& llvmContext = mContext.getLLVMContext();
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);
  mScope.getLocals()["foo"] = fooValue;
  
  EXPECT_EQ(mScope.getLocals()["foo"], fooValue);
  EXPECT_EQ(mScope.getLocals()["bar"] == NULL, true);
}
