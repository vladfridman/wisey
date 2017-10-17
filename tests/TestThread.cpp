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
  "systems.vos.wisey.compiler.tests.MEngine.getVolume(tests/samples/test_stack_trace.yz:10)\n"
  "systems.vos.wisey.compiler.tests.MCar.getEngineVolume(tests/samples/test_stack_trace.yz:19)\n"
  "systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_stack_trace.yz:27)\n"
  "wisey.lang.CProgramRunner.run(wisey/lang/CProgramRunner.yz:13)\n"
  "Stack trace 2:\n"
  "systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_stack_trace.yz:29)\n"
  "wisey.lang.CProgramRunner.run(wisey/lang/CProgramRunner.yz:13)\n";

  runFileCheckOutput("tests/samples/test_stack_trace.yz",
                     expected.c_str(),
                     "");
}

TEST_F(TestFileSampleRunner, stackTraceMultipleFilesRunTest) {
  string expected =
  "systems.vos.wisey.compiler.tests.multifile.MAdder.add(tests/samples/test_multifile_stack_trace/MAdder.yz:5)\n"
  "systems.vos.wisey.compiler.tests.multifile.CProgram.run(tests/samples/test_multifile_stack_trace/CProgram.yz:10)\n"
  "wisey.lang.CProgramRunner.run(wisey/lang/CProgramRunner.yz:13)\n";
  
  vector<string> files;
  files.push_back("tests/samples/test_multifile_stack_trace/CProgram.yz");
  files.push_back("tests/samples/test_multifile_stack_trace/MAdder.yz");
  runFilesCheckOutput(files, expected.c_str(), "");
}
