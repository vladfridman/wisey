//
//  IncrementExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "yazyk/IncrementExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"

using namespace llvm;
using namespace yazyk;

Value* IncrementExpression::generateIR(IRGenerationContext& context) const {
  Value* originalValue = identifier.generateIR(context);
  Value* increment = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()),
                                      incrementBy,
                                      true);
  
  Value* incrementResult = llvm::BinaryOperator::Create(Instruction::Add,
                                                        originalValue,
                                                        increment,
                                                        variableName,
                                                        context.currentBlock());
  new StoreInst(incrementResult, context.locals()[identifier.getName()], context.currentBlock());
  return isPrefix ? incrementResult : originalValue;
}

