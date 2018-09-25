//
//  ThreadExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ThreadExpression_h
#define ThreadExpression_h

#include "IExpressionAssignable.hpp"
#include "IVariable.hpp"

namespace wisey {
  
  /**
   * Represents the current thread in a wisey program.
   */
  class ThreadExpression {

  public:
    
    /**
     * Variable name referencing the current thread
     */
    static std::string THREAD;
    
    /**
     * Variable name referencing current thread's call stack
     */
    static std::string CALL_STACK;
    
  };
  
} /* namespace wisey */

#endif /* ThreadExpression_h */

