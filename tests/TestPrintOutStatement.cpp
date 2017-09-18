//
//  TestPrintOutStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link PrintOutStatement}
//

#include "TestFileSampleRunner.hpp"

TEST_F(TestFileSampleRunner, printoutRunTest) {
  runFileCheckOutput("tests/samples/test_printout.yz", "Hello world!\n", "");
}

TEST_F(TestFileSampleRunner, printoutCompositeStringRunTest) {
  runFileCheckOutput("tests/samples/test_printout_composite_string.yz", "Car year: 2018\n", "");
}
