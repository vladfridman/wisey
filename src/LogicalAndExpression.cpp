//
//  LogicalAndExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>

#include "AutoCast.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "Log.hpp"
#include "LogicalAndExpression.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LogicalAndExpression::LogicalAndExpression(const IExpression* leftExpression,
                                           const IExpression* rightExpression,
                                           int line) :
mLeftExpression(leftExpression),
mRightExpression(rightExpression),
mLine(line) { }

LogicalAndExpression::~LogicalAndExpression() {
  delete mLeftExpression;
  delete mRightExpression;
}

int LogicalAndExpression::getLine() const {
  return mLine;
}

Value* LogicalAndExpression::generateIR(IRGenerationContext& context,
                                        const IType* assignToType) const {
  IExpression::checkForUndefined(context, mLeftExpression);
  IExpression::checkForUndefined(context, mRightExpression);

  Value* leftValue = mLeftExpression->generateIR(context, assignToType);
  Value* leftValueCast = AutoCast::maybeCast(context,
                                             mLeftExpression->getType(context),
                                             leftValue,
                                             PrimitiveTypes::BOOLEAN,
                                             mLeftExpression->getLine());
  BasicBlock* entryBlock = context.getBasicBlock();
  
  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* basicBlockRight = BasicBlock::Create(context.getLLVMContext(), "land.rhs", function);
  BasicBlock* basicBlockEnd = BasicBlock::Create(context.getLLVMContext(), "land.end", function);
  IRWriter::createConditionalBranch(context, basicBlockRight, basicBlockEnd, leftValueCast);
  
  context.setBasicBlock(basicBlockRight);
  Value* rightValue = mRightExpression->generateIR(context, assignToType);
  Value* rightValueCast = AutoCast::maybeCast(context,
                                             mRightExpression->getType(context),
                                             rightValue,
                                             PrimitiveTypes::BOOLEAN,
                                             mRightExpression->getLine());
  BasicBlock* lastRightBlock = context.getBasicBlock();
  IRWriter::createBranch(context, basicBlockEnd);
  
  context.setBasicBlock(basicBlockEnd);
  Type* type = Type::getInt1Ty(context.getLLVMContext());
  PHINode* phiNode = IRWriter::createPhiNode(context,
                                             type,
                                             "land",
                                             ConstantInt::getFalse(context.getLLVMContext()),
                                             entryBlock,
                                             rightValueCast,
                                             lastRightBlock);
  
  return phiNode;
}

const IType* LogicalAndExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN;
}

bool LogicalAndExpression::isConstant() const {
  return false;
}

bool LogicalAndExpression::isAssignable() const {
  return false;
}

void LogicalAndExpression::printToStream(IRGenerationContext& context,
                                         std::iostream& stream) const {
  mLeftExpression->printToStream(context, stream);
  stream << " && ";
  mRightExpression->printToStream(context, stream);
}
