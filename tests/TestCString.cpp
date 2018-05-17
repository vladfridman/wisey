//
//  TestCString.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/10/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.lang.CString}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, cstringCreateFromStringRunTest) {
  runFile("tests/samples/test_cstring_create_from_string.yz", "3");
}

TEST_F(TestFileRunner, cstringCopyRunTest) {
  runFile("tests/samples/test_cstring_copy.yz", "3");
}

TEST_F(TestFileRunner, cstringCompareRunTest) {
  runFile("tests/samples/test_cstring_compare_to_string.yz", "0");
}

TEST_F(TestFileRunner, cstringAppendStringRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_append_string.yz",
                     "Before cstring: 123456 capacity: 10\n"
                     "After cstring: 1234567 capacity: 10\n",
                     "");
}

TEST_F(TestFileRunner, cstringAppendStringLongRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_append_string_long.yz",
                     "Before cstring: 123 capacity: 6\n"
                     "After cstring: 1234567 capacity: 12\n",
                     "");
}

TEST_F(TestFileRunner, cstringAppendCStringRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_append_cstring.yz",
                     "Before cstring: 123 capacity: 6\n"
                     "After cstring: 1234567 capacity: 12\n",
                     "");
}

TEST_F(TestFileRunner, cstringCreateFromBooleanRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_create_from_boolean.yz",
                     "Result: true\n",
                     "");
}

TEST_F(TestFileRunner, cstringCreateFromCharRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_create_from_char.yz",
                     "Result: A\n",
                     "");
}

TEST_F(TestFileRunner, cstringCreateFromIntRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_create_from_int.yz",
                     "Result: 12345\n",
                     "");
}

TEST_F(TestFileRunner, cstringCreateFromLongRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_create_from_long.yz",
                     "Result: 1234567\n",
                     "");
}

TEST_F(TestFileRunner, cstringAppendCharArrayLongRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_append_char_array.yz",
                     "Before cstring: 123 capacity: 6\n"
                     "After cstring: 1234567 capacity: 12\n",
                     "");
}

TEST_F(TestFileRunner, cstringAppendImmutableCharArrayLongRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_append_immutable_char_array.yz",
                     "Before cstring: 123 capacity: 6\n"
                     "After cstring: 1234567 capacity: 12\n",
                     "");
}

TEST_F(TestFileRunner, cstringToModelRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_to_model.yz",
                     "before: 123 length: 3 capacity: 6\n"
                     "after: 123 length: 3\n",
                     "");
}

TEST_F(TestFileRunner, cstringCreateFromModelRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_create_from_model.yz",
                     "mstring: 123 length: 3\n"
                     "cstring: 123 length: 3 capacity: 6\n",
                     "");
}

TEST_F(TestFileRunner, cstringCreateFromCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_create_from_char_array.yz",
                     "cstring: abc length: 3 capacity: 6\n",
                     "");
}

TEST_F(TestFileRunner, cstringCreateFromImmutableCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_create_from_immutable_char_array.yz",
                     "cstring: def length: 3 capacity: 6\n",
                     "");
}

TEST_F(TestFileRunner, cstringCreateFromEmptyCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_create_from_empty_char_array.yz",
                     "cstring:  length: 0 capacity: 1\n",
                     "");
}

TEST_F(TestFileRunner, cstringCreateFromUnterminatedCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_create_from_unterminated_char_array.yz",
                     "cstring: a length: 1 capacity: 3\n",
                     "");
}
