//
//  TestCaseStatement.cpp
//  Yazyk
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

#include "yazyk/Block.hpp"
#include "yazyk/CaseStatement.hpp"
#include "yazyk/Float.hpp"
#include "yazyk/Integer.hpp"
#include "yazyk/IRGenerationContext.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace yazyk;

class MockExpression : public IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
};

class MockStatement : public IStatement {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
};

TEST(CaseStatementTest, TestStatementsGetGenerated) {
  IRGenerationContext context;
  NiceMock<MockStatement> statement;
  NiceMock<MockExpression> expression;
  Block statementBlock;
  statementBlock.getStatements().push_back(&statement);
  CaseStatement caseStatement(expression, statementBlock);
  
  EXPECT_CALL(statement, generateIR(_));
  
  caseStatement.generateIR(context);
}

TEST(CaseStatementTest, TestConstIntExpressionWorks) {
  IRGenerationContext context;
  Block statementBlock;
  Integer caseExpression(5l);
  CaseStatement caseStatement(caseExpression, statementBlock);
  
  Value* expected = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 5);
  
  EXPECT_EQ(caseStatement.getExpressionValue(context), expected);
}

TEST(CaseStatementTest, TestNonIntExpressionDeathTest) {
  IRGenerationContext context;
  Block statementBlock;
  Float caseExpression(5.2f);
  CaseStatement caseStatement(caseExpression, statementBlock);
  
  EXPECT_EXIT(caseStatement.getExpressionValue(context),
              ::testing::ExitedWithCode(1),
              "Error: Case expression should be an integer constant");
}
