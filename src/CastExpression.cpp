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

CastExpression::CastExpression(ITypeSpecifier* typeSpecifier, IExpression* expression)
: mTypeSpecifier(typeSpecifier), mExpression(expression) { }

CastExpression::~CastExpression() {
  delete mTypeSpecifier;
  delete mExpression;
}

IVariable* CastExpression::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value* CastExpression::generateIR(IRGenerationContext& context, IRGenerationFlag flag) const {
  const IType* fromType = mExpression->getType(context);
  Value* fromValue = mExpression->generateIR(context, flag);
  const IType* toType = mTypeSpecifier->getType(context);
  
  return fromType->castTo(context, fromValue, toType);
}

const IType* CastExpression::getType(IRGenerationContext& context) const {
  return mTypeSpecifier->getType(context);
}

bool CastExpression::isConstant() const {
  return false;
}

void CastExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "(";
  mTypeSpecifier->printToStream(context, stream);
  stream << ") ";
  mExpression->printToStream(context, stream);
}
