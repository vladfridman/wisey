//
//  TypeAnnotatedExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/30/26.
//  Copyright © 2026 Vladimir Fridman. All rights reserved.
//

#include "IRGenerationContext.hpp"
#include "TypeAnnotatedExpression.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

TypeAnnotatedExpression::TypeAnnotatedExpression(const IExpression* expression,
                                                 const ITypeSpecifier* typeSpecifier,
                                                 int line)
: mExpression(expression), mTypeSpecifier(typeSpecifier), mLine(line) { }

TypeAnnotatedExpression::~TypeAnnotatedExpression() {
  delete mExpression;
  delete mTypeSpecifier;
}

int TypeAnnotatedExpression::getLine() const {
  return mLine;
}

Value* TypeAnnotatedExpression::generateIR(IRGenerationContext& context,
                                           const IType* assignToType) const {
  IExpression::checkForUndefined(context, mExpression);

  const IType* actualType = mExpression->getType(context);
  const IType* declaredType = mTypeSpecifier->getType(context);

  // The annotation describes the value as it flows through this point. We
  // accept the exact type, an auto-castable type (so `int x = char_value -> int`
  // works because char implicitly widens). A mismatch is a compile error;
  // users wanting a deliberate conversion should write `(Type) expr` instead.
  if (actualType != declaredType
      && !actualType->canAutoCastTo(context, declaredType)) {
    context.reportError(mLine,
                        "Type annotation mismatch: expression has type '" +
                        actualType->getTypeName() +
                        "' but is annotated as '" + declaredType->getTypeName() + "'. " +
                        "Use a cast '(Type) expr' for a deliberate conversion.");
    throw 1;
  }

  // Generate the inner expression first. If the actual type differs from the
  // annotated type, perform the auto-cast so downstream code sees a value of
  // the declared type (matches what would happen without the annotation, just
  // typechecker-validated).
  Value* innerValue = mExpression->generateIR(context, assignToType);
  if (actualType == declaredType) {
    return innerValue;
  }
  return actualType->castTo(context, innerValue, declaredType, mLine);
}

const IType* TypeAnnotatedExpression::getType(IRGenerationContext& context) const {
  return mTypeSpecifier->getType(context);
}

bool TypeAnnotatedExpression::isConstant() const {
  return mExpression->isConstant();
}

bool TypeAnnotatedExpression::isAssignable() const {
  return mExpression->isAssignable();
}

void TypeAnnotatedExpression::printToStream(IRGenerationContext& context,
                                            std::iostream& stream) const {
  mExpression->printToStream(context, stream);
  stream << " ~> ";
  mTypeSpecifier->printToStream(context, stream);
}
