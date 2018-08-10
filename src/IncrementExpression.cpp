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

int IncrementExpression::getLine() const {
  return mLine;
}

IncrementExpression* IncrementExpression::newIncrementByOne(IExpression* expression, int line) {
  return new IncrementExpression(expression, 1, "inc", false, line);
}

IncrementExpression* IncrementExpression::newDecrementByOne(IExpression* expression, int line) {
  return new IncrementExpression(expression, -1, "dec", false, line);
}

IVariable* IncrementExpression::getVariable(IRGenerationContext& context,
                                            vector<const IExpression*>& arrayIndices) const {
  if (!mExpression->isAssignable()) {
    context.reportError(mLine, "Expression is not assignable");
    throw 1;
  }
  return ((IExpressionAssignable*) mExpression)->getVariable(context, arrayIndices);
}

Value* IncrementExpression::generateIR(IRGenerationContext& context,
                                       const IType* assignToType) const {
  const IType* expressionType = mExpression->getType(context);
  if (!mExpression->isAssignable()) {
    context.reportError(mLine, "Increment/decrement operation may only be applied to variables");
    throw 1;
  }
  if (expressionType != PrimitiveTypes::INT &&
      expressionType != PrimitiveTypes::CHAR &&
      expressionType != PrimitiveTypes::BYTE &&
      expressionType != PrimitiveTypes::LONG) {
    context.reportError(mLine,
                        "Expression is of a type that is incompatible with "
                        "increment/decrement operation");
    throw 1;
  }
  
  Value* originalValue = mExpression->generateIR(context, assignToType);
  Value* increment = ConstantInt::get(expressionType->getLLVMType(context),
                                      mIncrementBy,
                                      true);
  
  Value* incrementResult = IRWriter::createBinaryOperator(context,
                                                          Instruction::Add,
                                                          originalValue,
                                                          increment,
                                                          mVariableName);

  vector<const IExpression*> arrayIndices;
  IVariable* variable = ((IExpressionAssignable*) mExpression)->getVariable(context, arrayIndices);

  FakeExpression fakeExpression(incrementResult, expressionType);
  variable->generateAssignmentIR(context, &fakeExpression, arrayIndices, mLine);

  return mIsPrefix ? incrementResult : originalValue;
}

const IType* IncrementExpression::getType(IRGenerationContext& context) const {
  return mExpression->getType(context);
}

bool IncrementExpression::isConstant() const {
  return false;
}

bool IncrementExpression::isAssignable() const {
  return true;
}

void IncrementExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  mExpression->printToStream(context, stream);
  if (mIncrementBy > 0) {
    stream << "++";
  } else {
    stream << "--";
  }
}
