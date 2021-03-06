//
//  TestCMemoryPool.cpp
//  runtests
//
//  Created by Vladimir Fridman on 6/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.lang.CMemoryPool}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, pooledNodeDestructorDebugRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_pooled_node.yz",
                                        "destructor node object systems.vos.wisey.compiler.tests.NCar\n"
                                        "done destructing pool object systems.vos.wisey.compiler.tests.NCar\n"
                                        "destructor node object systems.vos.wisey.compiler.tests.NCar\n"
                                        "done destructing pool object systems.vos.wisey.compiler.tests.NCar\n"
                                        "destructor node object systems.vos.wisey.compiler.tests.NCar\n"
                                        "done destructing pool object systems.vos.wisey.compiler.tests.NCar\n"
                                        "destructor node object systems.vos.wisey.compiler.tests.NCar\n"
                                        "done destructing pool object systems.vos.wisey.compiler.tests.NCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "freeing systems.vos.wisey.compiler.tests.CProgram.mPool\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileRunner, pooledNodeRunTest) {
  runFile("tests/samples/test_pooled_node.yz", 2017);
}

TEST_F(TestFileRunner, destructPoolBeforeObjectsRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_destruct_pool_before_objects.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MMemoryPoolNonEmpty\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_destruct_pool_before_objects.yz:20)\n"
                               "Details: Attempting to destroy a memory pool that still contains 1 allocated object\n"
                               "Main thread ended without a result\n");
}
