//
//  TestProgramSuffix.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ProgramSuffix}
//

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, iprogramNotBoundRunDeathTest) {
  expectFailCompile("tests/samples/test_iprogram_not_bound.yz",
                    1,
                    "wisey.lang.IProgram is not bound to a controller, executable can not be generated");
}
