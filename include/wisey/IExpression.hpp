//
//  IExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IExpression_h
#define IExpression_h

#include <llvm/IR/Value.h>

#include "wisey/IHasType.hpp"
#include "wisey/IPrintable.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Interface representing a wisey language expression
   */
  class IExpression : public IHasType, public IPrintable {
    
  public:
    
    virtual ~IExpression() { }
    
    /**
     * Generate LLVM Intermediate Reprentation code
     */
    virtual llvm::Value* generateIR(IRGenerationContext& context,
                                    const IType* assignToType) const = 0;
    
    /**
     * Tells whether this expression returns a constant
     */
    virtual bool isConstant() const = 0;
    
    /**
     * Tells whether one can assign to this expression, e.g. variable identifier or array element
     */
    virtual bool isAssignable() const = 0;
    
    /**
     * Tells the line number where this expression is
     */
    virtual int getLine() const = 0;
    
    /**
     * Checks that expression is not undefined
     */
    static void checkForUndefined(IRGenerationContext& context,
                                  const IExpression* expression);
    
  };
  
  /**
   * Represents a list of expressions
   */
  typedef std::vector<const IExpression*> ExpressionList;
  
} /* namespace wisey */


#endif /* IExpression_h */

