//
//  TestThread.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests CThread wisey controller
//

#include "TestFileSampleRunner.hpp"

using namespace std;

TEST_F(TestFileSampleRunner, stackTraceRunTest) {
  string expected =
  "Stack trace:\n"
  "function1\n"
  "function2\n"
  "function3\n"
  "function4\n"
  "function5\n";
  
  runFileCheckOutput("tests/samples/test_stack_trace.yz",
                     expected.c_str(),
                     "");
}
