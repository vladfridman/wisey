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

#include "wisey/BooleanNotExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

BooleanNotExpression::~BooleanNotExpression() {
  delete mExpression;
}

IVariable* BooleanNotExpression::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value* BooleanNotExpression::generateIR(IRGenerationContext& context) const {
  const IType* expressionType = mExpression->getType(context);
  if (expressionType != PrimitiveTypes::BOOLEAN_TYPE) {
    Log::e("Boolean NOT operator '!' can only be applied to boolean types");
    exit(1);
  }
  
  Value* one = ConstantInt::get(Type::getInt1Ty(context.getLLVMContext()), 1);
  Value* expressionValue = mExpression->generateIR(context);
  return IRWriter::createBinaryOperator(context, Instruction::Xor, expressionValue, one, "lnot");
}

const IType* BooleanNotExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}

void BooleanNotExpression::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a boolean NOT expression, it is not a heap pointer");
  exit(1);
}

void BooleanNotExpression::addReferenceToOwner(IRGenerationContext& context,
                                          IVariable* reference) const {
  Log::e("Can not add a reference to non owner type boolean NOT expression");
  exit(1);
}

bool BooleanNotExpression::existsInOuterScope(IRGenerationContext& context) const {
  return false;
}

bool BooleanNotExpression::isConstant() const {
  return false;
}

void BooleanNotExpression::printToStream(IRGenerationContext& context,
                                         std::iostream& stream) const {
  stream << "!";
  mExpression->printToStream(context, stream);
}
