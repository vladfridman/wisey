//
//  CompilerDriver.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/11/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef CompilerDriver_h
#define CompilerDriver_h

#include "Compiler.hpp"

namespace wisey {
  
  /**
   * Calls commands on wisey compiler and linker to produce executables or library files
   */
  class CompilerDriver {
    std::string mWiseyHome;
    std::string mWiseyHeaders;
    std::string mWiseyLibDir;

  public:
    
    CompilerDriver();
    
    ~CompilerDriver();
    
    /**
     * Compile source files into an executable
     */
    void compileRunnable(int argc, char **argv);
    
    /**
     * Compile source files into an executable
     */
    void compileLibrary(int argc, char **argv);

  private:
    
    bool prepareForRunnable();
    
    bool prepareForLibrary();
    
    bool checkFileExists(char const* fileName) const;
    
    bool checkCommandExists(std::string check) const;

    int executeCommand(std::string command) const;

    bool checkYzcInstall() const;
    
  };
  
} /* namespace wisey */

#endif /* CompilerDriver_h */
