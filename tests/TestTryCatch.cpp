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

TEST_F(TestFileSampleRunner, tryCatchRunTest) {
  compileAndRunFile("tests/samples/test_try_catch.yz", 255);
}

TEST_F(TestFileSampleRunner, tryCatchTwoTypeRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_two_types.yz", 127);
}

TEST_F(TestFileSampleRunner, tryCatchCastToInterfaceRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_cast_to_interface.yz", 2);
}

TEST_F(TestFileSampleRunner, tryCatchContinueRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_continue.yz", 7);
}

TEST_F(TestFileSampleRunner, tryCatchReturnNoThrowRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_return_no_throw.yz", 3);
}

TEST_F(TestFileSampleRunner, tryCatchFinallyRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_finally.yz", 7);
}

TEST_F(TestFileSampleRunner, tryFinallyRunTest) {
  compileAndRunFile("tests/samples/test_try_finally.yz", 5);
}

TEST_F(TestFileSampleRunner, tryCatchRethrowRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_rethrow.yz", 127);
}

TEST_F(TestFileSampleRunner, tryCatchDirectThrowRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_direct_throw.yz", 31);
}

TEST_F(TestFileSampleRunner, tryCatchThroughMiddlemanRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_through_middleman.yz", 63);
}

TEST_F(TestFileSampleRunner, tryCatchNestedFinallyRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_nested_finally.yz", 45);
}

TEST_F(TestFileSampleRunner, tryThrowFromInsideCatchRunTest) {
  compileAndRunFile("tests/samples/test_throw_from_inside_catch.yz", 45);
}

TEST_F(TestFileSampleRunner, tryThrowFromInsideFinallyRunTest) {
  compileAndRunFile("tests/samples/test_throw_from_inside_finally.yz", 45);
}

TEST_F(TestFileSampleRunner, tryCatchTwoThrowsRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_two_throws.yz", 61);
}

TEST_F(TestFileSampleRunner, tryCatchNestedCatchInOuterTryRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_nested_catch_in_outer_try.yz", 65);
}

TEST_F(TestFileSampleRunner, tryCatchNestedUnmatchedThrowDeathRunTest) {
  expectFailCompile("tests/samples/test_try_catch_nested_unmatched_throw.yz",
                    1,
                    "Error: Method doTryCatch neither handles the exception "
                    "systems.vos.wisey.compiler.tests.MAnotherException nor throws it");
}

TEST_F(TestFileSampleRunner, tryCatchAccessTryVariableDeathRunTest) {
  expectFailCompile("tests/samples/test_try_catch_access_try_variable.yz",
                    1,
                    "Error: Undeclared variable 'a'");
}
