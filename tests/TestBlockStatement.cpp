//
//  TestBlockStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link BlockStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "yazyk/BlockStatement.hpp"
#include "yazyk/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::NiceMock;

class MockStatement : public IStatement {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
};

TEST(BlockStatementTest, SimepleTest) {
  IRGenerationContext context;
  NiceMock<MockStatement> mockStatement;
  Block block;
  block.getStatements().push_back(&mockStatement);
  BlockStatement blockStatement(block);
  
  EXPECT_CALL(mockStatement, generateIR(_));

  blockStatement.generateIR(context);
}