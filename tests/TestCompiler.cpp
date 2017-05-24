//
//  TestCompiler.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CompilerArgumentParser}
//

#include <gtest/gtest.h>

#include <llvm/ExecutionEngine/GenericValue.h>

#include "wisey/Compiler.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

struct CompilerTest : public ::testing::Test {
  CompilerArguments mArguments;
  vector<const char*> mSourceFiles;
  Compiler mCompiler;
 
  CompilerTest() {
    mSourceFiles.push_back("tests/samples/test_addition.yz");
  }
};

TEST_F(CompilerTest, compileAndRunTest) {
  mCompiler.compile(mSourceFiles, true);
  GenericValue result = mCompiler.run();
  string resultString = result.IntVal.toString(10, true);
  
  EXPECT_STREQ(resultString.c_str(), "7");
}

TEST_F(CompilerTest, compileAndSaveTest) {
  system("mkdir -p build");
  
  mCompiler.compile(mSourceFiles, true);
  mCompiler.saveBitcode("build/test.bc");
  system("/usr/local/bin/llc -filetype=obj build/test.bc");
  system("g++ -o build/test build/test.o");
  int result = system("build/test");
  int returnValue = WEXITSTATUS(result);
  
  EXPECT_EQ(returnValue, 7);
}

TEST_F(CompilerTest, runWithoutCompileDeathTest) {
  EXPECT_EXIT(mCompiler.run(),
              ::testing::ExitedWithCode(1),
              "Error: Need to compile before running code");
}

TEST_F(CompilerTest, printWithoutCompileDeathTest) {
  EXPECT_EXIT(mCompiler.printAssembly(),
              ::testing::ExitedWithCode(1),
              "Error: Need to compile before printing assembly");
}

TEST_F(CompilerTest, saveWithoutCompileDeathTest) {
  EXPECT_EXIT(mCompiler.saveBitcode("test"),
              ::testing::ExitedWithCode(1),
              "Error: Need to compile before saving bitcode");
}
