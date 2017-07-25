//
//  NegateExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instructions.h>

#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/NegateExpression.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

NegateExpression::~NegateExpression() {
  delete mExpression;
}

Value* NegateExpression::generateIR(IRGenerationContext& context) const {
  const IType* type = getType(context);
  if (type->getTypeKind() != PRIMITIVE_TYPE || type == PrimitiveTypes::VOID_TYPE) {
    Log::e("Can not apply negate operation to type '" + type->getName() + "'");
    exit(1);
  }
  
  if (type == PrimitiveTypes::FLOAT_TYPE || type == PrimitiveTypes::DOUBLE_TYPE) {
    Value* zero = ConstantFP::get(type->getLLVMType(context.getLLVMContext()), 0);
    return IRWriter::createBinaryOperator(context,
                                          Instruction::FSub,
                                          zero,
                                          mExpression->generateIR(context),
                                          "fsub");
  }
  
  Value* zero = ConstantInt::get(type->getLLVMType(context.getLLVMContext()), 0);
  return IRWriter::createBinaryOperator(context,
                                        Instruction::Sub,
                                        zero,
                                        mExpression->generateIR(context),
                                        "sub");
}

const IType* NegateExpression::getType(IRGenerationContext& context) const {
  return mExpression->getType(context);
}

void NegateExpression::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a negate expression result, it is not a heap pointer");
  exit(1);
}

void NegateExpression::addReferenceToOwner(IRGenerationContext& context,
                                           IVariable* reference) const {
  Log::e("Can not add a reference to non owner type negate expression");
  exit(1);
}

bool NegateExpression::existsInOuterScope(IRGenerationContext& context) const {
  return false;
}
