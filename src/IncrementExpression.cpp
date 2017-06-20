//
//  IncrementExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/IncrementExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

IncrementExpression::~IncrementExpression() {
  delete mIdentifier;
}

Value* IncrementExpression::generateIR(IRGenerationContext& context) const {
  const IType* identifierType = mIdentifier->getType(context);
  if (identifierType != PrimitiveTypes::INT_TYPE &&
      identifierType != PrimitiveTypes::CHAR_TYPE &&
      identifierType != PrimitiveTypes::LONG_TYPE) {
    Log::e("Identifier " + mIdentifier->getName() +
           " is of a type that is incopatible with increment/decrement operation");
    exit(1);
  }
  
  Value* originalValue = mIdentifier->generateIR(context);
  Value* increment = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()),
                                      mIncrementBy,
                                      true);
  
  Value* incrementResult = IRWriter::createBinaryOperator(context,
                                                          Instruction::Add,
                                                          originalValue,
                                                          increment,
                                                          mVariableName);

  IRWriter::newStoreInst(context,
                         incrementResult,
                         context.getScopes().getVariable(mIdentifier->getName())->getValue());
  return mIsPrefix ? incrementResult : originalValue;
}

const IType* IncrementExpression::getType(IRGenerationContext& context) const {
  return mIdentifier->getType(context);
}

void IncrementExpression::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of an increment operation result, it is not a heap pointer");
  exit(1);
}

bool IncrementExpression::existsInOuterScope(IRGenerationContext& context) const {
  return false;
}
