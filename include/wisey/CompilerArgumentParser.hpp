//
//  CompilerArgumentParser.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef CompilerArgumentParser_h
#define CompilerArgumentParser_h

#include "wisey/CompilerArguments.hpp"

namespace wisey {
  
  typedef enum CompilerModeEnum { YZC, WISEYC, WISEYLIBC } CompilerMode;

  /**
   * Used for parsing command line arguments for the Wisey compiler
   */
  class CompilerArgumentParser {
    
  public:
    
    CompilerArgumentParser() { }
    
    ~CompilerArgumentParser() { }
    
    /**
     * Parse given command line arguments
     */
    CompilerArguments parse(std::vector<std::string> argumnets, CompilerMode compilerMode) const;
    
  private:
    
    void printSyntaxAndExit(CompilerMode compilerMode) const;
  };
  
} /* namespace wisey */

#endif /* CompilerArgumentParser_h */

