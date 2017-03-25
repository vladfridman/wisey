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

#include "MockVariable.hpp"
#include "yazyk/Scope.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LocalHeapVariable.hpp"
#include "yazyk/LocalStackVariable.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace std;
using namespace llvm;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Test;

struct ScopeTest : public Test {
  IRGenerationContext mContext;
  Scope mScope;
  NiceMock<MockVariable>* mMockVariable;
 
public:
  
  ScopeTest() : mScope(Scope()) {
    mMockVariable = new NiceMock<MockVariable>();
  }
};

TEST_F(ScopeTest, localsTest) {
  mScope.setVariable("foo", mMockVariable);
  
  EXPECT_EQ(mScope.findVariable("foo"), mMockVariable);
  EXPECT_EQ(mScope.findVariable("bar"), nullptr);
  
  mScope.setVariable("bar", mMockVariable);
  
  EXPECT_EQ(mScope.findVariable("foo"), mMockVariable);
  EXPECT_EQ(mScope.findVariable("bar"), mMockVariable);
  
  mScope.clearVariable("foo");

  EXPECT_EQ(mScope.findVariable("foo"), nullptr);
  EXPECT_EQ(mScope.findVariable("bar"), mMockVariable);
}

TEST_F(ScopeTest, clearNonExistantVariableDeathTest) {
  EXPECT_EXIT(mScope.clearVariable("foo"),
              ::testing::ExitedWithCode(1),
              "Error: Variable 'foo' is not set in this scope.");
}

TEST_F(ScopeTest, maybeFreeOwnedMemoryHeapVariableTest) {
  mScope.setVariable("foo", mMockVariable);
  
  EXPECT_CALL(*mMockVariable, free(_));
  
  mScope.maybeFreeOwnedMemory(mContext);
}
