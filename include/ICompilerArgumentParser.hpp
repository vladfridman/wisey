//
//  ICompilerArgumentParser.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ICompilerArgumentParser_h
#define ICompilerArgumentParser_h

#include "CompilerArguments.hpp"

namespace wisey {
  
  /**
   * Represents compiler argument parser that is different depending on the compiler driver
   */
  class ICompilerArgumentParser {
    
  public:
    
    virtual ~ICompilerArgumentParser() { }
    
    /**
     * Parse given command line arguments
     */
    virtual CompilerArguments parse(std::vector<std::string> argumnets) const = 0;
    
  };
  
} /* namespace wisey */

#endif /* ICompilerArgumentParser_h */
