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

#include "TestFileRunner.hpp"
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
  long result = mCompiler.run(0, NULL);
  
  EXPECT_EQ(7, result);
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

  EXPECT_EXIT(mCompiler.run(0, NULL),
              ::testing::ExitedWithCode(1),
              "Error: Need to compile before running code");
}

TEST_F(CompilerTest, runMultipleFilesTest) {
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_controller_controller/Adder.yz");
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_controller_controller/Runner.yz");
  mCompilerArguments.addSourceFile(LIBWISEY);
  mCompiler.compile();
  long result = mCompiler.run(0, NULL);

  EXPECT_EQ(5, result);
}

TEST_F(CompilerTest, runMultipleFilesOutOfOrderTest) {
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_controller_controller/Runner.yz");
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_controller_controller/Adder.yz");
  mCompilerArguments.addSourceFile(LIBWISEY);
  mCompiler.compile();
  long result = mCompiler.run(0, NULL);

  EXPECT_EQ(5, result);
}

TEST_F(CompilerTest, runMultipleFilesControllerAndModelsTest) {
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_controller_model/MAdder.yz");
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_controller_model/CProgram.yz");
  mCompilerArguments.addSourceFile(LIBWISEY);
  mCompiler.compile();
  long result = mCompiler.run(0, NULL);

  EXPECT_EQ(7, result);
}

TEST_F(CompilerTest, runMultipleFilesInterdependentModelsTest) {
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_model_model/CProgram.yz");
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_model_model/MAdder.yz");
  mCompilerArguments.addSourceFile(LIBWISEY);
  mCompiler.compile();
  long result = mCompiler.run(0, NULL);

  EXPECT_EQ(6, result);
}

TEST_F(CompilerTest, runMultipleFilesUseWildcardTest) {
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_model_model/*.yz");
  mCompilerArguments.addSourceFile(LIBWISEY);
  mCompiler.compile();
  long result = mCompiler.run(0, NULL);

  EXPECT_EQ(6, result);
}

TEST_F(CompilerTest, runWildcardDirectoryDeathTest) {
  mCompilerArguments.addSourceFile("tests/samples/*/*.yz");
  mCompilerArguments.addSourceFile(LIBWISEY);
  EXPECT_EXIT(mCompiler.compile(),
              ::testing::ExitedWithCode(1),
              "Error: Directory wildcard matching is not supported");
}

TEST_F(CompilerTest, runNonExistingDirectoryDeathTest) {
  mCompilerArguments.addSourceFile("tests/foo/*.yz");
  mCompilerArguments.addSourceFile(LIBWISEY);
  EXPECT_EXIT(mCompiler.compile(),
              ::testing::ExitedWithCode(1),
              "Error: Could not open directory tests/foo");
}

TEST_F(CompilerTest, unterminatedCommentRunDeathTest) {
  string result = TestFileRunner::exec("bin/wiseyc "
                                       "tests/samples/test_unterminated_comment.yz");
  
  EXPECT_STREQ(result.c_str(), "tests/samples/test_unterminated_comment.yz(2): "
               "unterminated comment\n");
}

TEST_F(CompilerTest, twoErrorsRunDeathTest) {
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_two_errors/Adder.yz");
  mCompilerArguments.addSourceFile("tests/samples/test_multifile_two_errors/Runner.yz");
  mCompilerArguments.addSourceFile(LIBWISEY);
  
  EXPECT_EXIT(mCompiler.compile(),
              ::testing::ExitedWithCode(1),
              "tests/samples/test_multifile_two_errors/Adder.yz\\(5\\): Error: Undeclared variable 'j'\n"
              "tests/samples/test_multifile_two_errors/Runner.yz\\(10\\): Error: Undeclared variable 'i'\n");
}

TEST_F(TestFileRunner, commentsRunTest) {
  runFile("tests/samples/test_comments.yz", 5);
}

TEST_F(TestFileRunner, noPackageDeclarationRunTest) {
  runFile("tests/samples/test_no_package_declaration.yz", 7);
}

TEST_F(TestFileRunner, noPackageRunTest) {
  runFile("tests/samples/test_no_package.yz", 1);
}
