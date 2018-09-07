//
//  RelationalExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/RelationalExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

RelationalExpression::RelationalExpression(const IExpression* left,
                                           RelationalOperation operation,
                                           const IExpression* right,
                                           int line) :
mLeft(left),
mRight(right),
mOperation(operation),
mLine(line) { }

RelationalExpression::~RelationalExpression() {
  delete mLeft;
  delete mRight;
}

const IExpression* RelationalExpression::getLeft() const {
  return mLeft;
}

const IExpression* RelationalExpression::getRight() const {
  return mRight;
}

string RelationalExpression::getOperation() const {
  switch (mOperation) {
    case RELATIONAL_OPERATION_LT : return "<"; break;
    case RELATIONAL_OPERATION_GT : return ">"; break;
    case RELATIONAL_OPERATION_LE : return "<="; break;
    case RELATIONAL_OPERATION_GE : return ">="; break;
    case RELATIONAL_OPERATION_EQ : return "=="; break;
    case RELATIONAL_OPERATION_NE : return "!="; break;
  }
  assert(false && "unknown relational operation");
}


int RelationalExpression::getLine() const {
  return mLine;
}

Value* RelationalExpression::generateIR(IRGenerationContext& context,
                                        const IType* assignToType) const {
  IExpression::checkForUndefined(context, mLeft);
  IExpression::checkForUndefined(context, mRight);

  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);
  if ((!leftType->isPrimitive() && rightType->isPrimitive()) ||
      (leftType->isPrimitive() && !rightType->isPrimitive())) {
    context.reportError(mLine, "Can not compare objects to primitive types");
    throw 1;
  }
  
  if (!leftType->isPrimitive() || !rightType->isPrimitive()) {
    return generateIRForObjects(context, assignToType);
  }
  
  if (PrimitiveTypes::isFloatType(leftType) || PrimitiveTypes::isFloatType(rightType)) {
    return generateIRForFloats(context, assignToType);
  }
  
  return generateIRForInts(context, assignToType);
}

const IType* RelationalExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN;
}

Value* RelationalExpression::generateIRForObjects(IRGenerationContext& context,
                                                  const IType* assignToType) const {
  ICmpInst::Predicate predicate;
  switch (mOperation) {
    case RELATIONAL_OPERATION_EQ : predicate = ICmpInst::ICMP_EQ; break;
    case RELATIONAL_OPERATION_NE : predicate = ICmpInst::ICMP_NE; break;
    default:
      context.reportError(mLine, "Objects can only be used to compare for equality");
      throw 1;
  }
  
  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);
  
  Value* leftValue = mLeft->generateIR(context, assignToType);
  Value* rightValue = mRight->generateIR(context, assignToType);
  
  if (leftType == rightType) {
    return IRWriter::newICmpInst(context, predicate, leftValue, rightValue, "cmp");
  }
  
  if (leftType->canAutoCastTo(context, rightType)) {
    Value* castedLeftValue = leftType->castTo(context, leftValue, rightType, mLine);
    return IRWriter::newICmpInst(context, predicate, castedLeftValue, rightValue, "cmp");
  }
  
  if (rightType->canAutoCastTo(context, leftType)) {
    Value* castedRightValue = rightType->castTo(context, rightValue, leftType, mLine);
    return IRWriter::newICmpInst(context, predicate, leftValue, castedRightValue, "cmp");
  }
  
  reportIncompatableTypes(context, leftType, rightType);
  throw 1;
}

Value* RelationalExpression::generateIRForFloats(IRGenerationContext& context,
                                                 const IType* assignToType) const {
  FCmpInst::Predicate predicate;
  switch (mOperation) {
    case RELATIONAL_OPERATION_LT : predicate = FCmpInst::FCMP_OLT; break;
    case RELATIONAL_OPERATION_GT : predicate = FCmpInst::FCMP_OGT; break;
    case RELATIONAL_OPERATION_LE : predicate = FCmpInst::FCMP_OLE; break;
    case RELATIONAL_OPERATION_GE : predicate = FCmpInst::FCMP_OGE; break;
    case RELATIONAL_OPERATION_EQ : predicate = FCmpInst::FCMP_OEQ; break;
    case RELATIONAL_OPERATION_NE : predicate = FCmpInst::FCMP_ONE; break;
  }
  
  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);
  
  Value* leftValue = mLeft->generateIR(context, assignToType);
  Value* rightValue = mRight->generateIR(context, assignToType);
  
  if (leftType == rightType) {
    return IRWriter::newFCmpInst(context, predicate, leftValue, rightValue, "cmp");
  }
  
  if (leftType->canAutoCastTo(context, rightType)) {
    Value* castedLeftValue = leftType->castTo(context, leftValue, rightType, mLine);
    return IRWriter::newFCmpInst(context, predicate, castedLeftValue, rightValue, "cmp");
  }
  
  if (rightType->canAutoCastTo(context, leftType)) {
    Value* castedRightValue = rightType->castTo(context, rightValue, leftType, mLine);
    return IRWriter::newFCmpInst(context, predicate, leftValue, castedRightValue, "cmp");
  }
  
  reportIncompatableTypes(context, leftType, rightType);
  throw 1;
}

Value* RelationalExpression::generateIRForInts(IRGenerationContext& context,
                                               const IType* assignToType) const {
  ICmpInst::Predicate predicate;
  switch (mOperation) {
    case RELATIONAL_OPERATION_LT : predicate = ICmpInst::ICMP_SLT; break;
    case RELATIONAL_OPERATION_GT : predicate = ICmpInst::ICMP_SGT; break;
    case RELATIONAL_OPERATION_LE : predicate = ICmpInst::ICMP_SLE; break;
    case RELATIONAL_OPERATION_GE : predicate = ICmpInst::ICMP_SGE; break;
    case RELATIONAL_OPERATION_EQ : predicate = ICmpInst::ICMP_EQ; break;
    case RELATIONAL_OPERATION_NE : predicate = ICmpInst::ICMP_NE; break;
  }

  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);

  Value* leftValue = mLeft->generateIR(context, assignToType);
  Value* rightValue = mRight->generateIR(context, assignToType);
  
  if (leftType == rightType) {
    return IRWriter::newICmpInst(context, predicate, leftValue, rightValue, "cmp");
  }
  
  if (leftType->canAutoCastTo(context, rightType)) {
    Value* castedLeftValue = leftType->castTo(context, leftValue, rightType, mLine);
    return IRWriter::newICmpInst(context, predicate, castedLeftValue, rightValue, "cmp");
  }
  
  if (rightType->canAutoCastTo(context, leftType)) {
    Value* castedRightValue = rightType->castTo(context, rightValue, leftType, mLine);
    return IRWriter::newICmpInst(context, predicate, leftValue, castedRightValue, "cmp");
  }
  
  reportIncompatableTypes(context, leftType, rightType);
  throw 1;
}

void RelationalExpression::reportIncompatableTypes(IRGenerationContext& context,
                                                   const IType* leftType,
                                                   const IType* rightType) const {
  context.reportError(mLine, "Can not compare types " + leftType->getTypeName() + " and " +
                      rightType->getTypeName());
}

bool RelationalExpression::isConstant() const {
  return false;
}

bool RelationalExpression::isAssignable() const {
  return false;
}

void RelationalExpression::printToStream(IRGenerationContext& context,
                                         std::iostream& stream) const {
  IBinaryExpression::printToStream(context, stream, this);
}
