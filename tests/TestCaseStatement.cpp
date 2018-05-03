//
//  TestCaseStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CaseStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockStatement.hpp"
#include "TestPrefix.hpp"
#include "wisey/Block.hpp"
#include "wisey/CaseStatement.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/IRGenerationContext.hpp"

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct CaseStatementTest : public Test {
  IRGenerationContext mContext;
  Block* mStatementBlock;
  NiceMock<MockExpression>* mExpression;
  
  CaseStatementTest() : mStatementBlock(new Block(0)), mExpression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
  }
};

TEST_F(CaseStatementTest, statementsGetGeneratedTest) {
  NiceMock<MockStatement>* statement = new NiceMock<MockStatement>();
  mStatementBlock->getStatements().push_back(statement);
  CaseStatement* caseStatement = CaseStatement::newCaseStatement(mExpression, mStatementBlock, 0);
  
  EXPECT_CALL(*statement, generateIR(_));
  
  caseStatement->generateIR(mContext);
  delete caseStatement;
}

TEST_F(CaseStatementTest, constantIntExpressionWorksTest) {
  IntConstant* expression = new IntConstant(5l, 0);
  CaseStatement* caseStatement = CaseStatement::newCaseStatement(expression, mStatementBlock, 0);
  
  Value* expected = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
  
  EXPECT_EQ(caseStatement->getExpressionValue(mContext), expected);
  delete caseStatement;
}

TEST_F(CaseStatementTest, nonIntExpressionDeathTest) {
  FloatConstant* expression = new FloatConstant(5.2f, 0);
  CaseStatement* caseStatement = CaseStatement::newCaseStatement(expression, mStatementBlock, 5);
  
  Mock::AllowLeak(expression);
  Mock::AllowLeak(mStatementBlock);
  
  EXPECT_EXIT(caseStatement->getExpressionValue(mContext),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Case expression should be an integer constant");
  
  delete caseStatement;
}

TEST_F(CaseStatementTest, statementsIsNotMarkedFallThroughTest) {
  CaseStatement* caseStatement = CaseStatement::newCaseStatement(mExpression, mStatementBlock, 0);
  
  EXPECT_EQ(false, caseStatement->isFallThrough());
  
  delete caseStatement;
}


TEST_F(CaseStatementTest, statementsIsMarkedFallThroughTest) {
  CaseStatement* caseStatementWithFallThrough =
    CaseStatement::newCaseStatementWithFallThrough(mExpression, mStatementBlock, 0);
  
  EXPECT_EQ(true, caseStatementWithFallThrough->isFallThrough());
  
  delete caseStatementWithFallThrough;
}
