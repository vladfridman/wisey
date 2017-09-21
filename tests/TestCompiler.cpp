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
  CompilerArguments mCompilerArguments;
  Compiler mCompiler;
 
  CompilerTest() : mCompiler(mCompilerArguments) {
    mCompilerArguments.addSourceFile("tests/samples/test_addition.yz");
  }
};

TEST_F(CompilerTest, compileAndRunTest) {
  mCompiler.compile();
  GenericValue result = mCompiler.run();
  string resultString = result.IntVal.toString(10, true);
  
  EXPECT_STREQ(resultString.c_str(), "7");
}

TEST_F(CompilerTest, compileAndSaveTest) {
  system("mkdir -p build");
  
  mCompilerArguments.setOutputFile("build/test.bc");
  mCompiler.compile();
  
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

