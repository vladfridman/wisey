//
//  ReturnVoidStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef ReturnVoidStatement_h
#define ReturnVoidStatement_h

#include "wisey/IStatement.hpp"

namespace wisey {
  
  /**
   * Represents a return that does not return any expression
   */
  class ReturnVoidStatement : public IStatement {
    
    int mLine;
    
  public:
    
    ReturnVoidStatement(int line);
    
    ~ReturnVoidStatement();
    
    void generateIR(IRGenerationContext& context) const override;
    
    int getLine() const override;

  };  

} /* namespace wisey */

#endif /* ReturnVoidStatement_h */

