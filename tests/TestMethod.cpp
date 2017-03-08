//
//  TestMethod.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Method}
//

#include <gtest/gtest.h>

#include "yazyk/Method.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct MethodTest : public Test {
  Method* mMethod;
  
public:
  
  MethodTest() {
    MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble");
    MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar");
    std::vector<MethodArgument*> arguments;
    arguments.push_back(doubleArgument);
    arguments.push_back(charArgument);
    mMethod = new Method("mymethod", PrimitiveTypes::BOOLEAN_TYPE, arguments, 0);
  }
};

TEST_F(MethodTest, BasicMethodsTest) {
  ASSERT_STREQ(mMethod->getName().c_str(), "mymethod");
  ASSERT_EQ(mMethod->getReturnType(), PrimitiveTypes::BOOLEAN_TYPE);
  ASSERT_EQ(mMethod->getArguments().size(), 2u);
}

TEST_F(MethodTest, EqualsTest) {
  MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble2");
  MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar2");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(doubleArgument);
  arguments.push_back(charArgument);
  Method method("mymethod", PrimitiveTypes::BOOLEAN_TYPE, arguments, 0);
  
  ASSERT_TRUE(method.equals(mMethod));
}

TEST_F(MethodTest, NameNotEqualsTest) {
  MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble2");
  MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar2");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(doubleArgument);
  arguments.push_back(charArgument);
  Method method("differentname", PrimitiveTypes::BOOLEAN_TYPE, arguments, 0);
  
  ASSERT_FALSE(method.equals(mMethod));
}

TEST_F(MethodTest, NumberOfArgumentsNotEqualsTest) {
  MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble2");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(doubleArgument);
  Method method("mymethod", PrimitiveTypes::BOOLEAN_TYPE, arguments, 0);
  
  ASSERT_FALSE(method.equals(mMethod));
}

TEST_F(MethodTest, TypeOfArgumentsNotEqualsTest) {
  MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble2");
  MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "argChar2");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(doubleArgument);
  arguments.push_back(charArgument);
  Method method("mymethod", PrimitiveTypes::BOOLEAN_TYPE, arguments, 0);
  
  ASSERT_FALSE(method.equals(mMethod));
}
