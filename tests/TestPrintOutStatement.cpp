//
//  TestPrintOutStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link PrintOutStatement}
//

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, printOutRunTest) {
  runFileCheckOutput("tests/samples/test_printout.yz", "Hello world!\n", "");
}

TEST_F(TestFileRunner, printOutCompositeStringRunTest) {
  runFileCheckOutput("tests/samples/test_printout_composite_string.yz", "Car year: 2018\n", "");
}

TEST_F(TestFileRunner, printOutCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_printout_char_array.yz", "abc\n", "");
}

TEST_F(TestFileRunner, printOutFloatRunTest) {
  runFileCheckOutput("tests/samples/test_printout_float.yz", "3.141593\n", "");
}

TEST_F(TestFileRunner, printOutDoubleRunTest) {
  runFileCheckOutput("tests/samples/test_printout_double.yz", "3.141593e+00\n", "");
}

TEST_F(TestFileRunner, unreachablePrintOutStatementRunDeathTest) {
  expectFailCompile("tests/samples/test_unreachable_printout_statement.yz",
                    1,
                    "tests/samples/test_unreachable_printout_statement.yz\\(9\\): Error: Statement unreachable");
}

TEST_F(TestFileRunner, printOutNonPrimitiveTypeRunDeathTest) {
  expectFailCompile("tests/samples/test_printout_non_primitive_type.yz",
                    1,
                    "tests/samples/test_printout_non_primitive_type.yz\\(18\\): "
                    "Error: Can not do operation '\\+' on non-primitive types");
}

TEST_F(TestFileRunner, printOutVoidTypeRunDeathTest) {
  expectFailCompile("tests/samples/test_printout_void_type.yz",
                    1,
                    "tests/samples/test_printout_void_type.yz\\(21\\): "
                    "Error: Argument in the printout statement is not of printable type");
}
