//
//  TestFileSampleRunner.cpp
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

#include "TestFileSampleRunner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/Compiler.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

extern int yyparse();
extern FILE* yyin;
extern ProgramFile* programFile;

TestFileSampleRunner::TestFileSampleRunner() : mCompiler(mCompilerArguments) {
  InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
  
  Log::setLogLevel(ERRORLEVEL);
}

TestFileSampleRunner::~TestFileSampleRunner() {
  fclose(yyin);
}

void TestFileSampleRunner::runFile(string fileName, string expectedResult) {
  mCompilerArguments.addSourceFile(fileName);
  mCompiler.compile();
  GenericValue result = mCompiler.run();
  string resultString = result.IntVal.toString(10, true);

  ASSERT_STREQ(expectedResult.c_str(), resultString.c_str());
}

void TestFileSampleRunner::runFileCheckOutput(string fileName,
                                              string expectedOut,
                                              string expectedErr) {
  exec("mkdir -p build");

  mCompilerArguments.addSourceFile(fileName);
  mCompiler.compile();

  FILE* wiseyStdOut = fopen("build/wisey.out", "w");
  FILE* wiseyStdErr = fopen("build/wisey.err", "w");
  int oldStdOut = dup(STDOUT_FILENO);
  int oldStdErr = dup(STDERR_FILENO);
  dup2(fileno(wiseyStdOut), STDOUT_FILENO);
  dup2(fileno(wiseyStdErr), STDERR_FILENO);
  fclose(wiseyStdOut);
  fclose(wiseyStdErr);
  
  GenericValue result = mCompiler.run();
  string resultString = result.IntVal.toString(10, true);

  dup2(oldStdOut, STDOUT_FILENO);
  dup2(oldStdErr, STDERR_FILENO);
  
  char* stdOutContents;
  long stdOutFileSize;
  FILE* stdOutFile = fopen("build/wisey.out", "rb");
  fseek(stdOutFile, 0, SEEK_END);
  stdOutFileSize = ftell(stdOutFile) + 1;
  rewind(stdOutFile);
  stdOutContents = (char*) malloc(stdOutFileSize * (sizeof(char)));
  fread(stdOutContents, sizeof(char), stdOutFileSize, stdOutFile);
  fclose(stdOutFile);
  stdOutContents[stdOutFileSize - 1] = '\0';
  
  ASSERT_STREQ(expectedOut.c_str(), stdOutContents);

  char* stdErrContents;
  long stdErrFileSize;
  FILE* stdErrFile = fopen("build/wisey.err", "rb");
  fseek(stdErrFile, 0, SEEK_END);
  stdErrFileSize = ftell(stdErrFile) + 1;
  rewind(stdErrFile);
  stdErrContents = (char*) malloc(stdErrFileSize * (sizeof(char)));
  fread(stdErrContents, sizeof(char), stdErrFileSize, stdErrFile);
  fclose(stdErrFile);
  stdErrContents[stdErrFileSize - 1] = '\0';
  
  ASSERT_STREQ(expectedErr.c_str(), stdErrContents);
}

void TestFileSampleRunner::expectFailCompile(string fileName,
                                             int expectedErrorCode,
                                             string expectedErrorMessage) {
  mCompilerArguments.addSourceFile(fileName);

  EXPECT_EXIT(mCompiler.compile(),
              ::testing::ExitedWithCode(expectedErrorCode),
              expectedErrorMessage);
}

void TestFileSampleRunner::compileAndRunFile(string fileName, int expectedResult) {
  exec("mkdir -p build");
  
  string wiseyCompileCommand = "bin/wisey " + fileName + " -o build/test.bc";
  exec(wiseyCompileCommand.c_str());
  exec("/usr/local/bin/llc -filetype=obj build/test.bc");
  exec("g++ -o build/test build/test.o");
  int result = system("build/test");
  int returnValue = WEXITSTATUS(result);
  
  EXPECT_EQ(returnValue, expectedResult);
}

string TestFileSampleRunner::exec(const char* cmd) {
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
