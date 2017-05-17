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
#include "wisey/Block.hpp"
#include "wisey/CaseStatement.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/IRGenerationContext.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

TEST(CaseStatementTest, statementsGetGeneratedTest) {
  IRGenerationContext context;
  NiceMock<MockStatement> statement;
  NiceMock<MockExpression> expression;
  Block statementBlock;
  statementBlock.getStatements().push_back(&statement);
  CaseStatement* caseStatement = CaseStatement::newCaseStatement(expression, statementBlock);
  
  EXPECT_CALL(statement, generateIR(_));
  
  caseStatement->generateIR(context);
  delete caseStatement;
}

TEST(CaseStatementTest, constantIntExpressionWorksTest) {
  IRGenerationContext context;
  Block statementBlock;
  IntConstant expression(5l);
  CaseStatement* caseStatement = CaseStatement::newCaseStatement(expression, statementBlock);
  
  Value* expected = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 5);
  
  EXPECT_EQ(caseStatement->getExpressionValue(context), expected);
  delete caseStatement;
}

TEST(CaseStatementTest, nonIntExpressionDeathTest) {
  IRGenerationContext context;
  Block statementBlock;
  FloatConstant expression(5.2f);
  CaseStatement* caseStatement = CaseStatement::newCaseStatement(expression, statementBlock);
  
  EXPECT_EXIT(caseStatement->getExpressionValue(context),
              ::testing::ExitedWithCode(1),
              "Error: Case expression should be an integer constant");
}

TEST(CaseStatementTest, statementsIsMarkedFallThroughTest) {
  IRGenerationContext context;
  NiceMock<MockExpression> expression;
  Block statementBlock;
  CaseStatement* caseStatement = CaseStatement::newCaseStatement(expression, statementBlock);
  CaseStatement* caseStatementWithFallThrough =
    CaseStatement::newCaseStatementWithFallThrough(expression, statementBlock);
  
  EXPECT_EQ(false, caseStatement->isFallThrough());
  EXPECT_EQ(true, caseStatementWithFallThrough->isFallThrough());
  
  delete caseStatement;
  delete caseStatementWithFallThrough;
}
