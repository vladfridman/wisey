//
//  TestNString.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/10/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.lang.NString}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, nstringCreateFromStringRunTest) {
  runFile("tests/samples/test_nstring_create_from_string.yz", 3);
}

TEST_F(TestFileRunner, nstringCopyRunTest) {
  runFile("tests/samples/test_nstring_copy.yz", 3);
}

TEST_F(TestFileRunner, nstringCompareRunTest) {
  runFile("tests/samples/test_nstring_compare_to_string.yz", 0);
}

TEST_F(TestFileRunner, nstringAppendStringRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_append_string.yz",
                     "Before nstring: 123456 capacity: 10\n"
                     "After nstring: 1234567 capacity: 10\n",
                     "");
}

TEST_F(TestFileRunner, nstringAppendStringLongRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_append_string_long.yz",
                     "Before nstring: 123 capacity: 6\n"
                     "After nstring: 1234567 capacity: 12\n",
                     "");
}

TEST_F(TestFileRunner, nstringAppendnstringRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_append_nstring.yz",
                     "Before nstring: 123 capacity: 6\n"
                     "After nstring: 1234567 capacity: 12\n",
                     "");
}

TEST_F(TestFileRunner, nstringCreateFromBooleanRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_create_from_boolean.yz",
                     "Result: true\n",
                     "");
}

TEST_F(TestFileRunner, nstringCreateFromCharRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_create_from_char.yz",
                     "Result: A\n",
                     "");
}

TEST_F(TestFileRunner, nstringCreateFromFloatRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_create_from_float.yz",
                     "Result: 3.141500\n",
                     "");
}

TEST_F(TestFileRunner, nstringCreateFromDoubleRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_create_from_double.yz",
                     "Result: 3.141593e+00\n",
                     "");
}

TEST_F(TestFileRunner, nstringCreateFromIntRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_create_from_int.yz",
                     "Result: 12345\n",
                     "");
}

TEST_F(TestFileRunner, nstringCreateFromLongRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_create_from_long.yz",
                     "Result: 1234567\n",
                     "");
}

TEST_F(TestFileRunner, nstringAppendCharArrayLongRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_append_char_array.yz",
                     "Before nstring: 123 capacity: 6\n"
                     "After nstring: 1234567 capacity: 12\n",
                     "");
}

TEST_F(TestFileRunner, nstringAppendImmutableCharArrayLongRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_append_immutable_char_array.yz",
                     "Before nstring: 123 capacity: 6\n"
                     "After nstring: 1234567 capacity: 12\n",
                     "");
}

TEST_F(TestFileRunner, nstringToModelRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_to_model.yz",
                     "before: 123 length: 3 capacity: 6\n"
                     "after: 123 length: 3\n",
                     "");
}

TEST_F(TestFileRunner, nstringCreateFromModelRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_create_from_model.yz",
                     "mstring: 123 length: 3\n"
                     "nstring: 123 length: 3 capacity: 6\n",
                     "");
}

TEST_F(TestFileRunner, nstringCreateFromCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_create_from_char_array.yz",
                     "nstring: abc length: 3 capacity: 6\n",
                     "");
}

TEST_F(TestFileRunner, nstringCreateFromImmutableCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_create_from_immutable_char_array.yz",
                     "nstring: def length: 3 capacity: 6\n",
                     "");
}

TEST_F(TestFileRunner, nstringCreateFromEmptyCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_create_from_empty_char_array.yz",
                     "nstring:  length: 0 capacity: 1\n",
                     "");
}

TEST_F(TestFileRunner, nstringCreateFromUnterminatedCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_create_from_unterminated_char_array.yz",
                     "nstring: a length: 1 capacity: 3\n",
                     "");
}

TEST_F(TestFileRunner, nstringToBooleanRunTest) {
  runFile("tests/samples/test_nstring_to_boolean.yz", 1);
}

TEST_F(TestFileRunner, nstringToCharRunTest) {
  runFile("tests/samples/test_nstring_to_char.yz", 77);
}

TEST_F(TestFileRunner, nstringToIntRunTest) {
  runFile("tests/samples/test_nstring_to_int.yz", 15);
}

TEST_F(TestFileRunner, nstringToLongRunTest) {
  runFile("tests/samples/test_nstring_to_long.yz", 1234567);
}

TEST_F(TestFileRunner, nstringStartsWithRunTest) {
  runFile("tests/samples/test_nstring_starts_with.yz", 1);
}

TEST_F(TestFileRunner, nstringCharAtRunTest) {
  runFile("tests/samples/test_nstring_charat.yz", 1);
}

TEST_F(TestFileRunner, nstringToFloatRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_to_float.yz",
                     "Result: 3.141590\n",
                     "");
}

TEST_F(TestFileRunner, nstringToDoubleRunTest) {
  runFileCheckOutput("tests/samples/test_nstring_to_double.yz",
                     "Result: 3.141590e+00\n",
                     "");
}

TEST_F(TestFileRunner, nstringToIntThrowRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_nstring_to_int_throw.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MCastException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_nstring_to_int_throw.yz:10)\n"
                               "Details: Can not cast from string to int\n"
                               "Main thread ended without a result\n");

}
