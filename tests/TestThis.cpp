//
//  TestThis.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/7/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests this variable
//

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, compareToThisObjectFalseRunTest) {
  runFile("tests/samples/test_compare_to_this_object_false.yz", 0);
}

TEST_F(TestFileRunner, compareToThisObjectTrueRunTest) {
  runFile("tests/samples/test_compare_to_this_object_true.yz", 1);
}

TEST_F(TestFileRunner, compareToThisInterfaceFalseRunTest) {
  runFile("tests/samples/test_compare_to_this_interface_false.yz", 0);
}

TEST_F(TestFileRunner, compareToThisInterfaceTrueRunTest) {
  runFile("tests/samples/test_compare_to_this_interface_true.yz", 1);
}

TEST_F(TestFileRunner, thisAsObjectArgumentRunTest) {
  runFile("tests/samples/test_this_as_object_argument.yz", 3);
}

TEST_F(TestFileRunner, thisAsInterfaceArgumentRunTest) {
  runFile("tests/samples/test_this_as_interface_argument.yz", 5);
}
