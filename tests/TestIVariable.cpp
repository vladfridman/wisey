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
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "IVariable.hpp"
#include "PrimitiveTypes.hpp"

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
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(IVariable::getVariable(mContext, "bar", 7));
  EXPECT_STREQ("/tmp/source.yz(7): Error: Undeclared variable 'bar'\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, accessFieldVariableFromStaticMethodRunDeathTest) {
  expectFailCompile("tests/samples/test_access_field_variable_from_static_method.yz",
                    1,
                    "tests/samples/test_access_field_variable_from_static_method.yz\\(9\\): "
                    "Error: Member variables are not accessible from static methods");
}
