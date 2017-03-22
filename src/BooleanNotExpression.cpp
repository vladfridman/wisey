//
//  BooleanNotExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instructions.h>

#include "yazyk/BooleanNotExpression.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace yazyk;

Value* BooleanNotExpression::generateIR(IRGenerationContext& context) const {
  IType* expressionType = mExpression.getType(context);
  if (expressionType != PrimitiveTypes::BOOLEAN_TYPE) {
    Log::e("Boolean NOT operator '!' can only be applied to boolean types");
    exit(1);
  }
  
  Value* one = ConstantInt::get(Type::getInt1Ty(context.getLLVMContext()), 1);
  return llvm::BinaryOperator::Create(Instruction::Xor,
                                      mExpression.generateIR(context),
                                      one,
                                      "lnot",
                                      context.getBasicBlock());
}

IType* BooleanNotExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}
