//
//  TestFileSampleRunner.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef TestFileSampleRunner_h
#define TestFileSampleRunner_h

#include "gtest/gtest.h"
#include "gmock/gmock.h"

/**
 * Runs a sample yazyk file through compiler and checks whether the expected result matched 
 * the actual one.
 */
class TestFileSampleRunner : public testing::Test {
public:
  TestFileSampleRunner();
  
  ~TestFileSampleRunner();
  
  /**
   * Call this to run a sample yazyk file through the compiler and check the output of a compiled
   * program against the expected one.
   */
  void runFile(std::string fileName, std::string expectedResult);
  
  /**
   * Call this to run a sample yazyk file that is expected to fail at IR generation
   */
  void expectFailIRGeneration(std::string fileName,
                              int expectedErrorCode,
                              std::string expectedErrorMessage);

  /**
   * Call this to run a sample yazyk file that is expected to pass IR generation but
   * that will die during run with a given error message
   */
  void expectDeathDuringRun(std::string fileName,
                            std::string expectedErrorMessage);
  

private:
  
  void parseFile(std::string fileName);
};

#endif /* TestFileSampleRunner_h */
