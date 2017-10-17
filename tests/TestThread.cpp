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
  "Stack trace 1:\n"
  "wisey.lang.CThread.dumpStack(CThread.yz:109)\n"
  "systems.vos.wisey.compiler.tests.MEngine.getVolume(:0)\n"
  "systems.vos.wisey.compiler.tests.MCar.getEngineVolume(:0)\n"
  "systems.vos.wisey.compiler.tests.CProgram.run(:0)\n"
  ".(:0)\n"
  "Stack trace 2:\n"
  "wisey.lang.CThread.dumpStack(CThread.yz:109)\n"
  "systems.vos.wisey.compiler.tests.CProgram.run(:0)\n"
  ".(:0)\n";

  runFileCheckOutput("tests/samples/test_stack_trace.yz",
                     expected.c_str(),
                     "");
}
