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

#include "wisey/Block.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents a default CASE in a SWITCH statement
   */
  class DefaultCaseStatement : public IStatement {
    
    Block* mBlock;
    
  public:
    
    DefaultCaseStatement(Block* block);
    
    ~DefaultCaseStatement();
    
    llvm::Value* generateIR(IRGenerationContext& context) const override;
  };
  
} /* namespace wisey */

#endif /* DefaultCaseStatement_h */

