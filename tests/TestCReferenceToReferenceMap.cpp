//
//  TestCReferenceToReferenceMap.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/12/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.data.CReferenceToReferenceMap}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, cReferenceToReferenceMapRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_creference_to_reference_map.yz",
                                        "Hashmap size: 3\n"
                                        "Lookup Toyota: Toyota\n"
                                        "Lookup BMW: BMW\n"
                                        "Lookup Mercedes: Mercedes\n"
                                        "BMW was erased\n"
                                        "Toyota was erased\n"
                                        "Mercedes was erased\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "done destructing systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
