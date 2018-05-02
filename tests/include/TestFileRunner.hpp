//
//  TestFileRunner.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef TestFileRunner_h
#define TestFileRunner_h

#include <gtest/gtest.h>

#include "wisey/Compiler.hpp"

/**
 * Runs a sample wisey file through compiler and checks whether the expected result matched 
 * the actual one.
 */
class TestFileRunner : public ::testing::Test {
  static const std::string LIBWISEY;
  static const char STDOUT_FILE[];
  static const char STDERR_FILE[];

  wisey::CompilerArguments mCompilerArguments;
  wisey::Compiler mCompiler;
  
public:
  
  TestFileRunner();
  
  ~TestFileRunner();
  
  /**
   * Compiles a file without running it
   */
  void compileFile(std::string fileName);
  
  /**
   * Compiles a sample file, runs it and checks the result against the expected one.
   */
  void runFile(std::string fileName, std::string expectedResult);

  /**
   * Compiles sample files, runs them and checks the stdout and stderr against the expected one.
   */
  void runFilesCheckOutput(std::vector<std::string> fileNames,
                           std::string expectedOut,
                           std::string expectedErr);

  /**
   * Compiles a single file, runs it and checks the stdout and stderr against the expected one.
   */
  void runFileCheckOutput(std::string fileName,
                          std::string expectedOut,
                          std::string expectedErr);

  /**
   * Compiles sample files with destructor debug option on, runs them and checks outputs.
   */
  void runFilesCheckOutputWithDestructorDebug(std::vector<std::string> fileNames,
                                              std::string expectedOut,
                                              std::string expectedErr);

  /**
   * Compiles a single sample file with destructor debug option on, runs it and checks outputs.
   */
  void runFileCheckOutputWithDestructorDebug(std::string fileName,
                                             std::string expectedOut,
                                             std::string expectedErr);

  /**
   * Compile and run given file and compare the result
   */
  void compileAndRunFile(std::string fileName, int expectedResult);

  /**
   * Compile and run given file and compare the result
   */
  void compileAndRunFileCheckOutput(std::string fileName,
                                    int expectedResult,
                                    std::string expectedOut,
                                    std::string expectedErr);

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
  
  /**
   * Check that output content written into the given file is the same as the expected output
   */
  void checkOutput(const char fileName[], std::string exprectedOut);
  
};

#endif /* TestFileRunner_h */
