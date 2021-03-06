//
//  Block.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Block_h
#define Block_h

#include <llvm/IR/Instructions.h>

#include "IStatement.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents a block of statements
   */
  class Block : public IStatement {
    StatementList mStatements;
    
  public:
    
    Block();
    
    ~Block();
    
    StatementList& getStatements();
    
    void generateIR(IRGenerationContext& context) const override;
  };
  
} /* namespace wisey */

#endif /* Block_h */

