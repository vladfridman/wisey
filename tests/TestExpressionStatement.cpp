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
#include "ExpressionStatement.hpp"
#include "IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Test;

struct ExpressionStatementTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression>* mExpression;
  BasicBlock* mBasicBlock;

  ExpressionStatementTest() : mExpression(new NiceMock<MockExpression>()) {
    mContext.getScopes().pushScope();
    mBasicBlock = BasicBlock::Create(mContext.getLLVMContext());
    mContext.setBasicBlock(mBasicBlock);
  }
};

TEST_F(ExpressionStatementTest, generateIRTest) {
  ExpressionStatement expressionStatement(mExpression, 0);
  EXPECT_CALL(*mExpression, generateIR(_, _));
  
  expressionStatement.generateIR(mContext);
}

TEST_F(TestFileRunner, unreachableExpressionStatementRunDeathTest) {
  expectFailCompile("tests/samples/test_unreachable_expression_statement.yz",
                    1,
                    "tests/samples/test_unreachable_expression_statement.yz\\(10\\): Error: Statement unreachable");
}
