//
//  TestCInputReader.cpp
//  runtests
//
//  Created by Vladimir Fridman on 8/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.io.CInputReader}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, cInputReaderRunTest) {
  compileAndRunFileWithInputCheckOutput("tests/samples/test_cinput_reader.yz",
                                        1,
                                        "tests/samples/testinput.txt",
                                        "line: this"
                                        "line: is"
                                        "line: a"
                                        "line: test",
                                        "");
}
