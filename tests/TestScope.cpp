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

#include "MockType.hpp"
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
using ::testing::Return;
using ::testing::Test;

struct ScopeTest : public Test {
  IRGenerationContext mContext;
  Scope mScope;
  NiceMock<MockVariable>* mMockVariable;
  NiceMock<MockType> mMockType;
 
public:
  
  ScopeTest() : mScope(Scope()) {
    mMockVariable = new NiceMock<MockVariable>();
    ON_CALL(mMockType, getName()).WillByDefault(Return("MExceptionA"));
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

TEST_F(ScopeTest, addExceptionTest) {
  ASSERT_EQ(mScope.getExceptions().size(), 0u);
  
  mScope.addException(&mMockType);
  
  ASSERT_EQ(mScope.getExceptions().size(), 1u);
  
  mScope.removeException(&mMockType);
  
  ASSERT_EQ(mScope.getExceptions().size(), 0u);
}

TEST_F(ScopeTest, addExceptionsTest) {
  NiceMock<MockType> mockType;
  ON_CALL(mockType, getName()).WillByDefault(Return("MExceptionB"));

  ASSERT_EQ(mScope.getExceptions().size(), 0u);
  
  vector<IType*> exceptions;
  exceptions.push_back(&mMockType);
  exceptions.push_back(&mockType);
  
  mScope.addExceptions(exceptions);
  
  ASSERT_EQ(mScope.getExceptions().size(), 2u);
  
  mScope.removeException(&mMockType);
  mScope.removeException(&mockType);
  
  ASSERT_EQ(mScope.getExceptions().size(), 0u);
}

