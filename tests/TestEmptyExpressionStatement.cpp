//
//  TestEmptyExpressionStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link EmptyExpressionStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/EmptyExpressionStatement.hpp"

using namespace yazyk;

TEST(TestEmptyExpressionStatement, SimpleEmptyExpressionTest) {
  IRGenerationContext context;
  EmptyExpressionStatement statement;
  
  EXPECT_EQ(statement.generateIR(context) == NULL, true);
}

TEST_F(TestFileSampleRunner, EmptyExpressionStatementRunTest) {
  runFile("tests/samples/test_empty_expression_statement.yz", "5");
}
