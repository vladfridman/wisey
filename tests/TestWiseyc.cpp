//
//  TestMain.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests wiseyc.cpp
//

#include <gtest/gtest.h>

#include <array>
#include <fstream>
#include <iostream>

#include "TestFileRunner.hpp"

using namespace std;

struct MainTest : public ::testing::Test {

  MainTest() {
    system("mkdir -p build");
  }
  
  ~MainTest() { }
};

TEST_F(MainTest, noArgumentsTest) {
  EXPECT_STREQ(TestFileRunner::exec("bin/wiseyc 2>&1").c_str(),
               "Syntax: wiseyc "
               "[-d|--destructor-debug] "
               "[-e|--emit-llvm] "
               "[-h|--help] "
               "[-v|--verbouse] "
               "[-H|--headers <header_file.yzh>] "
               "[-o|--output <object_file.o>] "
               "[-n|--no-output] "
               "[--no-optimization] "
               "<source_file.yz>...\n");
}

TEST_F(MainTest, missingFileTest) {
  EXPECT_STREQ(TestFileRunner::exec("bin/wiseyc -v "
                                          "tests/samples/missingFile.yz 2>&1").c_str(),
               "Info: Parsing file tests/samples/missingFile.yz\n"
               "Error: File tests/samples/missingFile.yz not found!\n");
}

TEST_F(MainTest, missingOutputFileTest) {
  EXPECT_STREQ(TestFileRunner::exec("bin/wiseyc tests/samples/test_addition.yz -o "
                                          "2>&1").c_str(),
               "Error: You need to specify the output file name after \"-o\"\n");
  EXPECT_STREQ(TestFileRunner::exec("bin/wiseyc tests/samples/test_addition.yz --output "
                                          "2>&1").c_str(),
               "Error: You need to specify the output file name after \"--output\"\n");
}

TEST_F(MainTest, helpTest) {
  EXPECT_STREQ(TestFileRunner::exec("bin/wiseyc -h 2>&1").c_str(),
               "Syntax: wiseyc "
               "[-d|--destructor-debug] "
               "[-e|--emit-llvm] "
               "[-h|--help] "
               "[-v|--verbouse] "
               "[-H|--headers <header_file.yzh>] "
               "[-o|--output <object_file.o>] "
               "[-n|--no-output] "
               "[--no-optimization] "
               "<source_file.yz>...\n");
  EXPECT_STREQ(TestFileRunner::exec("bin/wiseyc --help 2>&1").c_str(),
               "Syntax: wiseyc "
               "[-d|--destructor-debug] "
               "[-e|--emit-llvm] "
               "[-h|--help] "
               "[-v|--verbouse] "
               "[-H|--headers <header_file.yzh>] "
               "[-o|--output <object_file.o>] "
               "[-n|--no-output] "
               "[--no-optimization] "
               "<source_file.yz>...\n");
}

TEST_F(MainTest, outputToFileTest) {
  system("mkdir -p build");

  system("bin/wiseyc tests/samples/test_addition.yz libwisey/libwisey.yz -o build/test.o 2>&1");
  system("g++ -o build/test build/test.o -Llibwisey -lwisey");
  int result = system("build/test");
  int returnValue = WEXITSTATUS(result);
  
  EXPECT_EQ(returnValue, 7);
}

TEST_F(MainTest, extractHeadersTest) {
  system("mkdir -p build");
  system("bin/wiseyc tests/samples/test_addition.yz libwisey/libwisey.yz "
         "-H build/test.yzh --no-output 2>&1");
  
  ifstream stream;
  stream.open("build/test.yzh");
  string output((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());
  
  EXPECT_STREQ("/* Interfaces */\n"
               "\n"
               "/* Models */\n"
               "\n"
               "/* Controllers */\n"
               "\n"
               "external controller systems.vos.wisey.compiler.tests.CProgram\n"
               "  implements\n"
               "    wisey.lang.IProgram {\n"
               "\n"
               "  int run(\n"
               "    immutable string[] arguments) override;\n"
               "}\n"
               "\n"
               "/* Nodes */\n"
               "\n"
               "/* Bindings */\n"
               "\n"
               "bind(wisey.lang.IProgram).to(systems.vos.wisey.compiler.tests.CProgram);\n"
               "\n"
               "/* llvm Structs */\n"
               "\n"
               "/* llvm Functions */\n"
               "\n"
               "/* llvm Globals */\n"
               "\n"
               "::llvm::struct::__sFILE::pointer __stderrp;\n",
               output.c_str());
  stream.close();
}

TEST_F(MainTest, emitLLVMTest) {
  system("mkdir -p build");

  string resultWithoutEmitLLVM =
  TestFileRunner::exec("bin/wiseyc -o build/test.bc tests/samples/test_addition.yz "
                             "libwisey/libwisey.yz");
  EXPECT_EQ(resultWithoutEmitLLVM.find("define i32 @main"), string::npos);
  
  string resultWithEmitLLVM = TestFileRunner::exec("bin/wiseyc --emit-llvm -o build/test.bc "
                                                   "tests/samples/test_addition.yz "
                                                   "libwisey/libwisey.yz");
  EXPECT_NE(resultWithEmitLLVM.find("define i32 @main"), string::npos);
  
  resultWithEmitLLVM = TestFileRunner::exec("bin/wiseyc -e -o build/test.bc "
                                            "tests/samples/test_addition.yz libwisey/libwisey.yz");
  EXPECT_NE(resultWithEmitLLVM.find("define i32 @main"), string::npos);
}

TEST_F(TestFileRunner, debugDestructorsRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_controller_injection_chain.yz",
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "freeing systems.vos.wisey.compiler.tests.CProgram.mTopController\n"
                                        "destructor systems.vos.wisey.compiler.tests.CTopController\n"
                                        "freeing systems.vos.wisey.compiler.tests.CTopController.mMiddleController\n"
                                        "destructor systems.vos.wisey.compiler.tests.CMiddleController\n"
                                        "freeing systems.vos.wisey.compiler.tests.CMiddleController.mBottomController\n"
                                        "destructor systems.vos.wisey.compiler.tests.CBottomController\n"
                                        "done destructing systems.vos.wisey.compiler.tests.CBottomController\n"
                                        "done destructing systems.vos.wisey.compiler.tests.CMiddleController\n"
                                        "done destructing systems.vos.wisey.compiler.tests.CTopController\n"
                                        "done destructing systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
