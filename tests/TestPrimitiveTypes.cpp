//
//  TestPrimitiveTypes.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link PrimitiveTypes}
//

#include <gtest/gtest.h>

#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct PrimitiveTypesTest : public Test {
  
  PrimitiveTypesTest() { }
  
};

TEST_F(PrimitiveTypesTest, isFloatTypeTest) {
  EXPECT_FALSE(PrimitiveTypes::isFloatType(PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_FALSE(PrimitiveTypes::isFloatType(PrimitiveTypes::CHAR_TYPE));
  EXPECT_FALSE(PrimitiveTypes::isFloatType(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(PrimitiveTypes::isFloatType(PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(PrimitiveTypes::isFloatType(PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(PrimitiveTypes::isFloatType(PrimitiveTypes::DOUBLE_TYPE));
  EXPECT_FALSE(PrimitiveTypes::isFloatType(PrimitiveTypes::VOID_TYPE));
}
