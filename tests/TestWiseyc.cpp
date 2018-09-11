//
//  TestWiseyc.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/11/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests wiseyc.cpp
//

#include <gtest/gtest.h>

#include <array>
#include <fstream>
#include <iostream>

#include "TestFileRunner.hpp"

using namespace std;

struct WiseyTest : public ::testing::Test {
  
  WiseyTest() {
    system("mkdir -p build");
  }
  
  ~WiseyTest() { }
};

TEST_F(WiseyTest, noArgumentsTest) {
  EXPECT_STREQ("Syntax: wiseyc "
               "[-d|--destructor-debug] "
               "[-e|--emit-llvm] "
               "[-h|--help] "
               "[-v|--verbouse] "
               "[-H|--headers <header_file.yz>] "
               "[-o|--output <runnable_file_name>] "
               "[-n|--no-output] "
               "[--no-optimization] "
               "[-L<path_to_library>] "
               "[-l<library_name_to_link>] "
               "<source_file.yz>...\n",
               TestFileRunner::exec("bin/wiseyc 2>&1").c_str());
}

TEST_F(WiseyTest, runTest) {
  system("bin/wiseyc tests/samples/test_addition.yz -o build/test 2>&1");
  int result = system("build/test");
  int returnValue = WEXITSTATUS(result);
  
  EXPECT_EQ(returnValue, 7);
}
