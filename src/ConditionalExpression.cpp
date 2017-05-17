//
//  ConditionalExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/ConditionalExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Value *ConditionalExpression::generateIR(IRGenerationContext& context) const {
  checkTypes(context);
  
  Value* conditionValue = mConditionExpression.generateIR(context);
  
  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* blockCondTrue = BasicBlock::Create(context.getLLVMContext(), "cond.true", function);
  BasicBlock* blockCondFalse = BasicBlock::Create(context.getLLVMContext(), "cond.false", function);
  BasicBlock* blockCondEnd = BasicBlock::Create(context.getLLVMContext(), "cond.end", function);
  IRWriter::createConditionalBranch(context, blockCondTrue, blockCondFalse, conditionValue);
  
  context.setBasicBlock(blockCondTrue);
  Value* condTrueValue = mConditionTrueExpression.generateIR(context);
  Type* condTrueResultType = condTrueValue->getType();
  BasicBlock* lastBlock = context.getBasicBlock();
  IRWriter::createBranch(context, blockCondEnd);

  context.setBasicBlock(blockCondFalse);
  Value* condFalseValue = mConditionFalseExpression.generateIR(context);
  Type* condFalseResultType = condTrueValue->getType();
  lastBlock = context.getBasicBlock();
  IRWriter::createBranch(context, blockCondEnd);

  if (condTrueResultType != condFalseResultType) {
    Log::e("Results of different type in a conditional expresion!");
  }

  context.setBasicBlock(blockCondEnd);
  PHINode* phiNode = IRWriter::createPhiNode(context,
                                             condTrueResultType,
                                             "cond",
                                             condTrueValue,
                                             blockCondTrue,
                                             condFalseValue,
                                             blockCondFalse);
  
  return phiNode;
}

IType* ConditionalExpression::getType(IRGenerationContext& context) const {
  return mConditionTrueExpression.getType(context);
}

void ConditionalExpression::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a conditional expression result, it is not a heap pointer");
  exit(1);
}

// TODO: implement a more sensible type checking/casting
void ConditionalExpression::checkTypes(IRGenerationContext& context) const {
  IType* trueExpressionType = mConditionTrueExpression.getType(context);
  IType* falseExpressionType = mConditionFalseExpression.getType(context);
  
  if (mConditionExpression.getType(context) != PrimitiveTypes::BOOLEAN_TYPE) {
    Log::e("Condition in a conditional expression is not of type BOOLEAN");
    exit(1);
  }
  
  if (trueExpressionType != falseExpressionType) {
    Log::e("Incopatible types in conditional expression operation");
    exit(1);
  }
  
  if (trueExpressionType == PrimitiveTypes::VOID_TYPE) {
    Log::e("Can not use expressions of type VOID in a conditional expression");
    exit(1);
  }
}
