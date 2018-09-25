//
//  CompoundStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef CompoundStatement_h
#define CompoundStatement_h

#include "Block.hpp"
#include "IRGenerationContext.hpp"
#include "IStatement.hpp"

namespace wisey {
  
  /**
   * Wraps a block in a statement
   */
  class CompoundStatement : public IStatement {
    
    Block* mBlock;
    int mLine;
    
  public:
    
    CompoundStatement(Block* block, int line);
    
    ~CompoundStatement();
    
    void generateIR(IRGenerationContext& context) const override;
  };
  
} /* namespace wisey */

#endif /* CompoundStatement_h */

