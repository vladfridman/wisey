//
//  ConditionalExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "AutoCast.hpp"
#include "ConditionalExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "Log.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ConditionalExpression::ConditionalExpression(const IExpression* conditionExpression,
                                             const IExpression* ifTrueExpression,
                                             const IExpression* ifFalseExpression,
                                             int line) :
mConditionExpression(conditionExpression),
mIfTrueExpression(ifTrueExpression),
mIfFalseExpression(ifFalseExpression),
mLine(line) {  }

ConditionalExpression::~ConditionalExpression() {
  delete mConditionExpression;
  delete mIfTrueExpression;
  delete mIfFalseExpression;
}

int ConditionalExpression::getLine() const {
  return mLine;
}

Value* ConditionalExpression::generateIR(IRGenerationContext& context,
                                         const IType* assignToType) const {
  checkTypes(context);
  
  Value* conditionValue = mConditionExpression->generateIR(context, assignToType);
  const IType* conditionType = mConditionExpression->getType(context);
  Value* conditionValueCast = AutoCast::maybeCast(context,
                                                  conditionType,
                                                  conditionValue,
                                                  PrimitiveTypes::BOOLEAN,
                                                  mLine);
  
  Function* function = context.getBasicBlock()->getParent();
  
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* blockTrue = BasicBlock::Create(llvmContext, "cond.true", function);
  BasicBlock* blockFalse = BasicBlock::Create(llvmContext, "cond.false", function);
  BasicBlock* blockEnd = BasicBlock::Create(llvmContext, "cond.end", function);
  IRWriter::createConditionalBranch(context, blockTrue, blockFalse, conditionValueCast);
  
  context.setBasicBlock(blockTrue);
  Value* ifTrueValue = mIfTrueExpression->generateIR(context, assignToType);
  Type* ifTrueResultType = ifTrueValue->getType();
  IRWriter::createBranch(context, blockEnd);
  BasicBlock* blockFromTrue = context.getBasicBlock();

  context.setBasicBlock(blockFalse);
  Value* ifFalseValue = mIfFalseExpression->generateIR(context, assignToType);
  Type* ifFalseResultType = ifFalseValue->getType();
  IRWriter::createBranch(context, blockEnd);
  BasicBlock* blockFromFalse = context.getBasicBlock();

  if (ifTrueResultType != ifFalseResultType) {
    context.reportError(mLine, "Results of different type in a conditional expresion!");
    throw 1;
  }

  context.setBasicBlock(blockEnd);
  PHINode* phiNode = IRWriter::createPhiNode(context,
                                             ifTrueResultType,
                                             "cond",
                                             ifTrueValue,
                                             blockFromTrue,
                                             ifFalseValue,
                                             blockFromFalse);
  
  return phiNode;
}

const IType* ConditionalExpression::getType(IRGenerationContext& context) const {
  return mIfTrueExpression->getType(context);
}

// TODO: implement a more sensible type checking/casting
void ConditionalExpression::checkTypes(IRGenerationContext& context) const {
  const IType* ifTrueType = mIfTrueExpression->getType(context);
  const IType* ifFalseType = mIfFalseExpression->getType(context);

  IExpression::checkForUndefined(context, mConditionExpression);
  IExpression::checkForUndefined(context, mIfTrueExpression);
  IExpression::checkForUndefined(context, mIfFalseExpression);

  if (!mConditionExpression->getType(context)->canAutoCastTo(context, PrimitiveTypes::BOOLEAN)) {
    context.reportError(mLine, "Condition in a conditional expression is not of type BOOLEAN");
    throw 1;
  }
  
  if (ifTrueType != ifFalseType &&
      ifTrueType->getLLVMType(context) != ifFalseType->getLLVMType(context)) {
    context.reportError(mLine, "Incompatible types in conditional expression operation");
    throw 1;
  }
  
  if (ifTrueType == PrimitiveTypes::VOID) {
    context.reportError(mLine, "Can not use expressions of type VOID in a conditional expression");
    throw 1;
  }
}

bool ConditionalExpression::isConstant() const {
  return false;
}

bool ConditionalExpression::isAssignable() const {
  return false;
}

void ConditionalExpression::printToStream(IRGenerationContext& context,
                                          std::iostream& stream) const {
  mConditionExpression->printToStream(context, stream);
  stream << " ? ";
  mIfTrueExpression->printToStream(context, stream);
  stream << " : ";
  mIfFalseExpression->printToStream(context, stream);
}
