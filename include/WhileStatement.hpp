//
//  WhileStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef WhileStatement_h
#define WhileStatement_h

#include <llvm/IR/Instructions.h>

#include "IExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IStatement.hpp"

namespace wisey {
  
  /**
   * Represents a while loop statement
   */
  class WhileStatement : public IStatement {
    IExpression* mConditionExpression;
    IStatement* mStatement;
    
  public:
    
    WhileStatement(IExpression* conditionExpression, IStatement* statement);
    
    ~WhileStatement();
    
    void generateIR(IRGenerationContext& context) const override;
  };
  
} /* namespace wisey */

#endif /* WhileStatement_h */

