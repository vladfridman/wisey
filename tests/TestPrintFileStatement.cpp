//
//  TestPrintFileStatement.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link PrintErrStatement}
//

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, prinFileRunTest) {
  runFile("tests/samples/test_printfile.yz", "1");
  checkOutput("build/test.txt", "This is a test\n");
}
