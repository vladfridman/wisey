//
//  TestTryCatch.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/21/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests try/catch statement
//

#include "TestFileSampleRunner.hpp"

TEST_F(TestFileSampleRunner, tryCatchParseTest) {
  runFile("tests/samples/test_try_catch.yz", "5");
}

