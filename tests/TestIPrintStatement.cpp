//
//  TestIPrintStatement.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IPrintStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>

#include "MockExpression.hpp"
#include "wisey/AdditiveMultiplicativeExpression.hpp"
#include "wisey/IPrintStatement.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct IPrintStatementTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mLeftExpression;
  NiceMock<MockExpression>* mRightExpression;

  IPrintStatementTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mLeftExpression(new NiceMock<MockExpression>()),
  mRightExpression(new NiceMock<MockExpression>()) {
    ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::STRING_TYPE));
  }
};

TEST_F(IPrintStatementTest, getExpressionsTest) {
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression, 0);

  ExpressionList expressions = IPrintStatement::getExpressions(mContext, &expression, 0);
  
  EXPECT_EQ(2u, expressions.size());
  EXPECT_EQ(mLeftExpression, expressions.front());
  EXPECT_EQ(mRightExpression, expressions.back());
}
