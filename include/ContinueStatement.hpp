//
//  ContinueStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/29/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef ContinueStatement_h
#define ContinueStatement_h

#include <llvm/IR/Instructions.h>

#include "IStatement.hpp"

namespace wisey {
  
  /**
   * Represents a continue statement used inside loops
   */
  class ContinueStatement : public IStatement {
    int mLine;
    
  public:
    
    ContinueStatement(int line);
    
    ~ContinueStatement();
    
    void generateIR(IRGenerationContext& context) const override;
  };
  
}

#endif /* ContinueStatement_h */

