//
//  CastExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <sstream>

#include "wisey/AutoCast.hpp"
#include "wisey/CastExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

CastExpression::CastExpression(const ITypeSpecifier* typeSpecifier,
                               const IExpression* expression,
                               int line)
: mTypeSpecifier(typeSpecifier), mExpression(expression), mLine(line) { }

CastExpression::~CastExpression() {
  delete mTypeSpecifier;
  delete mExpression;
}

int CastExpression::getLine() const {
  return mLine;
}

Value* CastExpression::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  const IType* fromType = mExpression->getType(context);
  Value* fromValue = mExpression->generateIR(context, assignToType);
  const IType* toType = mTypeSpecifier->getType(context);
  if (!fromType->canCastTo(context, toType)) {
    context.reportError(mLine, "Can not cast from type " + fromType->getTypeName() + " to type " +
                        toType->getTypeName());
    throw 1;
  }

  return fromType->castTo(context, fromValue, toType, mLine);
}

const IType* CastExpression::getType(IRGenerationContext& context) const {
  return mTypeSpecifier->getType(context);
}

bool CastExpression::isConstant() const {
  return false;
}

bool CastExpression::isAssignable() const {
  return false;
}

void CastExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "(";
  mTypeSpecifier->printToStream(context, stream);
  stream << ") ";
  mExpression->printToStream(context, stream);
}
