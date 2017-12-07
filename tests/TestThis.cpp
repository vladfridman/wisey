//
//  TestThis.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/7/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests this variable
//

#include "TestFileSampleRunner.hpp"

TEST_F(TestFileSampleRunner, compareToThisObjectFalseRunTest) {
  runFile("tests/samples/test_compare_to_this_object_false.yz", "0");
}

TEST_F(TestFileSampleRunner, compareToThisObjectTrueRunTest) {
  runFile("tests/samples/test_compare_to_this_object_true.yz", "1");
}

TEST_F(TestFileSampleRunner, compareToThisInterfaceFalseRunTest) {
  runFile("tests/samples/test_compare_to_this_interface_false.yz", "0");
}

TEST_F(TestFileSampleRunner, compareToThisInterfaceTrueRunTest) {
  runFile("tests/samples/test_compare_to_this_interface_true.yz", "1");
}

TEST_F(TestFileSampleRunner, thisAsObjectArgumentRunTest) {
  runFile("tests/samples/test_this_as_object_argument.yz", "3");
}

TEST_F(TestFileSampleRunner, thisAsInterfaceArgumentRunTest) {
  runFile("tests/samples/test_this_as_interface_argument.yz", "5");
}
