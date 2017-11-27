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

Value* BooleanNotExpression::generateIR(IRGenerationContext& context, IRGenerationFlag flag) const {
  assert(flag == IR_GENERATION_NORMAL);
  
  const IType* expressionType = mExpression->getType(context);
  if (expressionType != PrimitiveTypes::BOOLEAN_TYPE) {
    Log::e("Boolean NOT operator '!' can only be applied to boolean types");
    exit(1);
  }
  
  Value* one = ConstantInt::get(Type::getInt1Ty(context.getLLVMContext()), 1);
  Value* expressionValue = mExpression->generateIR(context, flag);
  return IRWriter::createBinaryOperator(context, Instruction::Xor, expressionValue, one, "lnot");
}

const IType* BooleanNotExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}

void BooleanNotExpression::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a boolean NOT expression, it is not a heap pointer");
  exit(1);
}

bool BooleanNotExpression::isConstant() const {
  return false;
}

void BooleanNotExpression::printToStream(IRGenerationContext& context,
                                         std::iostream& stream) const {
  stream << "!";
  mExpression->printToStream(context, stream);
}
