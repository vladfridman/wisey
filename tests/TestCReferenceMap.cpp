//
//  TestCReferenceMap.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/12/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.io.CReferenceMap}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, cReferenceMapRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_creference_map.yz",
                                        "Lookup Toyota: Toyota\n"
                                        "Lookup BMW: BMW\n"
                                        "Lookup Mercedes: Mercedes\n"
                                        "BMW was erased\n"
                                        "Toyota was erased\n"
                                        "Mercedes was erased\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
