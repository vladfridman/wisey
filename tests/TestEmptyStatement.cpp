//
//  TestEmptyStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link EmptyStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/EmptyStatement.hpp"

using namespace yazyk;

TEST(TestEmptyStatement, SimpleEmptyStatementTest) {
  IRGenerationContext context;
  EmptyStatement statement;
  
  EXPECT_EQ(statement.generateIR(context), nullptr);
}

TEST_F(TestFileSampleRunner, EmptyStatementRunTest) {
  runFile("tests/samples/test_empty_statement.yz", "5");
}
