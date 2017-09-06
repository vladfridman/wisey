//
//  LogicalOrExpression.cpp
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
#include "wisey/LogicalOrExpression.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

LogicalOrExpression::~LogicalOrExpression() {
  delete mLeftExpression;
  delete mRightExpression;
}

IVariable* LogicalOrExpression::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value* LogicalOrExpression::generateIR(IRGenerationContext& context) const {
  Value* leftValue = mLeftExpression->generateIR(context);
  BasicBlock* entryBlock = context.getBasicBlock();
  
  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* basicBlockRight = BasicBlock::Create(context.getLLVMContext(), "lor.rhs", function);
  BasicBlock* basicBlockEnd = BasicBlock::Create(context.getLLVMContext(), "lor.end", function);
  IRWriter::createConditionalBranch(context, basicBlockEnd, basicBlockRight, leftValue);
  
  context.setBasicBlock(basicBlockRight);
  Value* rightValue = mRightExpression->generateIR(context);
  BasicBlock* lastRightBlock = context.getBasicBlock();
  IRWriter::createBranch(context, basicBlockEnd);
  
  context.setBasicBlock(basicBlockEnd);
  Type* type = Type::getInt1Ty(context.getLLVMContext());
  PHINode* phiNode = IRWriter::createPhiNode(context,
                                             type,
                                             "lor",
                                             ConstantInt::getTrue(context.getLLVMContext()),
                                             entryBlock,
                                             rightValue,
                                             lastRightBlock);
  
  return phiNode;
}

const IType* LogicalOrExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}

void LogicalOrExpression::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a logical OR operation result, it is not a heap pointer");
  exit(1);
}

void LogicalOrExpression::addReferenceToOwner(IRGenerationContext& context,
                                               IVariable* reference) const {
  Log::e("Can not add a reference to non owner type logical OR operation expression");
  exit(1);
}

bool LogicalOrExpression::existsInOuterScope(IRGenerationContext& context) const {
  return false;
}
