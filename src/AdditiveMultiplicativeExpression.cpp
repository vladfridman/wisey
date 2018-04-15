//
//  AdditiveMultiplicativeExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/InstrTypes.h>

#include "wisey/AdditiveMultiplicativeExpression.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

AdditiveMultiplicativeExpression::
AdditiveMultiplicativeExpression(IExpression* leftExpression,
                                 int operation,
                                 IExpression* rightExpression,
                                 int line) :
mLeftExpression(leftExpression),
mRightExpression(rightExpression),
mOperation(operation),
mLine(line) { }

AdditiveMultiplicativeExpression::~AdditiveMultiplicativeExpression() {
  delete mLeftExpression;
  delete mRightExpression;
}

int AdditiveMultiplicativeExpression::getLine() const {
  return mLine;
}

IVariable* AdditiveMultiplicativeExpression::getVariable(IRGenerationContext& context,
                                                         vector<const IExpression*>& arrayIndices) const {
  return NULL;
}

Value* AdditiveMultiplicativeExpression::generateIR(IRGenerationContext& context,
                                                    const IType* assignToType) const {
  const IType* leftType = mLeftExpression->getType(context);
  const IType* rightType = mRightExpression->getType(context);
  checkTypes(context, leftType, rightType);

  Instruction::BinaryOps instruction;
  string name;
  bool isFloat = leftType == PrimitiveTypes::FLOAT_TYPE ||
    leftType == PrimitiveTypes::DOUBLE_TYPE ||
    rightType == PrimitiveTypes::FLOAT_TYPE ||
    rightType == PrimitiveTypes::DOUBLE_TYPE;
  switch (mOperation) {
    case '+': name = "add"; instruction = isFloat ? Instruction::FAdd : Instruction::Add; break;
    case '-': name = "sub"; instruction = isFloat ? Instruction::FSub : Instruction::Sub; break;
    case '*': name = "mul"; instruction = isFloat ? Instruction::FMul : Instruction::Mul; break;
    case '/': name = "div"; instruction = isFloat ? Instruction::FDiv : Instruction::SDiv; break;
    case '%': name = "div"; instruction = isFloat ? Instruction::FRem : Instruction::SRem; break;
    default: return NULL;
  }
  
  Value* leftValue = mLeftExpression->generateIR(context, assignToType);
  Value* rightValue = mRightExpression->generateIR(context, assignToType);
  
  if (leftType->canAutoCastTo(context, rightType)) {
    leftValue = AutoCast::maybeCast(context, leftType, leftValue, rightType, mLine);
  } else {
    rightValue = AutoCast::maybeCast(context, rightType, rightValue, leftType, mLine);
  }
  
  return IRWriter::createBinaryOperator(context, instruction, leftValue, rightValue, name);
}

const IType* AdditiveMultiplicativeExpression::getType(IRGenerationContext& context) const {
  const IType* leftType = mLeftExpression->getType(context);
  const IType* rightType = mRightExpression->getType(context);
  checkTypes(context, leftType, rightType);

  if (leftType->canAutoCastTo(context, rightType)) {
    return rightType;
  }
  return leftType;
}

// TODO: implement a more sensible type checking/casting
void AdditiveMultiplicativeExpression::checkTypes(IRGenerationContext& context,
                                                  const IType* leftType,
                                                  const IType* rightType) const {
  if (leftType == PrimitiveTypes::VOID_TYPE || rightType == PrimitiveTypes::VOID_TYPE) {
    Log::e_deprecated("Can not use expressions of type VOID in a '" + string(1, mOperation) + "' operation");
    exit(1);
  }
  
  if (!leftType->isPrimitive() || !rightType->isPrimitive()) {
    Log::e_deprecated("Can not do operation '" + string(1, mOperation) + "' on non-primitive types");
    exit(1);
  }

  if (!leftType->canCastTo(context, rightType) && !rightType->canCastTo(context, leftType)) {
    Log::e_deprecated("Incompatible types in '" + string(1, mOperation) + "' operation");
    exit(1);
  }
  
  if (!leftType->canAutoCastTo(context, rightType) && !rightType->canAutoCastTo(context, leftType)) {
    Log::e_deprecated("Incompatible types in '" + string(1, mOperation) +
           "' operation that require an explicit cast");
    exit(1);
  }
}

bool AdditiveMultiplicativeExpression::isConstant() const {
  return false;
}

void AdditiveMultiplicativeExpression::printToStream(IRGenerationContext& context,
                                                     std::iostream& stream) const {
  mLeftExpression->printToStream(context, stream);
  stream << " + ";
  mRightExpression->printToStream(context, stream);
}
