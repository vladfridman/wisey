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

#include "wisey/CompoundStatement.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents an if / else statement
   * if (expression) { statement }
   */
  class IfStatement : public IStatement {
    IExpression* mCondition;
    CompoundStatement* mThenStatement;
    int mLine;
    
  public:
    
    IfStatement(IExpression* condition, CompoundStatement* thenStatement, int line);
    
    ~IfStatement();
    
    void generateIR(IRGenerationContext& context) const override;
    
    int getLine() const override;

  };

} /* namespace wisey */

#endif /* IfStatement_h */

