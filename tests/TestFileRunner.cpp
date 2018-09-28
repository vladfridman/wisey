//
//  TestFileRunner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <stdio.h>

#include <llvm/IR/Verifier.h>
#include "llvm/Support/FileSystem.h"
#include <llvm/Support/TargetSelect.h>
#include <llvm-c/Target.h>

#include "TestFileRunner.hpp"
#include "IRGenerationContext.hpp"
#include "Log.hpp"
#include "Compiler.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

extern int yyparse();
extern FILE* yyin;
extern ProgramFile* programFile;

const string TestFileRunner::LIBWISEY = "build/libwisey.yz";
const char TestFileRunner::STDOUT_FILE[] = "build/wisey.out";
const char TestFileRunner::STDERR_FILE[] = "build/wisey.err";

TestFileRunner::TestFileRunner() : mCompiler(mCompilerArguments) {
  InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
  
  Log::setLogLevel(ERRORLEVEL);
}

TestFileRunner::~TestFileRunner() {
  fclose(yyin);
}

void TestFileRunner::compileFile(string fileName) {
  mCompilerArguments.addSourceFile(fileName);
  mCompilerArguments.addSourceFile(LIBWISEY);
  mCompiler.compile();
}

void TestFileRunner::runFile(string fileName, int expectedResult) {
  mCompilerArguments.addSourceFile(fileName);
  mCompilerArguments.addSourceFile(LIBWISEY);
  mCompiler.compile();
  int result = mCompiler.run(0, NULL);
  
  ASSERT_EQ(expectedResult, result);
}

void TestFileRunner::runFilesCheckOutput(vector<string> fileNames,
                                         string expectedOut,
                                         string expectedErr) {
  exec("mkdir -p build");
  
  for (string fileName : fileNames) {
    mCompilerArguments.addSourceFile(fileName);
  }
  mCompilerArguments.addSourceFile(LIBWISEY);
  mCompiler.compile();
  
  FILE* wiseyStdOut = fopen(STDOUT_FILE, "w");
  FILE* wiseyStdErr = fopen(STDERR_FILE, "w");
  int oldStdOut = dup(STDOUT_FILENO);
  int oldStdErr = dup(STDERR_FILENO);
  dup2(fileno(wiseyStdOut), STDOUT_FILENO);
  dup2(fileno(wiseyStdErr), STDERR_FILENO);
  fclose(wiseyStdOut);
  fclose(wiseyStdErr);
  
  mCompiler.run(0, NULL);
  
  dup2(oldStdOut, STDOUT_FILENO);
  dup2(oldStdErr, STDERR_FILENO);
  
  checkOutput(STDOUT_FILE, expectedOut);
  checkOutput(STDERR_FILE, expectedErr);
}

void TestFileRunner::runFilesCheckOutputWithDestructorDebug(vector<string> fileNames,
                                                            string expectedOut,
                                                            string expectedErr) {
  mCompilerArguments.setDestructorDebug(true);
  runFilesCheckOutput(fileNames, expectedOut, expectedErr);
}

void TestFileRunner::runFileCheckOutput(string fileName,
                                        string expectedOut,
                                        string expectedErr) {
  vector<string> fileNames;
  fileNames.push_back(fileName);
  runFilesCheckOutput(fileNames, expectedOut, expectedErr);
}

void TestFileRunner::runFileCheckOutputWithDestructorDebug(string fileName,
                                                           string expectedOut,
                                                           string expectedErr) {
  vector<string> fileNames;
  fileNames.push_back(fileName);
  runFilesCheckOutputWithDestructorDebug(fileNames, expectedOut, expectedErr);
}

void TestFileRunner::expectFailCompile(string fileName,
                                       int expectedErrorCode,
                                       string expectedErrorMessage) {
  mCompilerArguments.addSourceFile(fileName);
  mCompilerArguments.addSourceFile(LIBWISEY);
  
  EXPECT_EXIT(mCompiler.compile(),
              ::testing::ExitedWithCode(expectedErrorCode),
              expectedErrorMessage);
}

void TestFileRunner::compileAndRunFile(string fileName, int expectedResult) {
  exec("mkdir -p build");
  
  string wiseyCompileCommand = "bin/yzc " + fileName + " " + LIBWISEY + " -o build/test.o";
  exec(wiseyCompileCommand.c_str());
  exec("g++ -o build/test build/test.o -Llib -lwisey");
  int result = system("build/test");
  int returnValue = WEXITSTATUS(result);
  
  EXPECT_EQ(returnValue, expectedResult);
}

void TestFileRunner::compileAndRunFileCheckOutput(string fileName,
                                                  int expectedResult,
                                                  string expectedOut,
                                                  string expectedErr) {
  exec("mkdir -p build");
  
  string wiseyCompileCommand = "bin/yzc " + fileName + " " + LIBWISEY + " -o build/test.o";
  exec(wiseyCompileCommand.c_str());
  exec("g++ -o build/test build/test.o -Llib -lwisey");
  int result = system("build/test > build/wisey.out 2> build/wisey.err");
  int returnValue = WEXITSTATUS(result);
  
  checkOutput(STDOUT_FILE, expectedOut);
  checkOutput(STDERR_FILE, expectedErr);
  
  EXPECT_EQ(returnValue, expectedResult);
}

void TestFileRunner::compileAndRunFileWithInputCheckOutput(string fileName,
                                                           int expectedResult,
                                                           string inputFile,
                                                           string expectedOut,
                                                           string expectedErr) {
  exec("mkdir -p build");
  
  string wiseyCompileCommand = "bin/yzc " + fileName + " " + LIBWISEY + " -o build/test.o";
  exec(wiseyCompileCommand.c_str());
  exec("g++ -o build/test build/test.o -Llib -lwisey");
  string command = "build/test < " + inputFile + "> build/wisey.out 2> build/wisey.err";
  int result = system(command.c_str());
  int returnValue = WEXITSTATUS(result);
  
  checkOutput(STDOUT_FILE, expectedOut);
  checkOutput(STDERR_FILE, expectedErr);
  
  EXPECT_EQ(returnValue, expectedResult);
}

string TestFileRunner::exec(const char* cmd) {
  array<char, 128> buffer;
  string result;
  shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
  if (!pipe) return "popen() failed!";
  while (!feof(pipe.get())) {
    if (fgets(buffer.data(), 128, pipe.get()) != NULL)
      result += buffer.data();
  }
  return result;
}

void TestFileRunner::checkOutput(const char fileName[], string expectedOut) {
  char* contents;
  long fileSize;
  FILE* file = fopen(fileName, "rb");
  fseek(file, 0, SEEK_END);
  fileSize = ftell(file) + 1;
  rewind(file);
  contents = (char*) malloc(fileSize * (sizeof(char)));
  fread(contents, sizeof(char), fileSize, file);
  fclose(file);
  contents[fileSize - 1] = '\0';
  
  ASSERT_STREQ(expectedOut.c_str(), contents);
}


