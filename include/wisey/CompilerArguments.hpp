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
    bool mShouldGenerateMain;
    std::vector<std::string> mSourceFiles;
    std::vector<std::string> mLibraryPaths;
    std::vector<std::string> mLibraryNames;
    std::vector<std::string> mAdditionalObjects;

  public:
    
    CompilerArguments();
    
    ~CompilerArguments();
    
    /**
     * Returns output object file name
     */
    std::string getOutputFile() const;
    
    /**
     * Configures output object file name
     */
    void setOutputFile(std::string outputFile);
    
    /**
     * Returns header file name that compiler should generate
     */
    std::string getHeaderFile() const;
    
    /**
     * Configures header file name that compiler should generate
     */
    void setHeaderFile(std::string headerFile);
    
    /**
     * Tells whether the compiler should print generate IR code
     */
    bool shouldPrintAssembly() const;
    
    /**
     * Configures whether the compiler should print generate IR code
     */
    void setShouldPrintAssembly(bool value);
    
    /**
     * Tells whether the verbouse mode is on
     */
    bool isVerbouse() const;
    
    /**
     * Turns on verbouse mode
     */
    void setVerbouse(bool value);
    
    /**
     * Tells whether the compiler should generate a binary file
     */
    bool shouldOutput() const;
    
    /**
     * Configures whether the compiler should generate a binary file
     */
    void setShouldOutput(bool value);
    
    /**
     * Returns the list of source files
     */
    std::vector<std::string> getSourceFiles() const;
    
    /**
     * Adds a source file to the list of source files
     */
    void addSourceFile(std::string sourceFile);
    
    /**
     * Tells whether the destructor debug messages should be generated
     */
    bool isDestructorDebugOn() const;

    /**
     * Sets whether destructor debug messages should be generated
     */
    void setDestructorDebug(bool value);
    
    /**
     * Tells whether the compiler should run optimization passes
     */
    bool shouldOptimize() const;
    
    /**
     * Configures whether the compiler should run optimization passes
     */
    void setShouldOptimize(bool value);
    
    /**
     * Returns list of library paths
     */
    std::vector<std::string> getLibraryPaths() const;
    
    /**
     * Adds a library path
     */
    void addLibraryPath(std::string path);
    
    /**
     * Returns list of library names
     */
    std::vector<std::string> getLibraryNames() const;
    
    /**
     * Adds a library name
     */
    void addLibraryName(std::string name);
    
    /**
     * Returns list of additional objects for linking into a library
     */
    std::vector<std::string> getAdditionalObjects() const;
    
    /**
     * Adds an additioinal object for linking into a library
     */
    void addAdditionalObject(std::string name);

    /**
     * Tells whether main() function should be generated
     */
    bool shouldGenerateMain() const;
    
    /**
     * Configures arguments not to generate main() function
     */
    void setShouldNotGenerateMain();

    /**
     * Return compiler arguments for yzc
     */
    std::string getForYzc() const;
    
  };
  
} /* namespace wisey */

#endif /* CompilerArguments_h */

