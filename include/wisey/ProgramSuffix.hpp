//
//  ProgramSuffix.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {
  
  /**
   * Things that need to be run after the parsed program code
   */
  class ProgramSuffix : public IStatement {
    
  public:
    
    void generateIR(IRGenerationContext& context) const override;
    
  private:
    
    void generateMain(IRGenerationContext& context) const;

  };
  
} /* namespace wisey */

