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
  bool mShouldPrintAssembly;
  std::vector<std::string> mSourceFiles;
  
public:
  
  CompilerArguments() : mOutputFile(""), mShouldPrintAssembly(false) { }
  
  ~CompilerArguments() { }
  
  std::string getOutputFile() { return mOutputFile; }
  
  void setOutputFile(std::string outputFile) { mOutputFile = outputFile; }
  
  bool shouldPrintAssembly() { return mShouldPrintAssembly; }
  
  void setShouldPrintAssembly(bool value) { mShouldPrintAssembly = value; }
  
  std::vector<std::string> getSourceFiles() { return mSourceFiles; }
  
  void addSourceFile(std::string sourceFile) { mSourceFiles.push_back(sourceFile); }
  
};
  
} /* namespace wisey */

#endif /* CompilerArguments_h */
