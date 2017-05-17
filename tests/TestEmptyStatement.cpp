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
#include "wisey/EmptyStatement.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace yazyk;

TEST(TestEmptyStatement, simpleEmptyStatementTest) {
  IRGenerationContext context;
  
  EXPECT_EQ(EmptyStatement::EMPTY_STATEMENT.generateIR(context), nullptr);
}

TEST_F(TestFileSampleRunner, emptyStatementRunTest) {
  runFile("tests/samples/test_empty_statement.yz", "5");
}
