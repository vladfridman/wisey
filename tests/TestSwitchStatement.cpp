//
//  TestSwitchStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link SwitchStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileSampleRunner.hpp"

TEST_F(TestFileSampleRunner, SwitchStatementTest) {
  runFile("tests/samples/test_switch_statement.yz", "2");
}

TEST_F(TestFileSampleRunner, SwitchStatementWithBreaksTest) {
  runFile("tests/samples/test_switch_statement_with_breaks.yz", "5");
}

TEST_F(TestFileSampleRunner, SwitchStatementWithDefaultTest) {
  runFile("tests/samples/test_switch_statement_with_default.yz", "7");
}
