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

TEST_F(TestFileRunner, printOutNonPrimitiveTypeRunDeathTest) {
  expectFailCompile("tests/samples/test_printout_non_primitive_type.yz",
                    1,
                    "Error: Can not print non primitive types");
}

TEST_F(TestFileRunner, printOutVoidTypeRunDeathTest) {
  expectFailCompile("tests/samples/test_printout_void_type.yz",
                    1,
                    "Error: Can not print a void type expression");
}
