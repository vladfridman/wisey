//
//  TestCContextManager.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/12/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.threads.CContextManager}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, cContextManagerRunTest) {
  runFileCheckOutput("tests/samples/test_ccontext_manager.yz",
                     "In context context1 looking for Toyota got Toyota back\n"
                     "In context context1 looking for BMW got BMW back\n"
                     "In context context2 looking for Toyota got BMW back\n"
                     "In context context2 looking for BMW got Toyota back\n"
                     "In context context2 looking for BMW got BMW back\n",
                     "");
}

TEST_F(TestFileRunner, cContextManagerThrowsRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_ccontext_manager_throws.yz",
                               1,
                               "",
                               "Unhandled exception wisey.threads.MContextNonExistant\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_ccontext_manager_throws.yz:10)\n"
                               "Details: Context mycontext does not exist\n"
                               "Main thread ended without a result\n");
}
