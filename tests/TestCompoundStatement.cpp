//
//  TestCompoundStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CompoundStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockStatement.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/CompoundStatement.hpp"

using namespace llvm;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Test;

struct TestCompoundStatement : public Test {
  IRGenerationContext mContext;
  NiceMock<MockStatement>* mStatement1;
  NiceMock<MockStatement>* mStatement2;
  Block* mBlock;

  TestCompoundStatement() :
    mStatement1(new NiceMock<MockStatement>()),
    mStatement2(new NiceMock<MockStatement>()),
    mBlock(new Block()) {
  }
};

TEST_F(TestCompoundStatement, generateIRTest) {
  mBlock->getStatements().push_back(mStatement1);
  mBlock->getStatements().push_back(mStatement2);
  CompoundStatement compoundStatement(mBlock);
  
  EXPECT_CALL(*mStatement1, generateIR(_));
  EXPECT_CALL(*mStatement2, generateIR(_));
  
  compoundStatement.generateIR(mContext);
}

TEST_F(TestFileSampleRunner, compoundStatementOutOfScopeVariableRunDeathTest) {
  expectFailCompile("tests/samples/test_compound_statement_fail.yz",
                    1,
                    "Error: Undeclared variable 'j'");
}
