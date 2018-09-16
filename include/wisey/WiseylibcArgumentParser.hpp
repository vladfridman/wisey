//
//  WiseylibcArgumentParser.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef WiseylibcArgumentParser_h
#define WiseylibcArgumentParser_h

#include "wisey/ICompilerArgumentParser.hpp"

namespace wisey {
  
  /**
   * Used for parsing command line arguments for the wiseylibc compiler driver
   */
  class WiseylibcArgumentParser : public ICompilerArgumentParser {
    
  public:
    
    WiseylibcArgumentParser();
    
    ~WiseylibcArgumentParser();
    
    CompilerArguments parse(std::vector<std::string> arguments) const override;
    
  private:
    
    void printSyntaxAndExit() const;
  };
  
} /* namespace wisey */

#endif /* WiseylibcArgumentParser_h */
