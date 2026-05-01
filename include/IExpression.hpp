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

#include "IHasType.hpp"
#include "IPrintable.hpp"
#include "IVariable.hpp"

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
     * Tells whether this expression IS a call expression — method call, static
     * method call, llvm function call, builder, injector, etc.
     *
     * Used by the "annotation required" rule: at every consumer site (return,
     * assignment, argument, etc.) the operand must NOT be an unwrapped call.
     * It must either be a non-call (literal, identifier, operator) or a
     * `TypeAnnotatedExpression` wrapping the call. The wrapped form returns
     * false here, so a consumer-side check that rejects `isCallExpression()`
     * naturally accepts annotated calls.
     *
     * Default: false. Overridden by call-like classes to return true.
     */
    virtual bool isCallExpression() const { return false; }

    /**
     * Reports an error and throws if `expression` is a call expression that
     * isn't wrapped in `expr -> Type`. Called from each consumer site that
     * requires an annotated value.
     */
    static void requireAnnotated(IRGenerationContext& context,
                                 const IExpression* expression,
                                 const char* consumerDescription);

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

