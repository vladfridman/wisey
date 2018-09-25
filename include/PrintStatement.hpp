//
//  PrintStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/27/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef PrintStatement_h
#define PrintStatement_h

#include <llvm/IR/Instructions.h>

#include "IPrintStatement.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents a print statement for printing things to a text stream
   */
  class PrintStatement : public IPrintStatement {
    IExpression* mStreamExpression;
    IExpression* mExpression;
    int mLine;
    
  public:
    
    PrintStatement(IExpression* streamExpression, IExpression* expression, int line);
    
    ~PrintStatement();
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* PrintStatement_h */
