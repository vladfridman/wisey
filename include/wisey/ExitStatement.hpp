//
//  ExitStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExitStatement_h
#define ExitStatement_h

#include <llvm/IR/Instructions.h>

#include "wisey/IExpression.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents an exit statement that ends the program
   */
  class ExitStatement : public IStatement {
    IExpression* mExpression;
    int mLine;
    
  public:
    
    ExitStatement(IExpression* expression, int line);
    
    ~ExitStatement();
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* ExitStatement_h */

