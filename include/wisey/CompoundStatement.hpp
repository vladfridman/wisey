//
//  CompoundStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef CompoundStatement_h
#define CompoundStatement_h

#include "wisey/Block.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IStatement.hpp"

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
    
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* CompoundStatement_h */

