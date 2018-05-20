//
//  TestContinueStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/29/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ContinueStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileRunner.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace wisey;

TEST(ContinueStatementTest, noScopesTest) {
  IRGenerationContext context;
  
  EXPECT_EQ(context.getScopes().getContinueToBlock(), nullptr);
}

TEST_F(TestFileRunner, continueForLoopRunTest) {
  runFile("tests/samples/test_continue_for_loop.yz", 9);
}

TEST_F(TestFileRunner, continueNestedForLoopsRunTest) {
  runFile("tests/samples/test_continue_nested_for_loop.yz", 51);
}

TEST_F(TestFileRunner, continueWhileLoopRunTest) {
  runFile("tests/samples/test_continue_while_loop.yz", 20);
}

TEST_F(TestFileRunner, continueInIfStatementRunDeathTest) {
  expectFailCompile("tests/samples/test_continue_in_if_statement.yz",
                    1,
                    "tests/samples/test_continue_in_if_statement.yz\\(9\\): Error: continue statement not inside a loop");
}

TEST_F(TestFileRunner, unreachableContinueStatementRunDeathTest) {
  expectFailCompile("tests/samples/test_unreachable_continue_statement.yz",
                    1,
                    "tests/samples/test_unreachable_continue_statement.yz\\(9\\): Error: Statement unreachable");
}
