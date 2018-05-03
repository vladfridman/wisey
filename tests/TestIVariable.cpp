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
#include "TestPrefix.hpp"
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
    TestPrefix::generateIR(mContext);
    
    ON_CALL(*mVariable, getName()).WillByDefault(Return("foo"));
    ON_CALL(*mVariable, getType()).WillByDefault(Return(PrimitiveTypes::INT));

    mContext.getScopes().pushScope();
    mContext.getScopes().setVariable(mContext, mVariable);
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
  EXPECT_EQ(IVariable::getVariable(mContext, "foo", 0), mVariable);
}

TEST_F(IVariableTest, getVariableDoesNotExistDeathTest) {
  Mock::AllowLeak(mVariable);
  
  EXPECT_EXIT(IVariable::getVariable(mContext, "bar", 7),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(7\\): Error: Undeclared variable 'bar'");
}

