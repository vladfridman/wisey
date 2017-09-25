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
#include <fstream>
#include <iostream>

#include "TestFileSampleRunner.hpp"

using namespace std;

struct MainTest : public ::testing::Test {
  MainTest() {
    system("mkdir -p build");
  }
  
  ~MainTest() { }
};

TEST_F(MainTest, noArgumentsTest) {
  EXPECT_STREQ(TestFileSampleRunner::exec("bin/wiseyc 2>&1").c_str(),
               "Syntax: wiseyc [-e|--emit-llvm] [-h|--help] [-v|--verbouse] "
               "[-H|--headers <header_file.yzh>] [-o|--output <object_file.o>] "
               "[-n|--no-output] <source_file.yz>...\n");
}

TEST_F(MainTest, missingFileTest) {
  EXPECT_STREQ(TestFileSampleRunner::exec("bin/wiseyc -v "
                                          "tests/samples/missingFile.yz 2>&1").c_str(),
               "Info: Parsing file tests/samples/missingFile.yz\n"
               "Error: File tests/samples/missingFile.yz not found!\n");
}

TEST_F(MainTest, missingOutputFileTest) {
  EXPECT_STREQ(TestFileSampleRunner::exec("bin/wiseyc tests/samples/test_addition.yz -o "
                                          "2>&1").c_str(),
               "Error: You need to specify the output file name after \"-o\"\n");
  EXPECT_STREQ(TestFileSampleRunner::exec("bin/wiseyc tests/samples/test_addition.yz --output "
                                          "2>&1").c_str(),
               "Error: You need to specify the output file name after \"--output\"\n");
}

TEST_F(MainTest, helpTest) {
  EXPECT_STREQ(TestFileSampleRunner::exec("bin/wiseyc -h 2>&1").c_str(),
               "Syntax: wiseyc [-e|--emit-llvm] [-h|--help] [-v|--verbouse] "
               "[-H|--headers <header_file.yzh>] [-o|--output <object_file.o>] "
               "[-n|--no-output] <source_file.yz>...\n");
  EXPECT_STREQ(TestFileSampleRunner::exec("bin/wiseyc --help 2>&1").c_str(),
               "Syntax: wiseyc [-e|--emit-llvm] [-h|--help] [-v|--verbouse] "
               "[-H|--headers <header_file.yzh>] [-o|--output <object_file.o>] "
               "[-n|--no-output] <source_file.yz>...\n");
}

TEST_F(MainTest, outputToFileTest) {
  system("mkdir -p build");

  system("bin/wiseyc tests/samples/test_addition.yz -o build/test.o 2>&1");
  system("g++ -o build/test build/test.o");
  int result = system("build/test");
  int returnValue = WEXITSTATUS(result);
  
  EXPECT_EQ(returnValue, 7);
}

TEST_F(MainTest, extractHeadersTest) {
  system("mkdir -p build");
  system("bin/wiseyc tests/samples/test_addition.yz -H build/test.yzh --no-output 2>&1");
  
  ifstream stream;
  stream.open("build/test.yzh");
  string output((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());
  
  EXPECT_STREQ("\\* Interfaces *\\\n"
               "\n"
               "interface wisey.lang.IProgram {\n"
               "  int run();\n"
               "}\n"
               "\n"
               "\\* Models *\\\n"
               "\n"
               "external model wisey.lang.MNullPointerException {\n"
               "}\n"
               "\n"
               "\\* Controllers *\\\n"
               "\n"
               "external controller systems.vos.wisey.compiler.tests.CProgram\n"
               "  implements\n"
               "    wisey.lang.IProgram {\n"
               "\n"
              "  int run();\n"
               "}\n"
               "\n"
               "\\* Nodes *\\\n"
               "\n"
               "\\* Bindings *\\\n"
               "\n"
               "bind(systems.vos.wisey.compiler.tests.CProgram).to(wisey.lang.IProgram);\n"
               "\n",
               output.c_str());
}

TEST_F(MainTest, emitLLVMTest) {
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
