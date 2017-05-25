//
//  TestBreakStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/29/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link BreakStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileSampleRunner.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace wisey;

TEST(BreakStatementTest, noScopesTest) {
  IRGenerationContext context;
  
  EXPECT_EQ(context.getScopes().getBreakToBlock(), nullptr);
}

TEST_F(TestFileSampleRunner, breakOutOfForLoopRunTest) {
  runFile("tests/samples/test_break_out_of_for_loop.yz", "5");
}

TEST_F(TestFileSampleRunner, breakOutOfNestedForLoopsRunTest) {
  runFile("tests/samples/test_break_out_of_nested_for_loop.yz", "99");
}

TEST_F(TestFileSampleRunner, breakOutOfWhileLoopRunTest) {
  runFile("tests/samples/test_break_out_of_while_loop.yz", "51");
}

TEST_F(TestFileSampleRunner, breakOutOfIfStatementRunDeathTest) {
  expectFailCompile("tests/samples/test_break_out_of_if_statement.yz",
                    1,
                    "Error: break statement not inside a loop or a switch");
}
