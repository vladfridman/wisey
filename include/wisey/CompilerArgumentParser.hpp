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
    CompilerArguments parse(std::vector<std::string> argumnets) const;
    
  private:
    
    void printSyntaxAndExit() const;
  };
  
} /* namespace wisey */

#endif /* CompilerArgumentParser_h */

