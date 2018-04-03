//
//  TestExpressionStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/19/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ExpressionStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "wisey/ExpressionStatement.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Test;

struct ExpressionStatementTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression>* mExpression;
  
  ExpressionStatementTest() : mExpression(new NiceMock<MockExpression>()) {
  }
};

TEST_F(ExpressionStatementTest, generateIRTest) {
  ExpressionStatement expressionStatement(mExpression);
  EXPECT_CALL(*mExpression, generateIR(_, _));
  
  expressionStatement.generateIR(mContext);
}
