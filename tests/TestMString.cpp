//
//  TestMString.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/10/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.io.MString}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, mstringCreateFromCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_mstring_create_from_char_array.yz",
                     "mstring: abc length: 3\n",
                     "");
}

TEST_F(TestFileRunner, mstringCreateFromImmutableCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_mstring_create_from_immutable_char_array.yz",
                     "mstring: def length: 3\n",
                     "");
}

TEST_F(TestFileRunner, mstringCreateFromEmptyCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_mstring_create_from_empty_char_array.yz",
                     "mstring:  length: 0\n",
                     "");
}

TEST_F(TestFileRunner, mstringCreateFromUnterminatedCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_mstring_create_from_unterminated_char_array.yz",
                     "mstring: a length: 1\n",
                     "");
}

TEST_F(TestFileRunner, mstringCopyRunTest) {
  runFile("tests/samples/test_mstring_copy.yz", "7");
}
