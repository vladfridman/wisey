//
//  TestCWorkerThread.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/12/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.threads.CWorkerThread}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, threadExpressionNotNullRunTest) {
  runFile("tests/samples/test_thread_expression_not_null.yz", 1);
}

TEST_F(TestFileRunner, threadExpressionTypeRunTest) {
  runFile("tests/samples/test_thread_expression_type.yz", 1);
}

TEST_F(TestFileRunner, threadRunRunTest) {
  runFileCheckOutput("tests/samples/test_thread_run.yz",
                     "Thread started = 0, has finished = 0\n"
                     "Starting worker thread\n"
                     "Worker thread started\n"
                     "Thread started = 1, has finished = 0\n"
                     "This is worker thread!\n"
                     "Worker thread finished with result: 5\n"
                     "Thread started = 1, has finished = 1\n",
                     "");
}

TEST_F(TestFileRunner, threadResetRunTest) {
  runFileCheckOutput("tests/samples/test_thread_reset.yz",
                     "Thread started = 0, has finished = 0\n"
                     "Starting worker thread\n"
                     "Worker thread started\n"
                     "Thread started = 1, has finished = 0\n"
                     "This is worker thread!\n"
                     "Worker thread finished with result: 5\n"
                     "Thread started = 1, has finished = 1\n"
                     "Thread started = 0, has finished = 0\n"
                     "Starting worker thread\n"
                     "Worker thread started\n"
                     "Thread started = 1, has finished = 0\n"
                     "This is worker thread!\n"
                     "Worker thread finished with result: 5\n"
                     "Thread started = 1, has finished = 1\n",
                     "");
}

TEST_F(TestFileRunner, threadCancelRunTest) {
  runFileCheckOutput("tests/samples/test_thread_cancel.yz",
                     "Thread started = 0, has result = 0, was cancelled = 0\n"
                     "Starting worker thread\n"
                     "Worker thread started\n"
                     "Thread started = 1, has result = 0, was cancelled = 0\n"
                     "Worker thread cancelled\n"
                     "Thread started = 1, has result = 0, was cancelled = 1\n"
                     "Thread result is null\n",
                     "");
}

TEST_F(TestFileRunner, threadThrowConcealedCallExceptionDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_thread_throw_concealed_call_exception.yz",
                               1,
                               "",
                               "Unhandled exception wisey.threads.MThreadBoundaryException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_thread_throw_concealed_call_exception.yz:20)\n"
                               "Details: method internal to the thread is called from outside the thread\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, threadThrowRevealedCallExceptionDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_thread_throw_revealed_call_exception.yz",
                               0,
                               "",
                               "Unhandled exception wisey.threads.MThreadBoundaryException\n"
                               "  at systems.vos.wisey.compiler.tests.MWorker.work(tests/samples/test_thread_throw_revealed_call_exception.yz:10)\n"
                               "Details: method external to the thread is called from inside the thread\n");
}
