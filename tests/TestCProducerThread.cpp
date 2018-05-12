//
//  TestCProducerThread.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/12/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.threads.CProducerThread}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, threadProducerRunRunTest) {
  runFileCheckOutput("tests/samples/test_thread_producer_run.yz",
                     "Thread started = 0, has result = 0\n"
                     "Starting producer thread\n"
                     "Producer thread started\n"
                     "Thread started = 1, has result = 0\n"
                     "This is producer thread!\n"
                     "Adding result 0\n"
                     "Adding result 1\n"
                     "Producer thread finished with result: 0\n"
                     "Thread started = 1, has result = 1\n"
                     "Producer thread finished with result: 1\n"
                     "Thread started = 1, has result = 0\n",
                     "");
}

TEST_F(TestFileRunner, threadProducerResetRunTest) {
  runFileCheckOutput("tests/samples/test_thread_producer_reset.yz",
                     "Thread started = 0, has result = 0\n"
                     "Starting producer thread\n"
                     "Producer thread started\n"
                     "Thread started = 1, has result = 0\n"
                     "This is producer thread!\n"
                     "Adding result 0\n"
                     "Adding result 1\n"
                     "Producer thread finished with result: 0\n"
                     "Thread started = 1, has result = 1\n"
                     "Producer thread finished with result: 1\n"
                     "Thread started = 1, has result = 0\n"
                     "Thread started = 0, has result = 0\n"
                     "Starting producer thread\n"
                     "Producer thread started\n"
                     "Thread started = 1, has result = 0\n"
                     "This is producer thread!\n"
                     "Adding result 0\n"
                     "Adding result 1\n"
                     "Producer thread finished with result: 0\n"
                     "Thread started = 1, has result = 1\n"
                     "Producer thread finished with result: 1\n"
                     "Thread started = 1, has result = 0\n",
                     "");
}
