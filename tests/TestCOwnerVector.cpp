//
//  TestCOwnerVector.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.data.COwnerVector}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, cOwnerVectorRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_cownervector.yz",
                                        "Car year: 2018\n"
                                        "Car year: 2017\n"
                                        "Car year: 2016\n"
                                        "Car year: 2015\n"
                                        "Car year: 2015\n"
                                        "Car year: 2016\n"
                                        "Car year: 2017\n"
                                        "Car year: 2018\n"
                                        "Car year: 2018\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MCar\n"
                                        "Vector size: 3\n"
                                        "Car year: 2015\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "freeing systems.vos.wisey.compiler.tests.CProgram.mVector\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
