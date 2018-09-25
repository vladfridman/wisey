//
//  IExpressionAssignable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IExpressionAssignable_h
#define IExpressionAssignable_h

#include <llvm/IR/Value.h>

#include "IExpression.hpp"
#include "IPrintable.hpp"
#include "IVariable.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents an assignable expression, e.g. variable identifier or an array element
   */
  class IExpressionAssignable : public IExpression {
    
  public:
    
    /**
     * Returns the variable that this assignable expression resolves to
     */
    virtual IVariable* getVariable(IRGenerationContext& context,
                                   std::vector<const IExpression*>& arrayIndices) const = 0;
    
  };
  
} /* namespace wisey */

#endif /* IExpressionAssignable_h */
