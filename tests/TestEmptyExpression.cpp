//
//  TestEmptyExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link EmptyExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "EmptyExpression.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"

using namespace wisey;

struct EmptyExpressionTest : public ::testing::Test {
  IRGenerationContext mContext;
  EmptyExpression mEmptyExpression;
  
  EmptyExpressionTest() { }
};

TEST_F(EmptyExpressionTest, simpleEmptyExpressionTest) {
  EXPECT_EQ(mEmptyExpression.generateIR(mContext, PrimitiveTypes::VOID), nullptr);
}

TEST_F(EmptyExpressionTest, emptyExpressionTypeTest) {
  EXPECT_EQ(mEmptyExpression.getType(mContext), PrimitiveTypes::VOID);
}

TEST_F(EmptyExpressionTest, isConstantTest) {
  EXPECT_FALSE(mEmptyExpression.isConstant());
}

TEST_F(EmptyExpressionTest, isAssignableTest) {
  EXPECT_FALSE(mEmptyExpression.isAssignable());
}

TEST_F(EmptyExpressionTest, printToStreamTest) {
  std::stringstream stringStream;
  mEmptyExpression.printToStream(mContext, stringStream);
  
  EXPECT_EQ(0u, stringStream.str().size());
}

