//
//  TestBreakStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/29/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link BreakStatement}
//

#include "TestFileSampleRunner.hpp"

TEST_F(TestFileSampleRunner, BreakOutOfForLoopTest) {
  runFile("tests/samples/test_break_out_of_for_loop.yz", "5");
}

TEST_F(TestFileSampleRunner, BreakOutOfNestedForLoopsTest) {
  runFile("tests/samples/test_break_out_of_nested_for_loop.yz", "99");
}

TEST_F(TestFileSampleRunner, BreakOutOfWhileLoopTest) {
  runFile("tests/samples/test_break_out_of_while_loop.yz", "51");
}

TEST_F(TestFileSampleRunner, BreakOutOfIfStatementTest) {
  expectFailIRGeneration("tests/samples/test_break_out_of_if_statement.yz",
                         1,
                         "Error: break statement not inside a loop or a switch");
}
