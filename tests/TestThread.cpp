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
  "object1.method1(FileA:123)\n"
  "object2.method2(FileB:57)\n"
  "Stack trace:\n"
  "object1.method1(FileA:123)\n"
  "Stack trace:\n"
  "Stack trace:\n"
  "Stack trace:\n"
  "object3.method3(FileC:99)\n";

  runFileCheckOutput("tests/samples/test_stack_trace.yz",
                     expected.c_str(),
                     "");
}
