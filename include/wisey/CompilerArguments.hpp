//
//  CompilerArguments.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef CompilerArguments_h
#define CompilerArguments_h

#include <string>
#include <vector>

namespace wisey {
  
/**
 * Represents parsed command line arguments passed to the Wisey compiler
 */
class CompilerArguments {
  std::string mOutputFile;
  std::string mHeaderFile;
  bool mShouldPrintAssembly;
  bool mIsVerbouse;
  bool mShouldOutput;
  bool mIsDestructorDebugOn;
  std::vector<std::string> mSourceFiles;
  
public:
  
  CompilerArguments() :
  mOutputFile(""),
  mHeaderFile(""),
  mShouldPrintAssembly(false),
  mIsVerbouse(false),
  mShouldOutput(true),
  mIsDestructorDebugOn(false) { }
  
  ~CompilerArguments() { }
  
  std::string getOutputFile() { return mOutputFile; }
  
  void setOutputFile(std::string outputFile) { mOutputFile = outputFile; }
  
  std::string getHeaderFile() { return mHeaderFile; }
  
  void setHeaderFile(std::string headerFile) { mHeaderFile = headerFile; }
  
  bool shouldPrintAssembly() { return mShouldPrintAssembly; }
  
  void setShouldPrintAssembly(bool value) { mShouldPrintAssembly = value; }
  
  bool isVerbouse() { return mIsVerbouse; }
  
  void setVerbouse(bool value) { mIsVerbouse = value; }
  
  bool shouldOutput() { return mShouldOutput; }
  
  void setShouldOutput(bool value) { mShouldOutput = value; }
  
  std::vector<std::string> getSourceFiles() { return mSourceFiles; }
  
  void addSourceFile(std::string sourceFile) { mSourceFiles.push_back(sourceFile); }

  void setDestructorDebug(bool value) { mIsDestructorDebugOn = value; }
  
  bool isDestructorDebugOn() { return mIsDestructorDebugOn; }
  
};
  
} /* namespace wisey */

#endif /* CompilerArguments_h */
