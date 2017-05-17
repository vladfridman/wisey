//
//  TestContinueStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/29/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ContinueStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileSampleRunner.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace yazyk;

TEST(ContinueStatementTest, noScopesTest) {
  IRGenerationContext context;
  
  EXPECT_EQ(context.getScopes().getContinueToBlock(), nullptr);
}

TEST_F(TestFileSampleRunner, continueForLoopRunTest) {
  runFile("tests/samples/test_continue_for_loop.yz", "9");
}

TEST_F(TestFileSampleRunner, continueNestedForLoopsRunTest) {
  runFile("tests/samples/test_continue_nested_for_loop.yz", "51");
}

TEST_F(TestFileSampleRunner, continueWhileLoopRunTest) {
  runFile("tests/samples/test_continue_while_loop.yz", "20");
}

TEST_F(TestFileSampleRunner, continueInIfStatementRunDeathTest) {
  expectFailIRGeneration("tests/samples/test_continue_in_if_statement.yz",
                         1,
                         "Error: continue statement not inside a loop");
}
