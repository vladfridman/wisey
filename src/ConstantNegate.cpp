//
//  ConstantNegate.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "ConstantNegate.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ConstantNegate::ConstantNegate(const IConstantExpression* expression, int line) :
mExpression(expression), mLine(line) {
}

ConstantNegate::~ConstantNegate() {
  delete mExpression;
}

int ConstantNegate::getLine() const {
  return mLine;
}

llvm::Constant* ConstantNegate::generateIR(IRGenerationContext& context,
                                           const IType* assignToType) const {
  llvm::Constant* expressionValue = mExpression->generateIR(context, assignToType);
  if (expressionValue->getType()->isIntegerTy()) {
    return ConstantExpr::getNeg(expressionValue);
  }
  ConstantFP* fp = llvm::cast<ConstantFP>(expressionValue);
  llvm::APFloat negated = fp->getValueAPF();
  negated.changeSign();
  return ConstantFP::get(fp->getType()->getContext(), negated);
}

const IType* ConstantNegate::getType(IRGenerationContext& context) const {
  return mExpression->getType(context);
}

bool ConstantNegate::isConstant() const {
  return true;
}

bool ConstantNegate::isAssignable() const {
  return false;
}

void ConstantNegate::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "-";
  mExpression->printToStream(context, stream);
}
