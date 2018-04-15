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
using namespace std;
using namespace wisey;

LogicalOrExpression::LogicalOrExpression(IExpression* leftExpression,
                                         IExpression* rightExpression,
                                         int line) :
mLeftExpression(leftExpression),
mRightExpression(rightExpression),
mLine(line) { }

LogicalOrExpression::~LogicalOrExpression() {
  delete mLeftExpression;
  delete mRightExpression;
}

int LogicalOrExpression::getLine() const {
  return mLine;
}

IVariable* LogicalOrExpression::getVariable(IRGenerationContext& context,
                                            vector<const IExpression*>& arrayIndices) const {
  return NULL;
}

Value* LogicalOrExpression::generateIR(IRGenerationContext& context,
                                       const IType* assignToType) const {
  Value* leftValue = mLeftExpression->generateIR(context, assignToType);
  BasicBlock* entryBlock = context.getBasicBlock();
  
  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* basicBlockRight = BasicBlock::Create(context.getLLVMContext(), "lor.rhs", function);
  BasicBlock* basicBlockEnd = BasicBlock::Create(context.getLLVMContext(), "lor.end", function);
  IRWriter::createConditionalBranch(context, basicBlockEnd, basicBlockRight, leftValue);
  
  context.setBasicBlock(basicBlockRight);
  Value* rightValue = mRightExpression->generateIR(context, assignToType);
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

bool LogicalOrExpression::isConstant() const {
  return false;
}

void LogicalOrExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  mLeftExpression->printToStream(context, stream);
  stream << " || ";
  mRightExpression->printToStream(context, stream);
}
