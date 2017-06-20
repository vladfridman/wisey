//
//  TestIVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IVariable}

#include <gtest/gtest.h>

#include "wisey/IVariable.hpp"

using namespace std;
using namespace wisey;

struct IVariableTest : public ::testing::Test {
  IVariableTest() { }
};

TEST_F(IVariableTest, getTemporaryVariableNameTest) {
  string someObject;
  string name = IVariable::getTemporaryVariableName(&someObject);
  
  EXPECT_STREQ(name.substr(0, 5).c_str(), "__tmp");
  EXPECT_EQ(stol(name.substr(5).c_str()), (long) &someObject);
}
