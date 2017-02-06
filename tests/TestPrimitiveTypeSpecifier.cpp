//
//  TestPrimitiveTypeSpecifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link PrimitiveTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace yazyk;

TEST(PrimitiveTypeSpecifierTest, CreationTest) {
  IRGenerationContext context;
  PrimitiveTypeSpecifier primitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  
  EXPECT_EQ(primitiveTypeSpecifier.getType(context), PrimitiveTypes::INT_TYPE);
  EXPECT_EQ(primitiveTypeSpecifier.getStorageType(), STACK_VARIABLE);
}
