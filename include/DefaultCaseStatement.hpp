//
//  DefaultCaseStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef DefaultCaseStatement_h
#define DefaultCaseStatement_h

#include <llvm/IR/Constants.h>

#include "Block.hpp"
#include "IStatement.hpp"
#include "IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents a default CASE in a SWITCH statement
   */
  class DefaultCaseStatement : public IStatement {
    
    Block* mBlock;
    
  public:
    
    DefaultCaseStatement(Block* block);
    
    ~DefaultCaseStatement();
    
    void generateIR(IRGenerationContext& context) const override;
  };
  
} /* namespace wisey */

#endif /* DefaultCaseStatement_h */

