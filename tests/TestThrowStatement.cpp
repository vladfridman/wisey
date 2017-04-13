//
//  TestThrowStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ThrowStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileSampleRunner.hpp"

TEST_F(TestFileSampleRunner, throwStatementParseRunTest) {
  runFile("tests/samples/test_throw.yz", "3");
}
