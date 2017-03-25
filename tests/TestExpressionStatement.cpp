//
//  TestExpressionStatement.cpp
//  Yazyk
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
#include "TestFileSampleRunner.hpp"
#include "yazyk/ExpressionStatement.hpp"
#include "yazyk/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::NiceMock;

TEST(ExpressionStatementTest, CreatingTest) {
  IRGenerationContext context;
  NiceMock<MockExpression> expression;
  ExpressionStatement expressionStatement(expression);
  EXPECT_CALL(expression, generateIR(_));
  
  expressionStatement.generateIR(context);
}
