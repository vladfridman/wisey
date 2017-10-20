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

#include "wisey/EmptyExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace wisey;

struct EmptyExpressionTest : public ::testing::Test {
  IRGenerationContext mContext;
  EmptyExpression mEmptyExpression;
  
  EmptyExpressionTest() { }
};

TEST_F(EmptyExpressionTest, getVariableTest) {
  EXPECT_EQ(mEmptyExpression.getVariable(mContext), nullptr);
}

TEST_F(EmptyExpressionTest, simpleEmptyExpressionTest) {
  EXPECT_EQ(mEmptyExpression.generateIR(mContext), nullptr);
}

TEST_F(EmptyExpressionTest, emptyExpressionTypeTest) {
  EXPECT_EQ(mEmptyExpression.getType(mContext), PrimitiveTypes::VOID_TYPE);
}

TEST_F(EmptyExpressionTest, existsInOuterScopeTest) {
  EXPECT_FALSE(mEmptyExpression.existsInOuterScope(mContext));
}

TEST_F(EmptyExpressionTest, isConstantTest) {
  EXPECT_FALSE(mEmptyExpression.isConstant());
}

TEST_F(EmptyExpressionTest, printToStreamTest) {
  std::stringstream stringStream;
  mEmptyExpression.printToStream(mContext, stringStream);
  
  EXPECT_EQ(0u, stringStream.str().size());
}

TEST_F(EmptyExpressionTest, releaseOwnershipDeathTest) {
  EXPECT_EXIT(mEmptyExpression.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of an empty epxression, it is not a heap pointer");
}

TEST_F(EmptyExpressionTest, addReferenceToOwnerDeathTest) {
  EXPECT_EXIT(mEmptyExpression.addReferenceToOwner(mContext, NULL),
              ::testing::ExitedWithCode(1),
              "Error: Can not add a reference to non owner type empty expression");
}
