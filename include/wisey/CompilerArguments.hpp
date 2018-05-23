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
    bool mShouldOptimize;
    std::vector<std::string> mSourceFiles;
    
  public:
    
    CompilerArguments() :
    mOutputFile(""),
    mHeaderFile(""),
    mShouldPrintAssembly(false),
    mIsVerbouse(false),
    mShouldOutput(true),
    mIsDestructorDebugOn(false),
    mShouldOptimize(true) {
    }
    
    ~CompilerArguments() {
    }
    
    /**
     * Returns output object file name
     */
    std::string getOutputFile() { return mOutputFile; }
    
    /**
     * Configures output object file name
     */
    void setOutputFile(std::string outputFile) { mOutputFile = outputFile; }
    
    /**
     * Returns header file name that compiler should generate
     */
    std::string getHeaderFile() { return mHeaderFile; }
    
    /**
     * Configures header file name that compiler should generate
     */
    void setHeaderFile(std::string headerFile) { mHeaderFile = headerFile; }
    
    /**
     * Tells whether the compiler should print generate IR code
     */
    bool shouldPrintAssembly() { return mShouldPrintAssembly; }
    
    /**
     * Configures whether the compiler should print generate IR code
     */
    void setShouldPrintAssembly(bool value) { mShouldPrintAssembly = value; }
    
    /**
     * Tells whether the verbouse mode is on
     */
    bool isVerbouse() { return mIsVerbouse; }
    
    /**
     * Turns on verbouse mode
     */
    void setVerbouse(bool value) { mIsVerbouse = value; }
    
    /**
     * Tells whether the compiler should generate a binary file
     */
    bool shouldOutput() { return mShouldOutput; }
    
    /**
     * Configures whether the compiler should generate a binary file
     */
    void setShouldOutput(bool value) { mShouldOutput = value; }
    
    /**
     * Returns the list of source files
     */
    std::vector<std::string> getSourceFiles() { return mSourceFiles; }
    
    /**
     * Adds a source file to the list of source files
     */
    void addSourceFile(std::string sourceFile) { mSourceFiles.push_back(sourceFile); }
    
    /**
     * Sets whether destructor debug messages should be generated
     */
    void setDestructorDebug(bool value) { mIsDestructorDebugOn = value; }
    
    /**
     * Tells whether the destructor debug messages should be generated
     */
    bool isDestructorDebugOn() { return mIsDestructorDebugOn; }
    
    /**
     * Tells whether the compiler should run optimization passes
     */
    bool shouldOptimize() { return mShouldOptimize; }
    
    /**
     * Configures whether the compiler should run optimization passes
     */
    void setShouldOptimize(bool value) { mShouldOptimize = value; }
    
  };
  
} /* namespace wisey */

#endif /* CompilerArguments_h */

