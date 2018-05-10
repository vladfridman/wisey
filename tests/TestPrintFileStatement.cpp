//
//  TestPrintFileStatement.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link PrintErrStatement}
//

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, printFileRunTest) {
  runFile("tests/samples/test_printfile.yz", "1");
  checkOutput("build/test.txt", "This is a test\n");
}

TEST_F(TestFileRunner, unreachablePrintFileStatementRunDeathTest) {
  expectFailCompile("tests/samples/test_unreachable_printfile_statement.yz",
                    1,
                    "tests/samples/test_unreachable_printfile_statement.yz\\(9\\): Error: Statement unreachable");
}

TEST_F(TestFileRunner, printFileCatchFileOpenExceptionRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_printfile_catch_file_open_exception.yz",
                               5,
                               "",
                               "");
}

TEST_F(TestFileRunner, fileOpenExceptionDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_printfile_open_exception.yz",
                               1,
                               "",
                               "Unhandled exception wisey.io.MFileOpenException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_printfile_open_exception.yz:12)\n"
                               "Details: Trying to open file build/foo.txt without closing build/test.txt\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, fileOpenFailDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_printfile_open_fail.yz",
                               1,
                               "",
                               "Unhandled exception wisey.io.MFileCouldNotOpenException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_printfile_open_fail.yz:11)\n"
                               "Details: Could not open file nonexisting/test.txt\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, fileWriterDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_printfile_write_exception.yz",
                               1,
                               "",
                               "Unhandled exception wisey.io.MFileAccessException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_printfile_write_exception.yz:10)\n"
                               "Details: Trying to operate a file that is not opened\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, fileCloseExceptionDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_printfile_close_exception.yz",
                               1,
                               "",
                               "Unhandled exception wisey.io.MFileCloseException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_printfile_close_exception.yz:10)\n"
                               "Details: Trying to close a file without opening it first\n"
                               "Main thread ended without a result\n");
}
