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

TEST_F(TestFileSampleRunner, tryCatchRethrowRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_rethrow.yz", 127);
}

TEST_F(TestFileSampleRunner, tryCatchDirectThrowRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_direct_throw.yz", 31);
}

TEST_F(TestFileSampleRunner, tryCatchThroughMiddlemanRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_through_middleman.yz", 63);
}

TEST_F(TestFileSampleRunner, tryThrowFromInsideCatchRunTest) {
  compileAndRunFile("tests/samples/test_throw_from_inside_catch.yz", 37);
}

TEST_F(TestFileSampleRunner, tryCatchTwoThrowsRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_two_throws.yz", 45);
}

TEST_F(TestFileSampleRunner, tryCatchNestedInCatchTryRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_nested_in_catch.yz", 5);
}

TEST_F(TestFileSampleRunner, tryCatchNestedInCatchWithCleanupOwnerTryRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_nested_in_catch_with_cleanup_owner.yz", 5);
}

TEST_F(TestFileSampleRunner, tryCatchNestedInCatchWithCleanupReferenceTryRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_nested_in_catch_with_cleanup_reference.yz", 5);
}

TEST_F(TestFileSampleRunner, tryCatchWithCleanupReferenceFromTryRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_with_cleanup_reference_from_try.yz", 255);
}

TEST_F(TestFileSampleRunner, tryCatchWithCleanupOwnerFromTryRunTest) {
  compileAndRunFile("tests/samples/test_try_catch_with_cleanup_owner_from_try.yz", 255);
}

TEST_F(TestFileSampleRunner, tryCatchAccessTryVariableDeathRunTest) {
  expectFailCompile("tests/samples/test_try_catch_access_try_variable.yz",
                    1,
                    "Error: Undeclared variable 'a'");
}

TEST_F(TestFileSampleRunner, tryCatchNestedDeathRunTest) {
  expectFailCompile("tests/samples/test_try_catch_nested.yz",
                    1,
                    "Error: Nested try blocks are not allowed. "
                    "Extract inner try/catch into a method.");
}
