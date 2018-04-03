//
//  TestSwitchStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link SwitchStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, switchStatementRunTest) {
  runFile("tests/samples/test_switch_statement.yz", "2");
}

TEST_F(TestFileRunner, switchStatementWithBreaksRunTest) {
  runFile("tests/samples/test_switch_statement_with_breaks.yz", "5");
}

TEST_F(TestFileRunner, switchStatementWithDefaultRunTest) {
  runFile("tests/samples/test_switch_statement_with_default.yz", "7");
}

TEST_F(TestFileRunner, switchStatementWithFallThroughRunTest) {
  runFile("tests/samples/test_switch_statement_with_fallthrough.yz", "7");
}
