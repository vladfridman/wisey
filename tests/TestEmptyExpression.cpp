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

TEST(EmptyExpressionTest, SimpleEmptyExpressionTest) {
  IRGenerationContext context;
  EmptyExpression expression;
  
  EXPECT_EQ(expression.generateIR(context), nullptr);
}

TEST(EmptyExpressionTest, TestEmptyExpressionType) {
  EmptyExpression expression;
 
  EXPECT_EQ(expression.getType(), PrimitiveTypes::VOID_TYPE);
}
