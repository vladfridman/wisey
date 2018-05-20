//
//  TestEmptyStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link EmptyStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileRunner.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace wisey;

TEST_F(TestFileRunner, emptyStatementRunTest) {
  runFile("tests/samples/test_empty_statement.yz", 5);
}
