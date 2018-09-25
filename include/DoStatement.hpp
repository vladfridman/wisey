//
//  DoStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef DoWhileStatement_h
#define DoWhileStatement_h

#include <llvm/IR/Instructions.h>

#include "IExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IStatement.hpp"

namespace wisey {
  
  /**
   * Represents a while loop statement
   */
  class DoStatement : public IStatement {
    IStatement* mStatement;
    IExpression* mConditionExpression;
    
  public:
    
    DoStatement(IStatement* statement, IExpression* conditionExpression);
    
    ~DoStatement();
    
    void generateIR(IRGenerationContext& context) const override;
  };
  
} /* namespace wisey */

#endif /* DoStatement_h */

