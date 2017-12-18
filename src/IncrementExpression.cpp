//
//  IncrementExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/FakeExpression.hpp"
#include "wisey/IncrementExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IncrementExpression::IncrementExpression(IExpression* expression,
                                         long long incrementBy,
                                         string variableName,
                                         bool isPrefix,
                                         int line) :
mExpression(expression),
mIncrementBy(incrementBy),
mVariableName(variableName),
mIsPrefix(isPrefix),
mLine(line) { }

IncrementExpression::~IncrementExpression() {
  delete mExpression;
}

IncrementExpression* IncrementExpression::newIncrementByOne(IExpression* expression, int line) {
  return new IncrementExpression(expression, 1, "inc", false, line);
}

IncrementExpression* IncrementExpression::newDecrementByOne(IExpression* expression, int line) {
  return new IncrementExpression(expression, -1, "dec", false, line);
}

IVariable* IncrementExpression::getVariable(IRGenerationContext& context,
                                            vector<const IExpression*>& arrayIndices) const {
  return mExpression->getVariable(context, arrayIndices);
}

Value* IncrementExpression::generateIR(IRGenerationContext& context, IRGenerationFlag flag) const {
  assert(flag == IR_GENERATION_NORMAL);
  
  const IType* expressionType = mExpression->getType(context);
  vector<const IExpression*> arrayIndices;
  IVariable* variable = mExpression->getVariable(context, arrayIndices);
  if (!variable) {
    Log::e("Increment/decrement operation may only be applied to variables");
    exit(1);
  }
  if (expressionType != PrimitiveTypes::INT_TYPE &&
      expressionType != PrimitiveTypes::CHAR_TYPE &&
      expressionType != PrimitiveTypes::LONG_TYPE) {
    Log::e("Expression is of a type that is incompatible with increment/decrement operation");
    exit(1);
  }
  
  Value* originalValue = mExpression->generateIR(context, flag);
  Value* increment = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()),
                                      mIncrementBy,
                                      true);
  
  Value* incrementResult = IRWriter::createBinaryOperator(context,
                                                          Instruction::Add,
                                                          originalValue,
                                                          increment,
                                                          mVariableName);

  
  FakeExpression fakeExpression(incrementResult, variable->getType());
  variable->generateAssignmentIR(context, &fakeExpression, arrayIndices, mLine);

  return mIsPrefix ? incrementResult : originalValue;
}

const IType* IncrementExpression::getType(IRGenerationContext& context) const {
  return mExpression->getType(context);
}

bool IncrementExpression::isConstant() const {
  return false;
}

void IncrementExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  mExpression->printToStream(context, stream);
  if (mIncrementBy > 0) {
    stream << "++";
  } else {
    stream << "--";
  }
}
