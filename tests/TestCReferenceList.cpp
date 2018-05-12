//
//  TestCReferenceList.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/11/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.data.CReferenceList}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, cReferenceListRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_creference_list.yz",
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
