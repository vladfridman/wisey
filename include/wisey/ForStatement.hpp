//
//  ForStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef ForStatement_h
#define ForStatement_h

#include <llvm/IR/Instructions.h>

#include "wisey/EmptyExpression.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {
  
  /**
   * Represents a for loop statement
   */
  class ForStatement : public IStatement {
    IStatement* mStartStatement;
    IStatement* mConditionStatement;
    IExpression* mIncrementExpression;
    IStatement* mBodyStatement;
    int mLine;
    
  public:
    
    /**
     * For loop statement with start statement, condition, increment expression and body
     * for (int i = 0; i < 10; i ++) { }
     */
    ForStatement(IStatement* startStatement,
                 IStatement* conditionStatement,
                 IExpression* incrementExpression,
                 IStatement* bodyStatement,
                 int line);
    
    ~ForStatement();
    
    /**
     * For loop statement without the increment part
     * for (int i = 0; i < 10;) { i ++ }
     */
    static ForStatement* newWithNoIncrement(IStatement* startStatement,
                                            IStatement* conditionStatement,
                                            IStatement* bodyStatement,
                                            int line);
    
    llvm::Value* generateIR(IRGenerationContext& context) const override;
  };
  
} /* namespace wisey */

#endif /* ForStatement_h */

