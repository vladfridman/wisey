//
//  TestPrintErrStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link PrintErrStatement}
//

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, printErrRunTest) {
  runFileCheckOutput("tests/samples/test_printerr.yz", "", "Hello world!\n");
}

TEST_F(TestFileRunner, printErrCompositeStringRunTest) {
  runFileCheckOutput("tests/samples/test_printerr_composite_string.yz", "", "Car year: 2018\n");
}

TEST_F(TestFileRunner, printErrNonPrimitiveTypeRunDeathTest) {
  expectFailCompile("tests/samples/test_printerr_non_primitive_type.yz",
                    1,
                    "Error: Can not print non primitive types");
}

TEST_F(TestFileRunner, printErrVoidTypeRunDeathTest) {
  expectFailCompile("tests/samples/test_printerr_void_type.yz",
                    1,
                    "Error: Can not print a void type expression");
}
