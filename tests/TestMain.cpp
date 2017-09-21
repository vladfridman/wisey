//
//  TestMain.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests main.cpp

#include <gtest/gtest.h>

#include <array>
#include <iostream>

#include "TestFileSampleRunner.hpp"

using namespace std;

struct TestMain : public ::testing::Test {
  TestMain() {
    system("mkdir -p build");
  }
  
  ~TestMain() { }
};

TEST_F(TestMain, noArgumentsTest) {
  EXPECT_STREQ(TestFileSampleRunner::exec("bin/wiseyc 2>&1").c_str(),
               "Error: Syntax: wiseyc [-e|--emit-llvm] [-h|--help] [-v|--verbouse] "
               "[-o|--output <bitcode_file>] <sourcefile.yz>...\n");
}

TEST_F(TestMain, missingFileTest) {
  EXPECT_STREQ(TestFileSampleRunner::exec("bin/wiseyc -v "
                                          "tests/samples/missingFile.yz 2>&1").c_str(),
               "Info: Parsing file tests/samples/missingFile.yz\n"
               "Error: File tests/samples/missingFile.yz not found!\n");
}

TEST_F(TestMain, missingOutputFileTest) {
  EXPECT_STREQ(TestFileSampleRunner::exec("bin/wiseyc tests/samples/test_addition.yz -o "
                                          "2>&1").c_str(),
               "Error: You need to specify the output file name after \"-o\"\n");
  EXPECT_STREQ(TestFileSampleRunner::exec("bin/wiseyc tests/samples/test_addition.yz --output "
                                          "2>&1").c_str(),
               "Error: You need to specify the output file name after \"--output\"\n");
}

TEST_F(TestMain, helpTest) {
  EXPECT_STREQ(TestFileSampleRunner::exec("bin/wiseyc -h 2>&1").c_str(),
               "Error: Syntax: wiseyc [-e|--emit-llvm] [-h|--help] [-v|--verbouse] "
               "[-o|--output <bitcode_file>] <sourcefile.yz>...\n");
  EXPECT_STREQ(TestFileSampleRunner::exec("bin/wiseyc --help 2>&1").c_str(),
               "Error: Syntax: wiseyc [-e|--emit-llvm] [-h|--help] [-v|--verbouse] "
               "[-o|--output <bitcode_file>] <sourcefile.yz>...\n");
}

TEST_F(TestMain, outputToFileTest) {
  system("mkdir -p build");

  system("bin/wiseyc tests/samples/test_addition.yz -o build/test.o 2>&1");
  system("g++ -o build/test build/test.o");
  int result = system("build/test");
  int returnValue = WEXITSTATUS(result);
  
  EXPECT_EQ(returnValue, 7);
}

TEST_F(TestMain, emitLLVMTest) {
  system("mkdir -p build");

  string resultWithoutEmitLLVM =
  TestFileSampleRunner::exec("bin/wiseyc -o build/test.bc tests/samples/test_addition.yz");
  EXPECT_EQ(resultWithoutEmitLLVM.find("define i32 @main()"), string::npos);
  
  string resultWithEmitLLVM = TestFileSampleRunner::exec("bin/wiseyc --emit-llvm -o build/test.bc "
                                                         "tests/samples/test_addition.yz");
  EXPECT_NE(resultWithEmitLLVM.find("define i32 @main()"), string::npos);
  
  resultWithEmitLLVM = TestFileSampleRunner::exec("bin/wiseyc -e -o build/test.bc "
                                                  "tests/samples/test_addition.yz");
  EXPECT_NE(resultWithEmitLLVM.find("define i32 @main()"), string::npos);
}
