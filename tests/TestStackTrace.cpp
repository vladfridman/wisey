//
//  TestStackTrace.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests reporting stack trace by TMainThread
//

#include "TestFileRunner.hpp"

using namespace std;

TEST_F(TestFileRunner, stackTraceRunTest) {
  string expected =
  "Stack trace 1:\n"
  "systems.vos.wisey.compiler.tests.MEngine.getVolume(tests/samples/test_stack_trace.yz:10)\n"
  "systems.vos.wisey.compiler.tests.MCar.getEngineVolume(tests/samples/test_stack_trace.yz:19)\n"
  "systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_stack_trace.yz:27)\n"
  "Stack trace 2:\n"
  "systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_stack_trace.yz:29)\n";

  runFileCheckOutput("tests/samples/test_stack_trace.yz",
                     expected.c_str(),
                     "");
}

TEST_F(TestFileRunner, stackTraceMultipleFilesRunTest) {
  string expected =
  "systems.vos.wisey.compiler.tests.multifile.MAdder.add(tests/samples/test_multifile_stack_trace/MAdder.yz:5)\n"
  "systems.vos.wisey.compiler.tests.multifile.CProgram.run(tests/samples/test_multifile_stack_trace/CProgram.yz:10)\n";
  
  vector<string> files;
  files.push_back("tests/samples/test_multifile_stack_trace/CProgram.yz");
  files.push_back("tests/samples/test_multifile_stack_trace/MAdder.yz");
  runFilesCheckOutput(files, expected.c_str(), "");
}

TEST_F(TestFileRunner, stackOverflowRunDeathTest) {
  string expected = "Unhandled exception wisey.lang.MStackOverflowException\n";
  for (int i = 0; i < 1023; i++) {
    expected += "  at systems.vos.wisey.compiler.tests.CProgram.callRecursive(tests/samples/test_stack_overflow.yz:9)\n";
  }
  expected += "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_stack_overflow.yz:14)\n";
  expected += "Main thread ended without a result\n";
  
  compileAndRunFileCheckOutput("tests/samples/test_stack_overflow.yz", 1, "", expected.c_str());
}
