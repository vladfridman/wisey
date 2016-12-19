//
//  TestTypeSpecifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link TypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "yazyk/TypeSpecifier.hpp"

using namespace yazyk;

TEST(TypeSpecifierTest, CreationTest) {
  IRGenerationContext context;
  TypeSpecifier typeSpecifier(PRIMITIVE_TYPE_INT);
  
  EXPECT_EQ(typeSpecifier.getType(), PRIMITIVE_TYPE_INT);
  EXPECT_EQ(typeSpecifier.generateIR(context) == NULL, true);
}
