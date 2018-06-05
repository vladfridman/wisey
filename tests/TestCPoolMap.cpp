//
//  TestCPoolMap.cpp
//  runtests
//
//  Created by Vladimir Fridman on 6/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.lang.CPoolMap}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, pooledModelDestructorDebugRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_pooled_model.yz",
                                        "destructor pooled object systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing pooled object systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor pooled object systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing pooled object systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor pooled object systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing pooled object systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor pooled object systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing pooled object systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "done destructing systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileRunner, pooledModelRunTest) {
  runFile("tests/samples/test_pooled_model.yz", 2017);
}

TEST_F(TestFileRunner, pooledNodeDestructorDebugRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_pooled_node.yz",
                                        "destructor pooled object systems.vos.wisey.compiler.tests.NCar\n"
                                        "done destructing pooled object systems.vos.wisey.compiler.tests.NCar\n"
                                        "destructor pooled object systems.vos.wisey.compiler.tests.NCar\n"
                                        "done destructing pooled object systems.vos.wisey.compiler.tests.NCar\n"
                                        "destructor pooled object systems.vos.wisey.compiler.tests.NCar\n"
                                        "done destructing pooled object systems.vos.wisey.compiler.tests.NCar\n"
                                        "destructor pooled object systems.vos.wisey.compiler.tests.NCar\n"
                                        "done destructing pooled object systems.vos.wisey.compiler.tests.NCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "done destructing systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileRunner, pooledNodeRunTest) {
  runFile("tests/samples/test_pooled_node.yz", 2017);
}
