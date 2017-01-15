//
//  TestCompoundStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CompoundStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/CompoundStatement.hpp"

using namespace llvm;
using namespace yazyk;

using ::testing::_;
using ::testing::NiceMock;

class MockStatement : public IStatement {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
};

TEST(TestCompoundStatement, SimpleCompoundStatementTest) {
  NiceMock<MockStatement> mockStatement1;
  NiceMock<MockStatement> mockStatement2;
  IRGenerationContext context;
  Block block;
  block.getStatements().push_back(&mockStatement1);
  block.getStatements().push_back(&mockStatement2);
  CompoundStatement compoundStatement(block);
  
  EXPECT_CALL(mockStatement1, generateIR(_));
  EXPECT_CALL(mockStatement2, generateIR(_));
  
  compoundStatement.generateIR(context);
}

TEST_F(TestFileSampleRunner, CompoundStatementOutOfScopeVariableTest) {
  expectFailIRGeneration("tests/samples/test_compound_statement_fail.yz",
                         1,
                         "Error: Undeclared variable j");
}