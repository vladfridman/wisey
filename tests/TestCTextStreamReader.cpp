//
//  TestCTextStreamReader.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/10/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.io.CTextStreamReader}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, cTextStreamFileReaderRunTest) {
  runFileCheckOutput("tests/samples/test_ctext_stream_file_reader.yz", "one\ntwo\nthree\n", "");
}

TEST_F(TestFileRunner, cTextStreamSystemInReaderRunTest) {
  compileAndRunFileWithInputCheckOutput("tests/samples/test_ctext_stream_systemin_reader.yz",
                                        1,
                                        "tests/samples/testinput.txt",
                                        "line: this"
                                        "line: is"
                                        "line: a"
                                        "line: test",
                                        "");
}
