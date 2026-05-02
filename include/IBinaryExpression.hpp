//
//  IBinaryExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IBinaryExpression_h
#define IBinaryExpression_h

#include "IExpression.hpp"

namespace wisey {
  /**
   * Represents an expression with two parts: left and right, such as a + b or a += b
   */
  class IBinaryExpression : public IExpression {
    
  public:
    
    /**
     * Get the left part of the binary expression
     */
    virtual const IExpression* getLeft() const = 0;
    
    /**
     * Get the right part of the binary expression
     */
    virtual const IExpression* getRight() const = 0;
    
    /**
     * Get the string representation of the binary operation
     */
    virtual std::string getOperation() const = 0;
    
    static void printToStream(IRGenerationContext& context,
                              std::iostream& stream,
                              const IBinaryExpression* expression);

    /**
     * Reports an error if either side of a binary expression is an
     * unannotated call. The visible-types pilot requires both operands
     * of `a + foo()`, `a < foo()`, `a && foo()` etc. to be wrapped in
     * `expr -> Type` so the type at the use site is local.
     */
    static void requireAnnotatedOperands(IRGenerationContext& context,
                                         const IExpression* left,
                                         const IExpression* right,
                                         const char* description);

  };

} /* namespace wisey */



#endif /* IBinaryExpression_h */
