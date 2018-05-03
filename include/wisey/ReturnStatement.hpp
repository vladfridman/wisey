//
//  ReturnStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef ReturnStatement_h
#define ReturnStatement_h

#include "wisey/IExpression.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {
  
  /**
   * Represents a return statement that returns an expression
   */
  class ReturnStatement : public IStatement {
    
    IExpression* mExpression;
    int mLine;
    
  public:
    
    ReturnStatement(IExpression* expression, int line);
    
    ~ReturnStatement();
    
    void generateIR(IRGenerationContext& context) const override;
    
    int getLine() const override;

  };  

} /* namespace wisey */

#endif /* ReturnStatement_h */

