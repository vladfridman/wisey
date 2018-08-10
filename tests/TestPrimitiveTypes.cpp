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
  EXPECT_FALSE(PrimitiveTypes::isFloatType(PrimitiveTypes::BOOLEAN));
  EXPECT_FALSE(PrimitiveTypes::isFloatType(PrimitiveTypes::CHAR));
  EXPECT_FALSE(PrimitiveTypes::isFloatType(PrimitiveTypes::BYTE));
  EXPECT_FALSE(PrimitiveTypes::isFloatType(PrimitiveTypes::INT));
  EXPECT_FALSE(PrimitiveTypes::isFloatType(PrimitiveTypes::LONG));
  EXPECT_TRUE(PrimitiveTypes::isFloatType(PrimitiveTypes::FLOAT));
  EXPECT_TRUE(PrimitiveTypes::isFloatType(PrimitiveTypes::DOUBLE));
  EXPECT_FALSE(PrimitiveTypes::isFloatType(PrimitiveTypes::VOID));
}
