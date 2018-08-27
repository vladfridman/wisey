//
//  TestPrintStatement.cpp
//  runtests
//
//  Created by Vladimir Fridman on 8/27/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link PrintStatement}
//

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, printStreamRunTest) {
  runFile("tests/samples/test_printstream.yz", 1);
  checkOutput("build/test.txt", "This is a test\n");
}

TEST_F(TestFileRunner, unreachablePrintStatementRunDeathTest) {
  expectFailCompile("tests/samples/test_unreachable_print_statement.yz",
                    1,
                    "tests/samples/test_unreachable_print_statement.yz\\(9\\): Error: Statement unreachable");
}

TEST_F(TestFileRunner, printStreamCatchFileOpenExceptionRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_printstream_catch_file_open_exception.yz",
                               5,
                               "",
                               "");
}

TEST_F(TestFileRunner, streamOpenFailDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_printstream_open_fail.yz",
                               1,
                               "",
                               "Unhandled exception wisey.io.MFileCouldNotOpenException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_printstream_open_fail.yz:10)\n"
                               "Details: Could not open file nonexisting/test.txt\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, streamWriterDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_printstream_write_exception.yz",
                               1,
                               "",
                               "Unhandled exception wisey.io.MStreamClosedException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_printstream_write_exception.yz:10)\n"
                               "Details: Stream is not opened and can not be used\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, streamCloseExceptionDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_printstream_close_exception.yz",
                               1,
                               "",
                               "Unhandled exception wisey.io.MStreamClosedException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_printstream_close_exception.yz:10)\n"
                               "Details: Stream is not opened and can not be used\n"
                               "Main thread ended without a result\n");
}
