//
//  TestIVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IVariable}

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "MockVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;

struct IVariableTest : public ::testing::Test {
  IRGenerationContext mContext;
  NiceMock<MockVariable>* mVariable;

  IVariableTest() : mVariable(new NiceMock<MockVariable>()) {
    ON_CALL(*mVariable, getName()).WillByDefault(Return("foo"));
    ON_CALL(*mVariable, getType()).WillByDefault(Return(PrimitiveTypes::INT_TYPE));

    mContext.getScopes().pushScope();
    mContext.getScopes().setVariable(mVariable);
  }
  
  ~IVariableTest() {
    delete mVariable;
  }
};

TEST_F(IVariableTest, getTemporaryVariableNameTest) {
  string someObject;
  string name = IVariable::getTemporaryVariableName(&someObject);
  
  EXPECT_STREQ(name.substr(0, 5).c_str(), "__tmp");
  EXPECT_EQ(stol(name.substr(5).c_str()), (long) &someObject);
}

TEST_F(IVariableTest, getVariableTest) {
  EXPECT_EQ(IVariable::getVariable(mContext, "foo"), mVariable);
}

TEST_F(IVariableTest, getVariableDoesNotExistDeathTest) {
  Mock::AllowLeak(mVariable);
  
  EXPECT_EXIT(IVariable::getVariable(mContext, "bar"),
              ::testing::ExitedWithCode(1),
              "Error: Undeclared variable 'bar'");
}

TEST_F(IVariableTest, getVariableWasClearedDeathTest) {
  Mock::AllowLeak(mVariable);
  
  mContext.getScopes().clearVariable(mContext, "foo");
  
  EXPECT_EXIT(IVariable::getVariable(mContext, "foo"),
              ::testing::ExitedWithCode(1),
              "Error: Variable 'foo' was previously cleared and can not be used");
}
