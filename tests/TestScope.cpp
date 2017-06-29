//
//  TestScope.cpp
//  Wisey
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
#include "wisey/Scope.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/HeapVariable.hpp"
#include "wisey/StackVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ScopeTest : public Test {
  IRGenerationContext mContext;
  Scope mScope;
  NiceMock<MockVariable>* mFooVariable;
  NiceMock<MockVariable>* mBarVariable;
  NiceMock<MockType> mMockType;

public:

  ScopeTest() :
  mFooVariable(new NiceMock<MockVariable>()),
  mBarVariable(new NiceMock<MockVariable>()) {
    ON_CALL(mMockType, getName()).WillByDefault(Return("MExceptionA"));
  }
};

TEST_F(ScopeTest, localsTest) {
  mScope.setVariable("foo", mFooVariable);
  
  EXPECT_EQ(mScope.findVariable("foo"), mFooVariable);
  EXPECT_EQ(mScope.findVariable("bar"), nullptr);
  
  mScope.setVariable("bar", mBarVariable);
  
  EXPECT_EQ(mScope.findVariable("foo"), mFooVariable);
  EXPECT_EQ(mScope.findVariable("bar"), mBarVariable);
}

TEST_F(ScopeTest, freeOwnedMemoryTest) {
  mScope.setVariable("foo", mFooVariable);
  
  EXPECT_CALL(*mFooVariable, free(_));
  
  mScope.freeOwnedMemory(mContext);
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
  
  vector<const IType*> exceptions;
  exceptions.push_back(&mMockType);
  exceptions.push_back(&mockType);
  
  mScope.addExceptions(exceptions);
  
  ASSERT_EQ(mScope.getExceptions().size(), 2u);
  
  mScope.removeException(&mMockType);
  mScope.removeException(&mockType);
  
  ASSERT_EQ(mScope.getExceptions().size(), 0u);
}

TEST_F(ScopeTest, eraseClearedVariablesTest) {
  map<string, IVariable*> clearedVariables;
  clearedVariables["foo"] = mFooVariable;
  mScope.setVariable("foo", mFooVariable);

  mScope.eraseClearedVariables(clearedVariables);
  
  EXPECT_EQ(clearedVariables.size(), 0u);
  EXPECT_EQ(mScope.findVariable("foo"), nullptr);
}
