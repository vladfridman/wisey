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
#include "yazyk/Log.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace yazyk;

Value* IncrementExpression::generateIR(IRGenerationContext& context) const {
  IType* identifierType = mIdentifier.getType(context);
  if (identifierType != PrimitiveTypes::INT_TYPE &&
      identifierType != PrimitiveTypes::CHAR_TYPE &&
      identifierType != PrimitiveTypes::LONG_TYPE) {
    Log::e("Identifier " + mIdentifier.getName() +
           " is of a type that is incopatible with increment/decrement operation");
    exit(1);
  }
  
  Value* originalValue = mIdentifier.generateIR(context);
  Value* increment = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()),
                                      mIncrementBy,
                                      true);
  
  Value* incrementResult = llvm::BinaryOperator::Create(Instruction::Add,
                                                        originalValue,
                                                        increment,
                                                        mVariableName,
                                                        context.getBasicBlock());
  new StoreInst(incrementResult,
                context.getScopes().getVariable(mIdentifier.getName())->getValue(),
                context.getBasicBlock());
  return mIsPrefix ? incrementResult : originalValue;
}

IType* IncrementExpression::getType(IRGenerationContext& context) const {
  return mIdentifier.getType(context);
}
