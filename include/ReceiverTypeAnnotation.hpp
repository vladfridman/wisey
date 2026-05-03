//
//  ReceiverTypeAnnotation.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/3/26.
//  Copyright © 2026 Vladimir Fridman. All rights reserved.
//

#ifndef ReceiverTypeAnnotation_h
#define ReceiverTypeAnnotation_h

#include "IExpression.hpp"
#include "ITypeSpecifier.hpp"

namespace wisey {

  /**
   * Represents a receiver-type annotation: `receiver:Type` immediately before
   * a `.method(...)` postfix. Built only from the grammar rule
   * `postfix_expression ':' type_specifier '.' TIDENTIFIER` (wrapped inside
   * the resulting IdentifierChain), so it never appears as a standalone
   * expression.
   *
   * Behaviorally a no-op at runtime — the inner expression's IR is generated
   * unchanged. At compile time the annotated type must equal the inner
   * expression's actual type, exactly, or be an auto-cast target. This is an
   * assertion, not a cast: a mismatch is a compile error pointing the user at
   * `(Type) expr` if a deliberate conversion is wanted.
   *
   * Motivation: surface the call's receiver type at every call site so an LLM
   * (or a human reading without an IDE) can verify the method exists without
   * resolving the receiver. Sibling proposal to `expr -> Type`. See
   * llm-language-design.md, "Kill cross-file ambiguity."
   */
  class ReceiverTypeAnnotation : public IExpression {
    const IExpression* mExpression;
    const ITypeSpecifier* mTypeSpecifier;
    int mLine;

  public:

    ReceiverTypeAnnotation(const IExpression* expression,
                           const ITypeSpecifier* typeSpecifier,
                           int line);

    ~ReceiverTypeAnnotation();

    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;

    const IType* getType(IRGenerationContext& context) const override;

    bool isConstant() const override;

    bool isAssignable() const override;

    bool isReceiverAnnotation() const override { return true; }

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

  };

} /* namespace wisey */

#endif /* ReceiverTypeAnnotation_h */
