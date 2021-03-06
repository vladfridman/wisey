//
//  BooleanNotExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instructions.h>

#include "AutoCast.hpp"
#include "BooleanNotExpression.hpp"
#include "IRWriter.hpp"
#include "Log.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

BooleanNotExpression::BooleanNotExpression(const IExpression* expression, int line) :
mExpression(expression), mLine(line) { }

BooleanNotExpression::~BooleanNotExpression() {
  delete mExpression;
}

int BooleanNotExpression::getLine() const {
  return mLine;
}

Value* BooleanNotExpression::generateIR(IRGenerationContext& context,
                                        const IType* assignToType) const {
  IExpression::checkForUndefined(context, mExpression);

  Value* expressionValue = mExpression->generateIR(context, PrimitiveTypes::VOID);
  Value* expressionValueCast = AutoCast::maybeCast(context,
                                                   mExpression->getType(context),
                                                   expressionValue,
                                                   PrimitiveTypes::BOOLEAN,
                                                   mExpression->getLine());
  
  Value* one = ConstantInt::get(Type::getInt1Ty(context.getLLVMContext()), 1);
  return IRWriter::createBinaryOperator(context, Instruction::Xor, expressionValueCast, one, "");
}

const IType* BooleanNotExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN;
}

bool BooleanNotExpression::isConstant() const {
  return false;
}

bool BooleanNotExpression::isAssignable() const {
  return false;
}

void BooleanNotExpression::printToStream(IRGenerationContext& context,
                                         std::iostream& stream) const {
  stream << "!";
  mExpression->printToStream(context, stream);
}
