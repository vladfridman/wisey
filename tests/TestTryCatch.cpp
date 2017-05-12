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

TEST_F(TestFileSampleRunner, tryCatchTest) {
  compileAndRunFile("tests/samples/test_try_catch.yz", 255);
}

TEST_F(TestFileSampleRunner, tryCatchTwoTypeTest) {
  compileAndRunFile("tests/samples/test_try_catch_two_types.yz", 127);
}

TEST_F(TestFileSampleRunner, tryCatchCastToInterfaceTest) {
  compileAndRunFile("tests/samples/test_try_catch_cast_to_interface.yz", 2);
}

TEST_F(TestFileSampleRunner, tryCatchContinueTest) {
  compileAndRunFile("tests/samples/test_try_catch_continue.yz", 7);
}

TEST_F(TestFileSampleRunner, tryCatchReturnNoThrowTest) {
  compileAndRunFile("tests/samples/test_try_catch_return_no_throw.yz", 3);
}

TEST_F(TestFileSampleRunner, tryCatchReturnFinallyTest) {
  compileAndRunFile("tests/samples/test_try_catch_finally.yz", 3);
}
