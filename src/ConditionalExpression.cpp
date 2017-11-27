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

Value* ConditionalExpression::generateIR(IRGenerationContext& context,
                                         IRGenerationFlag flag) const {
  checkTypes(context);
  
  Value* conditionValue = mConditionExpression->generateIR(context, IR_GENERATION_NORMAL);
  
  Function* function = context.getBasicBlock()->getParent();
  
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* blockTrue = BasicBlock::Create(llvmContext, "cond.true", function);
  BasicBlock* blockFalse = BasicBlock::Create(llvmContext, "cond.false", function);
  BasicBlock* blockEnd = BasicBlock::Create(llvmContext, "cond.end", function);
  IRWriter::createConditionalBranch(context, blockTrue, blockFalse, conditionValue);
  
  context.setBasicBlock(blockTrue);
  Value* ifTrueValue = mIfTrueExpression->generateIR(context, flag);
  Type* ifTrueResultType = ifTrueValue->getType();
  IRWriter::createBranch(context, blockEnd);

  context.setBasicBlock(blockFalse);
  Value* ifFalseValue = mIfFalseExpression->generateIR(context, flag);
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

bool ConditionalExpression::isConstant() const {
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
