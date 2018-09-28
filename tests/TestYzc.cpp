//
//  TestYzc.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests yzc.cpp
//

#include <gtest/gtest.h>

#include <array>
#include <fstream>
#include <iostream>

#include "TestFileRunner.hpp"

using namespace std;

struct YzcTest : public ::testing::Test {

  YzcTest() {
    system("mkdir -p build");
  }
  
  ~YzcTest() { }
};

TEST_F(YzcTest, noArgumentsTest) {
  EXPECT_STREQ("Syntax: yzc "
               "[-d|--destructor-debug] "
               "[-e|--emit-llvm] "
               "[-h|--help] "
               "[-v|--verbouse] "
               "[-H|--headers <header_file.yz>] "
               "[-o|--output <object_file_name.o>] "
               "[-n|--no-output] "
               "[--no-optimization] "
               "[--no-main] "
               "[--target-triple <target_machine_triple>] "
               "<source_file.yz>...\n",
               TestFileRunner::exec("bin/yzc 2>&1").c_str());
}

TEST_F(YzcTest, missingFileTest) {
  EXPECT_STREQ(TestFileRunner::exec("bin/yzc -v "
                                          "tests/samples/missingFile.yz 2>&1").c_str(),
               "Error: File tests/samples/missingFile.yz not found!\n");
}

TEST_F(YzcTest, missingOutputFileTest) {
  EXPECT_STREQ(TestFileRunner::exec("bin/yzc tests/samples/test_addition.yz -o "
                                          "2>&1").c_str(),
               "Error: You need to specify the output file name after \"-o\"\n");
  EXPECT_STREQ(TestFileRunner::exec("bin/yzc tests/samples/test_addition.yz --output "
                                          "2>&1").c_str(),
               "Error: You need to specify the output file name after \"--output\"\n");
}

TEST_F(YzcTest, helpTest) {
  EXPECT_STREQ("Syntax: yzc "
               "[-d|--destructor-debug] "
               "[-e|--emit-llvm] "
               "[-h|--help] "
               "[-v|--verbouse] "
               "[-H|--headers <header_file.yz>] "
               "[-o|--output <object_file_name.o>] "
               "[-n|--no-output] "
               "[--no-optimization] "
               "[--no-main] "
               "[--target-triple <target_machine_triple>] "
               "<source_file.yz>...\n",
               TestFileRunner::exec("bin/yzc -h 2>&1").c_str());
  EXPECT_STREQ("Syntax: yzc "
               "[-d|--destructor-debug] "
               "[-e|--emit-llvm] "
               "[-h|--help] "
               "[-v|--verbouse] "
               "[-H|--headers <header_file.yz>] "
               "[-o|--output <object_file_name.o>] "
               "[-n|--no-output] "
               "[--no-optimization] "
               "[--no-main] "
               "[--target-triple <target_machine_triple>] "
               "<source_file.yz>...\n",
               TestFileRunner::exec("bin/yzc --help 2>&1").c_str());
}

TEST_F(YzcTest, outputToFileTest) {
  system("mkdir -p build");

  system("bin/yzc tests/samples/test_addition.yz build/libwisey.yz -o "
         "build/test.o 2>&1");
  system("g++ -o build/test build/test.o -Llib -lwisey");
  int result = system("build/test");
  int returnValue = WEXITSTATUS(result);
  
  EXPECT_EQ(returnValue, 7);
}

TEST_F(YzcTest, extractHeadersTest) {
  system("mkdir -p build");
  system("bin/yzc tests/samples/test_addition.yz build/libwisey.yz "
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
               "external ::llvm::i64 wProcessorCount;\n"
               "external ::llvm::struct::__sFILE::pointer wStdErr;\n"
               "external ::llvm::struct::__sFILE::pointer wStdIn;\n"
               "external ::llvm::struct::__sFILE::pointer wStdOut;\n",
               output.c_str());
  stream.close();
}

TEST_F(YzcTest, emitLLVMTest) {
  system("mkdir -p build");

  string resultWithoutEmitLLVM =
  TestFileRunner::exec("bin/yzc -o build/test.bc tests/samples/test_addition.yz "
                             "build/libwisey.yz");
  EXPECT_EQ(resultWithoutEmitLLVM.find("define i32 @main"), string::npos);
  
  string resultWithEmitLLVM = TestFileRunner::exec("bin/yzc --emit-llvm -o build/test.bc "
                                                   "tests/samples/test_addition.yz "
                                                   "build/libwisey.yz");
  EXPECT_NE(resultWithEmitLLVM.find("define i32 @main"), string::npos);
  
  resultWithEmitLLVM = TestFileRunner::exec("bin/yzc -e -o build/test.bc "
                                            "tests/samples/test_addition.yz "
                                            "build/libwisey.yz");
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
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CBottomController\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CMiddleController\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CTopController\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
