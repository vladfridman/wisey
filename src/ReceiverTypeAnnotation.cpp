//
//  ReceiverTypeAnnotation.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/3/26.
//  Copyright © 2026 Vladimir Fridman. All rights reserved.
//

#include "IRGenerationContext.hpp"
#include "ReceiverTypeAnnotation.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ReceiverTypeAnnotation::ReceiverTypeAnnotation(const IExpression* expression,
                                               const ITypeSpecifier* typeSpecifier,
                                               int line)
: mExpression(expression), mTypeSpecifier(typeSpecifier), mLine(line) { }

ReceiverTypeAnnotation::~ReceiverTypeAnnotation() {
  delete mExpression;
  delete mTypeSpecifier;
}

int ReceiverTypeAnnotation::getLine() const {
  return mLine;
}

Value* ReceiverTypeAnnotation::generateIR(IRGenerationContext& context,
                                          const IType* assignToType) const {
  IExpression::checkForUndefined(context, mExpression);

  const IType* actualType = mExpression->getType(context);
  const IType* declaredType = mTypeSpecifier->getType(context);

  // Receiver annotations describe the type the reader sees flowing into the
  // following `.method(...)`. Accept exact match or auto-castable, mirroring
  // `expr -> Type`. A mismatch is a compile error pointing the user at a cast.
  if (actualType != declaredType
      && !actualType->canAutoCastTo(context, declaredType)) {
    context.reportError(mLine,
                        "Receiver-type annotation mismatch: receiver has type '" +
                        actualType->getTypeName() +
                        "' but is annotated as '" + declaredType->getTypeName() + "'. " +
                        "Use a cast '(Type) expr' for a deliberate conversion.");
    throw 1;
  }

  Value* innerValue = mExpression->generateIR(context, assignToType);
  if (actualType == declaredType) {
    return innerValue;
  }
  return actualType->castTo(context, innerValue, declaredType, mLine);
}

const IType* ReceiverTypeAnnotation::getType(IRGenerationContext& context) const {
  return mTypeSpecifier->getType(context);
}

bool ReceiverTypeAnnotation::isConstant() const {
  return mExpression->isConstant();
}

bool ReceiverTypeAnnotation::isAssignable() const {
  return mExpression->isAssignable();
}

void ReceiverTypeAnnotation::printToStream(IRGenerationContext& context,
                                           std::iostream& stream) const {
  mExpression->printToStream(context, stream);
  stream << ":";
  mTypeSpecifier->printToStream(context, stream);
}
