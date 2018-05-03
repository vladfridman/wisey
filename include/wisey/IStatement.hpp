//
//  IStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IStatement_h
#define IStatement_h

#include <llvm/IR/Value.h>

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Interface representing a wisey language statement
   */
  class IStatement {
    
  public:
    
    virtual ~IStatement() { }
    
    /**
     * Generate LLVM Intermediate Reprentation code
     */
    virtual void generateIR(IRGenerationContext& context) const = 0;
    
    /**
     * Return the line where this statement is
     */
    virtual int getLine() const = 0;
    
  };
  
  /**
   * Represents a list of statements
   */
  typedef std::vector<IStatement*> StatementList;
  
} /* namespace wisey */

#endif /* IStatement_h */

