//
//  IArithmeticExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IArithmeticExpression_h
#define IArithmeticExpression_h

#include "wisey/IBinaryExpression.hpp"

namespace wisey {
  
  /**
   * This is used to fake an expression that returns a given value and cleans up its type
   */
  class IArithmeticExpression : public IBinaryExpression {
    
  public:
    
    ~IArithmeticExpression() {}
    
    /**
     * Checks that types are compatable for an arithmetic expression
     */
    static void checkTypes(IRGenerationContext& context,
                           const IExpression* leftExpression,
                           const IExpression* rightExpression,
                           int operation,
                           int line);
    
  private:
    
    static bool isPointerArithmetic(const IType* left,
                                    const IType* right,
                                    int operation);

  };
  
} /* namespace wisey */

#endif /* IArithmeticExpression_h */
