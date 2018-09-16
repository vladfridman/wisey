//
//  WiseycArgumentParser.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef WiseycArgumentParser_h
#define WiseycArgumentParser_h

#include "wisey/ICompilerArgumentParser.hpp"

namespace wisey {
  
  /**
   * Used for parsing command line arguments for the wiseyc compiler driver
   */
  class WiseycArgumentParser : public ICompilerArgumentParser {
    
  public:
    
    WiseycArgumentParser();
    
    ~WiseycArgumentParser();
    
    CompilerArguments parse(std::vector<std::string> arguments) const override;
    
  private:
    
    void printSyntaxAndExit() const;
  };
  
} /* namespace wisey */

#endif /* WiseycArgumentParser_h */
