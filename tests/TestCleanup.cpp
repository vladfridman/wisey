//
//  TestCleanup.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Cleanup}
//

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, throwRceFromCleanupRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_throw_rce_from_cleanup.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_throw_rce_from_cleanup.yz:17)\n"
                               "Details: Object referenced by expression of type systems.vos.wisey.compiler.tests.MCar still has 1 active reference\n"
                               "Was thrown from memory clean up after wisey.lang.MNullPointerException\n"
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_throw_rce_from_cleanup.yz:17)\n"
                               "Main thread ended without a result\n");
}
