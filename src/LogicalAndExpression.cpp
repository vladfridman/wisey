//
//  LogicalAndExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/IRWriter.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/LogicalAndExpression.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace yazyk;

Value* LogicalAndExpression::generateIR(IRGenerationContext& context) const {
  Value* leftValue = mLeftExpression.generateIR(context);
  BasicBlock* entryBlock = context.getBasicBlock();
  
  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* basicBlockRight = BasicBlock::Create(context.getLLVMContext(), "land.rhs", function);
  BasicBlock* basicBlockEnd = BasicBlock::Create(context.getLLVMContext(), "land.end", function);
  IRWriter::createConditionalBranch(context, basicBlockRight, basicBlockEnd, leftValue);
  
  context.setBasicBlock(basicBlockRight);
  Value* rightValue = mRightExpression.generateIR(context);
  BasicBlock* lastRightBlock = context.getBasicBlock();
  IRWriter::createBranch(context, basicBlockEnd);
  
  context.setBasicBlock(basicBlockEnd);
  Type* type = Type::getInt1Ty(context.getLLVMContext());
  PHINode* phiNode = IRWriter::createPhiNode(context,
                                             type,
                                             "land",
                                             ConstantInt::getFalse(context.getLLVMContext()),
                                             entryBlock,
                                             rightValue,
                                             lastRightBlock);
  
  return phiNode;
}

IType* LogicalAndExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}

void LogicalAndExpression::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a logical AND operation result, it is not a heap pointer");
  exit(1);
}
