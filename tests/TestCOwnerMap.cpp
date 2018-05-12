//
//  TestCOwnerMap.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/12/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.io.COwnerMap}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, cOwnerMapRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_cowner_map.yz",
                                        "bmw is null\n"
                                        "Lookup Toyota: Toyota\n"
                                        "Lookup BMW: BMW\n"
                                        "Lookup Mercedes: Mercedes\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "BMW was erased\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "Toyota was erased\n"
                                        "Mercedes was erased\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
