//
//  NegateExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instructions.h>

#include "yazyk/Log.hpp"
#include "yazyk/NegateExpression.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace yazyk;

Value* NegateExpression::generateIR(IRGenerationContext& context) const {
  IType* type = getType(context);
  if (type->getTypeKind() != PRIMITIVE_TYPE || type == PrimitiveTypes::VOID_TYPE) {
    Log::e("Can not apply negate operation to type '" + type->getName() + "'");
    exit(1);
  }
  
  if (type == PrimitiveTypes::FLOAT_TYPE || type == PrimitiveTypes::DOUBLE_TYPE) {
    Value* zero = ConstantFP::get(type->getLLVMType(context.getLLVMContext()), 0);
    return llvm::BinaryOperator::Create(Instruction::FSub,
                                        zero,
                                        mExpression.generateIR(context),
                                        "fsub",
                                        context.getBasicBlock());
 
  }
  
  Value* zero = ConstantInt::get(type->getLLVMType(context.getLLVMContext()), 0);
  return llvm::BinaryOperator::Create(Instruction::Sub,
                                      zero,
                                      mExpression.generateIR(context),
                                      "sub",
                                      context.getBasicBlock());
}

IType* NegateExpression::getType(IRGenerationContext& context) const {
  return mExpression.getType(context);
}
