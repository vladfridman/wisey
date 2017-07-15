//
//  TestTryCatch.cpp
//  Wisey
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

TEST_F(TestFileSampleRunner, tryCatchFinallyTest) {
  compileAndRunFile("tests/samples/test_try_catch_finally.yz", 7);
}

TEST_F(TestFileSampleRunner, tryFinallyTest) {
  compileAndRunFile("tests/samples/test_try_finally.yz", 5);
}

TEST_F(TestFileSampleRunner, tryCatchRethrowTest) {
  compileAndRunFile("tests/samples/test_try_catch_rethrow.yz", 127);
}

TEST_F(TestFileSampleRunner, tryCatchDirectThrowTest) {
  compileAndRunFile("tests/samples/test_try_catch_direct_throw.yz", 31);
}

TEST_F(TestFileSampleRunner, tryCatchThroughMiddlemanTest) {
  compileAndRunFile("tests/samples/test_try_catch_through_middleman.yz", 63);
}

TEST_F(TestFileSampleRunner, tryCatchNestedUnmatchedThrowDeathTest) {
  expectFailCompile("tests/samples/test_try_catch_nested_unmatched_throw.yz",
                    1,
                    "Error: Method doTryCatch neither handles the exception "
                    "systems.vos.wisey.compiler.tests.MAnotherException nor throws it");
}
