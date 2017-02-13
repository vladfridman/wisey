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
#include "yazyk/LocalHeapVariable.hpp"
#include "yazyk/LocalStackVariable.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace std;
using namespace llvm;
using namespace yazyk;

using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Test;

class MockVariable : public IVariable {
public:
  MOCK_CONST_METHOD0(getName, string ());
  MOCK_CONST_METHOD0(getType, IType* ());
  MOCK_CONST_METHOD0(getValue, Value* ());
  MOCK_CONST_METHOD2(generateIdentifierIR, Value* (IRGenerationContext&, string));
  MOCK_METHOD2(generateAssignmentIR, Value* (IRGenerationContext&, IExpression&));
  MOCK_CONST_METHOD1(free, void (BasicBlock*));
};

struct ScopeTest : public Test {
  IRGenerationContext mContext;
  Scope mScope;
  NiceMock<MockVariable>* mMockVariable;
 
public:
  
  ScopeTest() : mScope(Scope()) {
    mMockVariable = new NiceMock<MockVariable>();
  }
};

TEST_F(ScopeTest, LocalsTest) {
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

TEST_F(ScopeTest, ClearNonExistantVariableDeathTest) {
  EXPECT_EXIT(mScope.clearVariable("foo"),
              ::testing::ExitedWithCode(1),
              "Error: Variable 'foo' is not set in this scope.");
}

TEST_F(ScopeTest, MaybeFreeOwnedMemoryHeapVariableTest) {
  mScope.setVariable("foo", mMockVariable);
  
  EXPECT_CALL(*mMockVariable, free(mContext.getBasicBlock()));
  
  mScope.maybeFreeOwnedMemory(mContext.getBasicBlock());
}
