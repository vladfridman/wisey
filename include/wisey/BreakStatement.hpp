//
//  BreakStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/25/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef BreakStatement_h
#define BreakStatement_h

#include <llvm/IR/Instructions.h>

#include "wisey/IStatement.hpp"

namespace wisey {
  
  /**
   * Represents a break statement used inside loops and switch statements
   */
  class BreakStatement : public IStatement {
    int mLine;
    
  public:
    
    BreakStatement(int line);
    
    ~BreakStatement();
    
    void generateIR(IRGenerationContext& context) const override;
    
    int getLine() const override;

  };

}

#endif /* BreakStatement_h */

