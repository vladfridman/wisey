//
//  PrintFileStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef PrintFileStatement_h
#define PrintFileStatement_h

#include <llvm/IR/Instructions.h>

#include "wisey/IPrintStatement.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents a printfile statement for printing things to a file
   */
  class PrintFileStatement : public IPrintStatement {
    IExpression* mFileExpression;
    IExpression* mExpression;
    int mLine;
    
  public:
    
    PrintFileStatement(IExpression* fileExpression, IExpression* expression, int line);

    ~PrintFileStatement();
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* PrintFileStatement_h */
