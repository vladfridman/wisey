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

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {
  
  /**
   * Represents a while loop statement
   */
  class DoStatement : public IStatement {
    IStatement* mStatement;
    IExpression* mConditionExpression;
    int mLine;
    
  public:
    
    DoStatement(IStatement* statement, IExpression* conditionExpression, int line);
    
    ~DoStatement();
    
    void generateIR(IRGenerationContext& context) const override;
    
    int getLine() const override;

  };

} /* namespace wisey */

#endif /* DoStatement_h */

