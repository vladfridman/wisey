//
//  YzcArgumentParser.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef YzcArgumentParser_h
#define YzcArgumentParser_h

#include "ICompilerArgumentParser.hpp"

namespace wisey {
  
  /**
   * Used for parsing command line arguments for the yzc compiler
   */
  class YzcArgumentParser : public ICompilerArgumentParser {
    
  public:
    
    YzcArgumentParser();
    
    ~YzcArgumentParser();
    
    CompilerArguments parse(std::vector<std::string> arguments) const override;
    
  private:
    
    void printSyntaxAndExit() const;
  };
  
} /* namespace wisey */

#endif /* YzcArgumentParser_h */
