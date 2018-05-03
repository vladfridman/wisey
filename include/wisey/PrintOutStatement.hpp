//
//  PrintOutStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef PrintOutStatement_h
#define PrintOutStatement_h

#include <llvm/IR/Instructions.h>

#include "wisey/IPrintStatement.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents a printout statement for printing things to stdout stream
   */
  class PrintOutStatement : public IPrintStatement {
    IExpression* mExpression;
    int mLine;
    
  public:
    
    PrintOutStatement(IExpression* expression, int line);
    
    ~PrintOutStatement();
    
    /**
     * Prints a given list of expressions each of which should have primitive type
     */
    static void printExpressionList(IRGenerationContext& context,
                                    ExpressionList expressionList,
                                    int line);
    
    void generateIR(IRGenerationContext& context) const override;
    
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* PrintOutStatement_h */

