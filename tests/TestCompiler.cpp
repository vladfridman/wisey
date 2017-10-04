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

#include "TestFileSampleRunner.hpp"
#include "wisey/Compiler.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

struct CompilerTest : public ::testing::Test {
  CompilerArguments mCompilerArguments;
  Compiler mCompiler;
  static const string LIBWISEY;
 
  CompilerTest() : mCompiler(mCompilerArguments) { }
};

const string CompilerTest::LIBWISEY =  "libwisey/libwisey.yz";

TEST_F(CompilerTest, compileAndRunTest) {
  mCompilerArguments.addSourceFile("tests/samples/test_addition.yz");
  mCompilerArguments.addSourceFile(LIBWISEY);
  mCompiler.compile();
  GenericValue result = mCompiler.run();
  string resultString = result.IntVal.toString(10, true);
  
  EXPECT_STREQ(resultString.c_str(), "7");
}

TEST_F(CompilerTest, compileAndSaveTest) {
  mCompilerArguments.addSourceFile("tests/samples/test_addition.yz");
  mCompilerArguments.addSourceFile(LIBWISEY);
  system("mkdir -p build");
  
  mCompilerArguments.setOutputFile("build/test.o");
  mCompiler.compile();
  
  system("g++ -o build/test build/test.o -Llibwisey -lwisey");
  int result = system("build/test");
  int returnValue = WEXITSTATUS(result);
  
  EXPECT_EQ(returnValue, 7);
}

TEST_F(CompilerTest, runWithoutCompileDeathTest) {
  mCompilerArguments.addSourceFile("tests/samples/test_addition.yz");

  EXPECT_EXIT(mCompiler.run(),
              ::testing::ExitedWithCode(1),
              "Error: Need to compile before running code");
}

TEST_F(CompilerTest, runMultipleFilesTest) {
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_controller_controller/Adder.yz");
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_controller_controller/Runner.yz");
  mCompilerArguments.addSourceFile(LIBWISEY);
  mCompiler.compile();
  GenericValue result = mCompiler.run();
  string resultString = result.IntVal.toString(10, true);

  EXPECT_STREQ(resultString.c_str(), "5");
}

TEST_F(CompilerTest, runMultipleFilesOutOfOrderTest) {
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_controller_controller/Runner.yz");
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_controller_controller/Adder.yz");
  mCompilerArguments.addSourceFile(LIBWISEY);
  mCompiler.compile();
  GenericValue result = mCompiler.run();
  string resultString = result.IntVal.toString(10, true);

  EXPECT_STREQ(resultString.c_str(), "5");
}

TEST_F(CompilerTest, runMultipleFilesControllerAndModelsTest) {
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_controller_model/MAdder.yz");
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_controller_model/CProgram.yz");
  mCompilerArguments.addSourceFile(LIBWISEY);
  mCompiler.compile();
  GenericValue result = mCompiler.run();
  string resultString = result.IntVal.toString(10, true);

  EXPECT_STREQ(resultString.c_str(), "7");
}

TEST_F(CompilerTest, runMultipleFilesInterdependentModelsTest) {
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_model_model/CProgram.yz");
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_model_model/MAdder.yz");
  mCompilerArguments.addSourceFile(LIBWISEY);
  mCompiler.compile();
  GenericValue result = mCompiler.run();
  string resultString = result.IntVal.toString(10, true);

  EXPECT_STREQ(resultString.c_str(), "6");
}

TEST_F(TestFileSampleRunner, commentsRunTest) {
  runFile("tests/samples/test_comments.yz", "5");
}

TEST_F(TestFileSampleRunner, noPackageDeclarationRunTest) {
  runFile("tests/samples/test_no_package_declaration.yz", "7");
}

TEST_F(CompilerTest, unterminatedCommentRunDeathTest) {
  string result = TestFileSampleRunner::exec("bin/wiseyc "
                                             "tests/samples/test_unterminated_comment.yz");
  
  string lastLine = result.substr(result.find("Line 2:"));
  EXPECT_STREQ(lastLine.c_str(), "Line 2: unterminated comment\n");
}
