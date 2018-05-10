//
//  TestCFileReader.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/10/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.io.CFileReader}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, cfileReaderRunTest) {
  runFileCheckOutput("tests/samples/test_cfile_reader.yz", "one\ntwo\nthree\n", "");
}
