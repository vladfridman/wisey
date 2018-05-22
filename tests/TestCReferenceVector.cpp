//
//  TestCReferenceVector.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/23/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.data.CReferenceVector}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, cReferenceVectorRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_creferencevector.yz",
                                        "Vector size: 0, capacity: 3\n"
                                        "Vector size: 1, capacity: 3\n"
                                        "Vector size: 2, capacity: 3\n"
                                        "Vector size: 3, capacity: 3\n"
                                        "Vector size: 4, capacity: 6\n"
                                        "Car year: 2015\n"
                                        "Vector size: 3, capacity: 6\n"
                                        "Car year: 2018\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
