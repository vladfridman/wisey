//
//  TestEmptyExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link EmptyExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "yazyk/EmptyExpression.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace yazyk;

TEST(EmptyExpressionTest, simpleEmptyExpressionTest) {
  IRGenerationContext context;
  EmptyExpression expression;
  
  EXPECT_EQ(expression.generateIR(context), nullptr);
}

TEST(EmptyExpressionTest, emptyExpressionTypeTest) {
  IRGenerationContext context;
  EmptyExpression expression;
 
  EXPECT_EQ(expression.getType(context), PrimitiveTypes::VOID_TYPE);
}

TEST(EmptyExpressionTest, releaseOwnershipDeathTest) {
  IRGenerationContext context;
  EmptyExpression expression;
  
  EXPECT_EXIT(expression.releaseOwnership(context),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of an empty epxression, it is not a heap pointer");
}
