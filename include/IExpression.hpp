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
     * Returns the expression on the LHS of a dot, when this expression is a
     * dotted form like `a.b` (IdentifierChain). For non-dotted expressions
     * the default returns nullptr. Used by the chain-depth guardrail to
     * walk back through `obj.foo().bar()` chains.
     */
    virtual const IExpression* peelDotReceiver() const { return nullptr; }

    /**
     * Returns the receiver expression of a call (the part to the left of the
     * `(`). For `a.b()` this is the `a.b` IdentifierChain; for `foo()` it is
     * the `foo` Identifier. Default returns nullptr; overridden by call
     * classes that want to participate in chain-depth checks.
     */
    virtual const IExpression* getCallReceiver() const { return nullptr; }

    /**
     * Reports an error and throws if `expression` is a call expression that
     * isn't wrapped in `expr -> Type`. Called from each consumer site that
     * requires an annotated value.
     */
    static void requireAnnotated(IRGenerationContext& context,
                                 const IExpression* expression,
                                 const char* consumerDescription);

    /**
     * Counts how many consecutive method calls form a postfix chain, starting
     * at `methodCall`. `a.b()` is depth 1; `a.b().c()` is 2; `a.b().c().d()`
     * is 3. Walks `getCallReceiver()` then `peelDotReceiver()` until the
     * chain breaks. Builder chains parse as dedicated AST nodes
     * (HeapBuilder/PoolBuilder/Injector) so they don't appear in this walk.
     */
    static int countMethodChainDepth(const IExpression* methodCall);

    /**
     * Reports an error and throws if `methodCall` is the head of a chain of
     * 3 or more method calls. The `expr -> Type` annotation can't attach
     * mid-chain (postfix-only), so deep chains hide the per-step types from
     * a reader. Forces the user to break the chain into named locals.
     */
    static void requireShortMethodChain(IRGenerationContext& context,
                                        const IExpression* methodCall);

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

