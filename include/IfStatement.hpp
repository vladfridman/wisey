//
//  IfStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IfStatement_h
#define IfStatement_h

#include <llvm/IR/Instructions.h>

#include "CompoundStatement.hpp"
#include "IExpression.hpp"
#include "IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents an if / else statement
   * if (expression) { statement }
   */
  class IfStatement : public IStatement {
    IExpression* mCondition;
    CompoundStatement* mThenStatement;
    
  public:
    
    IfStatement(IExpression* condition, CompoundStatement* thenStatement);
    
    ~IfStatement();
    
    void generateIR(IRGenerationContext& context) const override;
  };
  
} /* namespace wisey */

#endif /* IfStatement_h */

