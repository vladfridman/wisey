//
//  TestFileSampleRunner.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef TestFileSampleRunner_h
#define TestFileSampleRunner_h

#include <gtest/gtest.h>

#include "wisey/Compiler.hpp"

/**
 * Runs a sample wisey file through compiler and checks whether the expected result matched 
 * the actual one.
 */
class TestFileSampleRunner : public ::testing::Test {
  wisey::Compiler mCompiler;
  std::vector<std::string> mSourceFiles;
  
public:
  TestFileSampleRunner();
  
  ~TestFileSampleRunner();
  
  /**
   * Compiles a sample file, runs it and checks the result against the expected one.
   */
  void runFile(std::string fileName, std::string expectedResult);

  /**
   * Compiles a sample file, runs it and checks the stdout and stderr against the expected one.
   */
  void runFileCheckOutput(std::string fileName, std::string expectedOut, std::string expectedErr);

  /**
   * Compile and run given file and compare the result
   */
  void compileAndRunFile(std::string fileName, int expectedResult);
  
  /**
   * Call this to run a sample wisey file that is expected to fail at compilation
   */
  void expectFailCompile(std::string fileName,
                         int expectedErrorCode,
                         std::string expectedErrorMessage);
  
  /**
   * Call this to run a sample wisey file that is expected to pass IR generation but
   * that will die during run with a given error message
   */
  void expectDeathDuringRun(std::string fileName,
                            std::string expectedErrorMessage);
  

private:
  
  std::string exec(const char* cmd);
};

#endif /* TestFileSampleRunner_h */
