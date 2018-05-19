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

TEST_F(TestFileRunner, throwRceFromCleanupCustomExceptionRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_throw_rce_from_cleanup_custom_exception.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CService.throwException(tests/samples/test_throw_rce_from_cleanup_custom_exception.yz:22)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_throw_rce_from_cleanup_custom_exception.yz:34)\n"
                               "Details: Object referenced by expression of type systems.vos.wisey.compiler.tests.MData still has 2 active references\n"
                               "Was thrown from memory clean up after systems.vos.wisey.compiler.tests.MMyException\n"
                               "Unhandled exception systems.vos.wisey.compiler.tests.MMyException\n"
                               "  at systems.vos.wisey.compiler.tests.MMyException.report(tests/samples/test_throw_rce_from_cleanup_custom_exception.yz:15)\n"
                               "  at systems.vos.wisey.compiler.tests.CService.throwException(tests/samples/test_throw_rce_from_cleanup_custom_exception.yz:22)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_throw_rce_from_cleanup_custom_exception.yz:34)\n"
                               "Main thread ended without a result\n");
}
