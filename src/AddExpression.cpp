//
//  AddExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/AddExpression.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

AddExpression::AddExpression(const IExpression* left, const IExpression* right, int line) :
mLeft(left), mRight(right), mLine(line) {}

AddExpression::~AddExpression() {
  delete mLeft;
  delete mRight;
}

int AddExpression::getLine() const {
  return mLine;
}

const IExpression* AddExpression::getLeft() const {
  return mLeft;
}

const IExpression* AddExpression::getRight() const {
  return mRight;
}

Value* AddExpression::generateIR(IRGenerationContext& context,
                                 const IType* assignToType) const {
  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);
  checkTypes(context, leftType, rightType, '+', mLine);
  
  Value* leftValue = mLeft->generateIR(context, PrimitiveTypes::VOID);
  Value* rightValue = mRight->generateIR(context, PrimitiveTypes::VOID);
  
  if (leftType->isPointer()) {
    Value* index[1];
    index[0] = rightValue;
    return IRWriter::createGetElementPtrInst(context, leftValue, index);
  }

  if (StringType::isStringVariation(context, leftType, mLine) ||
      StringType::isStringVariation(context, rightType, mLine)) {
    assert(false && "attempting to get value of a stringformat type expression");
  }

  bool isFloat = leftType == PrimitiveTypes::FLOAT ||
  leftType == PrimitiveTypes::DOUBLE ||
  rightType == PrimitiveTypes::FLOAT ||
  rightType == PrimitiveTypes::DOUBLE;
  Instruction::BinaryOps instruction = isFloat ? Instruction::FAdd : Instruction::Add;
  
  if (leftType->canAutoCastTo(context, rightType)) {
    leftValue = AutoCast::maybeCast(context, leftType, leftValue, rightType, mLine);
  } else {
    rightValue = AutoCast::maybeCast(context, rightType, rightValue, leftType, mLine);
  }
  
  return IRWriter::createBinaryOperator(context, instruction, leftValue, rightValue, "add");
}

const IType* AddExpression::getType(IRGenerationContext& context) const {
  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);
  checkTypes(context, leftType, rightType, '+', mLine);
  
  if (leftType->isPointer()) {
    return leftType;
  }
  
  if (StringType::isStringVariation(context, leftType, mLine) ||
      StringType::isStringVariation(context, rightType, mLine)) {
    return PrimitiveTypes::STRING_FORMAT;
  }

  return leftType->canAutoCastTo(context, rightType) ? rightType : leftType;
}

bool AddExpression::isConstant() const {
  return false;
}

bool AddExpression::isAssignable() const {
  return false;
}

void AddExpression::printToStream(IRGenerationContext& context,
                                  std::iostream& stream) const {
  mLeft->printToStream(context, stream);
  stream << " + ";
  mRight->printToStream(context, stream);
}

