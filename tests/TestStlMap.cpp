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

TEST_F(TestFileRunner, stlMapRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_stl_map.yz",
                               1,
                               "destroy\n"
                               "destroy\n"
                               "car year = 2018\n",
                               "");
}
