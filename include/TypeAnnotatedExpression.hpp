//
//  TypeAnnotatedExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/30/26.
//  Copyright © 2026 Vladimir Fridman. All rights reserved.
//

#ifndef TypeAnnotatedExpression_h
#define TypeAnnotatedExpression_h

#include "IExpression.hpp"
#include "ITypeSpecifier.hpp"

namespace wisey {

  /**
   * Represents a postfix type-annotation: `expr -> Type`.
   *
   * Behaviorally a no-op at runtime — the inner expression's IR is generated
   * unchanged. At compile time the annotated type must equal the inner
   * expression's actual type, exactly. This is an assertion, not a cast:
   * a mismatch is a compile error, not a silent coercion. Use `(Type) expr`
   * for a cast.
   *
   * Motivation: surface the call's return type at every use site so an LLM
   * (or a human reading without an IDE) can verify types locally without
   * resolving the callee. See llm-language-design.md, "Make types visible at
   * every use site."
   */
  class TypeAnnotatedExpression : public IExpression {
    const IExpression* mExpression;
    const ITypeSpecifier* mTypeSpecifier;
    int mLine;

  public:

    TypeAnnotatedExpression(const IExpression* expression,
                            const ITypeSpecifier* typeSpecifier,
                            int line);

    ~TypeAnnotatedExpression();

    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;

    const IType* getType(IRGenerationContext& context) const override;

    bool isConstant() const override;

    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

  };

} /* namespace wisey */

#endif /* TypeAnnotatedExpression_h */
