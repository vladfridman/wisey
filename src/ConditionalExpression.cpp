//
//  ConditionalExpression.cpp
//  Wisey
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

ConditionalExpression::~ConditionalExpression() {
  delete mConditionExpression;
  delete mIfTrueExpression;
  delete mIfFalseExpression;
}

IVariable* ConditionalExpression::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value *ConditionalExpression::generateIR(IRGenerationContext& context) const {
  checkTypes(context);
  
  Value* conditionValue = mConditionExpression->generateIR(context);
  
  Function* function = context.getBasicBlock()->getParent();
  
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* blockTrue = BasicBlock::Create(llvmContext, "cond.true", function);
  BasicBlock* blockFalse = BasicBlock::Create(llvmContext, "cond.false", function);
  BasicBlock* blockEnd = BasicBlock::Create(llvmContext, "cond.end", function);
  IRWriter::createConditionalBranch(context, blockTrue, blockFalse, conditionValue);
  
  context.setBasicBlock(blockTrue);
  Value* ifTrueValue = mIfTrueExpression->generateIR(context);
  Type* ifTrueResultType = ifTrueValue->getType();
  IRWriter::createBranch(context, blockEnd);

  context.setBasicBlock(blockFalse);
  Value* ifFalseValue = mIfFalseExpression->generateIR(context);
  Type* ifFalseResultType = ifFalseValue->getType();
  IRWriter::createBranch(context, blockEnd);

  if (ifTrueResultType != ifFalseResultType) {
    Log::e("Results of different type in a conditional expresion!");
  }

  context.setBasicBlock(blockEnd);
  PHINode* phiNode = IRWriter::createPhiNode(context,
                                             ifTrueResultType,
                                             "cond",
                                             ifTrueValue,
                                             blockTrue,
                                             ifFalseValue,
                                             blockFalse);
  
  return phiNode;
}

const IType* ConditionalExpression::getType(IRGenerationContext& context) const {
  return mIfTrueExpression->getType(context);
}

void ConditionalExpression::releaseOwnership(IRGenerationContext& context) const {
  Function* function = context.getBasicBlock()->getParent();

  Value* conditionValue = mConditionExpression->generateIR(context);

  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* blockTrue = BasicBlock::Create(llvmContext, "cond.release.true", function);
  BasicBlock* blockFalse = BasicBlock::Create(llvmContext, "cond.release.false", function);
  BasicBlock* blockEnd = BasicBlock::Create(llvmContext, "cond.release.end", function);
  IRWriter::createConditionalBranch(context, blockTrue, blockFalse, conditionValue);

  Value* ifTrueValue = mIfTrueExpression->generateIR(context);
  const IType* ifTrueType = mIfTrueExpression->getType(context);
  Value* ifFalseValue = mIfFalseExpression->generateIR(context);
  const IType* ifFalseType = mIfFalseExpression->getType(context);

  context.setBasicBlock(blockTrue);
  mIfTrueExpression->releaseOwnership(context);
  assert(IType::isOwnerType(ifFalseType));
  ((IObjectOwnerType*) ifFalseType)->free(context, ifFalseValue);
  IRWriter::createBranch(context, blockEnd);
  
  context.setBasicBlock(blockFalse);
  mIfFalseExpression->releaseOwnership(context);
  assert(IType::isOwnerType(ifTrueType));
  ((IObjectOwnerType*) ifTrueType)->free(context, ifTrueValue);
  IRWriter::createBranch(context, blockEnd);

  context.setBasicBlock(blockEnd);
}

void ConditionalExpression::addReferenceToOwner(IRGenerationContext& context,
                                                IVariable* reference) const {
  mIfFalseExpression->addReferenceToOwner(context, reference);
  mIfTrueExpression->addReferenceToOwner(context, reference);
}

// TODO: implement a more sensible type checking/casting
void ConditionalExpression::checkTypes(IRGenerationContext& context) const {
  const IType* ifTrueExpressionType = mIfTrueExpression->getType(context);
  const IType* ifFalseExpressionType = mIfFalseExpression->getType(context);
  
  if (mConditionExpression->getType(context) != PrimitiveTypes::BOOLEAN_TYPE) {
    Log::e("Condition in a conditional expression is not of type BOOLEAN");
    exit(1);
  }
  
  if (ifTrueExpressionType != ifFalseExpressionType) {
    Log::e("Incompatible types in conditional expression operation");
    exit(1);
  }
  
  if (ifTrueExpressionType == PrimitiveTypes::VOID_TYPE) {
    Log::e("Can not use expressions of type VOID in a conditional expression");
    exit(1);
  }
}

bool ConditionalExpression::existsInOuterScope(IRGenerationContext& context) const {
  return mIfTrueExpression->existsInOuterScope(context) &&
  mIfFalseExpression->existsInOuterScope(context);
}

void ConditionalExpression::printToStream(std::iostream& stream) const {
  mConditionExpression->printToStream(stream);
  stream << " ? ";
  mIfTrueExpression->printToStream(stream);
  stream << " : ";
  mIfFalseExpression->printToStream(stream);
}
