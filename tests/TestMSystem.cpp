//
//  TestMSystem.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.lang.MSystem}
//

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, msystemGetOriginalOBjectTypeNameRunTest) {
  runFileCheckOutput("tests/samples/test_msystem_get_original_object_type_name.yz",
                     "Object: systems.vos.wisey.compiler.tests.CProgram\n",
                     "");
}
