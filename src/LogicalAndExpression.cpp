//
//  LogicalAndExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/LogicalAndExpression.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

LogicalAndExpression::~LogicalAndExpression() {
  delete mLeftExpression;
  delete mRightExpression;
}

IVariable* LogicalAndExpression::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value* LogicalAndExpression::generateIR(IRGenerationContext& context, IRGenerationFlag flag) const {
  assert(flag == IR_GENERATION_NORMAL);
  
  Value* leftValue = mLeftExpression->generateIR(context, flag);
  BasicBlock* entryBlock = context.getBasicBlock();
  
  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* basicBlockRight = BasicBlock::Create(context.getLLVMContext(), "land.rhs", function);
  BasicBlock* basicBlockEnd = BasicBlock::Create(context.getLLVMContext(), "land.end", function);
  IRWriter::createConditionalBranch(context, basicBlockRight, basicBlockEnd, leftValue);
  
  context.setBasicBlock(basicBlockRight);
  Value* rightValue = mRightExpression->generateIR(context, flag);
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

const IType* LogicalAndExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}

void LogicalAndExpression::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a logical AND operation result, it is not a heap pointer");
  exit(1);
}

bool LogicalAndExpression::isConstant() const {
  return false;
}

void LogicalAndExpression::printToStream(IRGenerationContext& context,
                                         std::iostream& stream) const {
  mLeftExpression->printToStream(context, stream);
  stream << " && ";
  mRightExpression->printToStream(context, stream);
}
