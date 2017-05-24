//
//  CompilerArguments.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef CompilerArguments_h
#define CompilerArguments_h

#include <vector>

namespace wisey {
  
/**
 * Represents parsed command line arguments passed to the Wisey compiler
 */
class CompilerArguments {
  char* mOutputFile;
  bool mShouldPrintAssembly;
  std::vector<const char*> mSourceFiles;
  
public:
  
  CompilerArguments() : mOutputFile(NULL), mShouldPrintAssembly(false) { }
  
  ~CompilerArguments() { }
  
  char* getOutputFile() { return mOutputFile; }
  
  void setOutputFile(char* outputFile) { mOutputFile = outputFile; }
  
  bool shouldPrintAssembly() { return mShouldPrintAssembly; }
  
  void setShouldPrintAssembly(bool value) { mShouldPrintAssembly = value; }
  
  std::vector<const char*> getSourceFiles() { return mSourceFiles; }
  
  void addSourceFile(const char* sourceFile) { mSourceFiles.push_back(sourceFile); }
  
};
  
} /* namespace wisey */

#endif /* CompilerArguments_h */
