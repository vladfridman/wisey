//
//  PrintErrStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef PrintErrStatement_h
#define PrintErrStatement_h

#include <llvm/IR/Instructions.h>

#include "IPrintStatement.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents a printout statement for printing things to stdout stream
   */
  class PrintErrStatement : public IPrintStatement {
    IExpression* mExpression;
    int mLine;
    
  public:
    
    PrintErrStatement(IExpression* expression, int line);
    
    ~PrintErrStatement();
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* PrintErrStatement_h */

