//
//  TestMLongToIntMap.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.data.MLongToIntMap}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, mLongToIntMapRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_mlong_to_int_map.yz",
                                        "Hashmap size: 3\n"
                                        "Lookup 1: 2\n"
                                        "Lookup 2: 4\n"
                                        "Lookup 3: 6\n"
                                        "Element with index 1 present\n"
                                        "Element with index 5 is not present\n"
                                        "mMap is null\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "freeing systems.vos.wisey.compiler.tests.CProgram.mMap\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
