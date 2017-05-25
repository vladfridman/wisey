//
//  TestFileSampleRunner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

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

TestFileSampleRunner::TestFileSampleRunner() {
  InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
  
  Log::setLogLevel(ERRORLEVEL);
}

TestFileSampleRunner::~TestFileSampleRunner() {
  fclose(yyin);
}

void TestFileSampleRunner::runFile(string fileName, string expectedResult) {
  mSourceFiles.push_back(fileName);
  
  mCompiler.compile(mSourceFiles, false);
  GenericValue result = mCompiler.run();
  string resultString = result.IntVal.toString(10, true);

  ASSERT_STREQ(expectedResult.c_str(), resultString.c_str());
}

void TestFileSampleRunner::expectFailCompile(string fileName,
                                             int expectedErrorCode,
                                             string expectedErrorMessage) {
  mSourceFiles.push_back(fileName);
  
  EXPECT_EXIT(mCompiler.compile(mSourceFiles, false),
              ::testing::ExitedWithCode(expectedErrorCode),
              expectedErrorMessage);
}

void TestFileSampleRunner::expectDeathDuringRun(string fileName,
                                                string expectedErrorMessage) {
  mSourceFiles.push_back(fileName);
  mCompiler.compile(mSourceFiles, false);
  
  ASSERT_DEATH(mCompiler.run(), expectedErrorMessage);
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
