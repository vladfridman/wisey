//
//  TestCLongToIntMap.cpp
//  runtests
//
//  Created by Vladimir Fridman on 8/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.data.CLongToIntMap}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, cLongToIntMapRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_clong_to_int_map.yz",
                                        "Hashmap size: 3\n"
                                        "Lookup 1: 2\n"
                                        "Lookup 2: 4\n"
                                        "Lookup 3: 6\n"
                                        "1 was erased\n"
                                        "2 was erased\n"
                                        "3 was erased\n"
                                        "Hashmap size: 0\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "freeing systems.vos.wisey.compiler.tests.CProgram.mMap\n"
                                        "done destructing systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
