//
//  TestWiseylibc.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/11/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests wiseylibc.cpp
//

#include <gtest/gtest.h>

#include <array>
#include <fstream>
#include <iostream>

#include "TestFileRunner.hpp"

using namespace std;

struct WiseylibcTest : public ::testing::Test {
  
  WiseylibcTest() {
    system("mkdir -p build");
  }
  
  ~WiseylibcTest() { }
};

TEST_F(WiseylibcTest, noArgumentsTest) {
  EXPECT_STREQ("Syntax: wiseylibc "
               "[-d|--destructor-debug] "
               "[-e|--emit-llvm] "
               "[-h|--help] "
               "[-v|--verbouse] "
               "[-H|--headers <header_file.yz>] "
               "[-o|--output <library_file_name.so>] "
               "[-n|--no-output] "
               "[--no-optimization] "
               "[--target-triple <target_machine_triple>] "
               "[-A<additional_object_to_link>] "
               "<source_file.yz>...\n",
               TestFileRunner::exec("bin/wiseylibc 2>&1").c_str());
}
