//
//  TestStlMap.cpp
//  runtests
//
//  Created by Vladimir Fridman on 7/30/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests functions for working with STL hash map
//

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, stlMapOfOwnersRunTest) {
  compileAndRunFileWithDestructorDebugCheckOutput("tests/samples/test_stl_map_of_owners.yz",
                                                  1,
                                                  "car year = 2018\n"
                                                  "car year = 2017\n"
                                                  "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                                  "done destructing systems.vos.wisey.compiler.tests.MCar\n"
                                                  "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                                  "done destructing systems.vos.wisey.compiler.tests.MCar\n"
                                                  "done destroying\n"
                                                  "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                                  "done destructing systems.vos.wisey.compiler.tests.CProgram\n",
                                                  "");
}
