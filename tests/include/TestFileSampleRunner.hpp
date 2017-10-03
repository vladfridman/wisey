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
  static const std::string WISEYLIB;
  wisey::CompilerArguments mCompilerArguments;
  wisey::Compiler mCompiler;

public:
  TestFileSampleRunner();
  
  ~TestFileSampleRunner();
  
  /**
   * Compiles a file without running it
   */
  void compileFile(std::string fileName);
  
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
   * Execute a command, capture and return output
   */
  static std::string exec(const char* cmd);
};

#endif /* TestFileSampleRunner_h */
