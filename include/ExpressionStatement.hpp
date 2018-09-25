//
//  ExpressionStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/19/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef ExpressionStatement_h
#define ExpressionStatement_h

#include "IExpression.hpp"
#include "IStatement.hpp"

namespace wisey {
  
  /**
   * Represents a statement
   */
  class ExpressionStatement : public IStatement {
    IExpression* mExpression;
    int mLine;
    
  public:
    
    ExpressionStatement(IExpression* expression, int line);
    
    ~ExpressionStatement();
    
    void generateIR(IRGenerationContext& context) const override;
  };
  
} /* namespace wisey */

#endif /* ExpressionStatement_h */

