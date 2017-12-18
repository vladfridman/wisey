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

RelationalExpression::RelationalExpression(IExpression* leftExpression,
                                           RelationalOperation operation,
                                           IExpression* rightExpression,
                                           int line) :
mLeftExpression(leftExpression),
mRightExpression(rightExpression),
mOperation(operation),
mLine(line) { }

RelationalExpression::~RelationalExpression() {
  delete mLeftExpression;
  delete mRightExpression;
}

IVariable* RelationalExpression::getVariable(IRGenerationContext& context,
                                             vector<const IExpression*>& arrayIndices) const {
  return NULL;
}

Value* RelationalExpression::generateIR(IRGenerationContext& context, IRGenerationFlag flag) const {
  assert(flag == IR_GENERATION_NORMAL);
  
  const IType* leftType = mLeftExpression->getType(context);
  const IType* rightType = mRightExpression->getType(context);
  TypeKind leftTypeKind = leftType->getTypeKind();
  TypeKind rightTypeKind = rightType->getTypeKind();
  if ((leftTypeKind != PRIMITIVE_TYPE && rightTypeKind == PRIMITIVE_TYPE) ||
      (leftTypeKind == PRIMITIVE_TYPE && rightTypeKind != PRIMITIVE_TYPE)) {
    Log::e("Can not compare objects to primitive types");
    exit(1);
  }
  
  if (leftTypeKind != PRIMITIVE_TYPE || rightTypeKind != PRIMITIVE_TYPE) {
    return generateIRForObjects(context);
  }
  
  if (PrimitiveTypes::isFloatType(leftType) || PrimitiveTypes::isFloatType(rightType)) {
    return generateIRForFloats(context);
  }
  
  return generateIRForInts(context);
}

const IType* RelationalExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}

Value* RelationalExpression::generateIRForObjects(IRGenerationContext& context) const {
  ICmpInst::Predicate predicate;
  switch (mOperation) {
    case RELATIONAL_OPERATION_EQ : predicate = ICmpInst::ICMP_EQ; break;
    case RELATIONAL_OPERATION_NE : predicate = ICmpInst::ICMP_NE; break;
    default:
      Log::e("Objects can only be used to compare for equality");
      exit(1);
  }
  
  const IType* leftType = mLeftExpression->getType(context);
  const IType* rightType = mRightExpression->getType(context);
  
  Value* leftValue = mLeftExpression->generateIR(context, IR_GENERATION_NORMAL);
  Value* rightValue = mRightExpression->generateIR(context, IR_GENERATION_NORMAL);
  
  if (leftType == rightType) {
    return IRWriter::newICmpInst(context, predicate, leftValue, rightValue, "cmp");
  }
  
  if (leftType->canAutoCastTo(rightType)) {
    Value* castedLeftValue = leftType->castTo(context, leftValue, rightType, mLine);
    return IRWriter::newICmpInst(context, predicate, castedLeftValue, rightValue, "cmp");
  }
  
  if (rightType->canAutoCastTo(leftType)) {
    Value* castedRightValue = rightType->castTo(context, rightValue, leftType, mLine);
    return IRWriter::newICmpInst(context, predicate, leftValue, castedRightValue, "cmp");
  }
  
  reportIncompatableTypes(leftType, rightType);
  exit(1);
}

Value* RelationalExpression::generateIRForFloats(IRGenerationContext& context) const {
  FCmpInst::Predicate predicate;
  switch (mOperation) {
    case RELATIONAL_OPERATION_LT : predicate = FCmpInst::FCMP_OLT; break;
    case RELATIONAL_OPERATION_GT : predicate = FCmpInst::FCMP_OGT; break;
    case RELATIONAL_OPERATION_LE : predicate = FCmpInst::FCMP_OLE; break;
    case RELATIONAL_OPERATION_GE : predicate = FCmpInst::FCMP_OGE; break;
    case RELATIONAL_OPERATION_EQ : predicate = FCmpInst::FCMP_OEQ; break;
    case RELATIONAL_OPERATION_NE : predicate = FCmpInst::FCMP_ONE; break;
  }
  
  const IType* leftType = mLeftExpression->getType(context);
  const IType* rightType = mRightExpression->getType(context);
  
  Value* leftValue = mLeftExpression->generateIR(context, IR_GENERATION_NORMAL);
  Value* rightValue = mRightExpression->generateIR(context, IR_GENERATION_NORMAL);
  
  if (leftType == rightType) {
    return IRWriter::newFCmpInst(context, predicate, leftValue, rightValue, "cmp");
  }
  
  if (leftType->canAutoCastTo(rightType)) {
    Value* castedLeftValue = leftType->castTo(context, leftValue, rightType, mLine);
    return IRWriter::newFCmpInst(context, predicate, castedLeftValue, rightValue, "cmp");
  }
  
  if (rightType->canAutoCastTo(leftType)) {
    Value* castedRightValue = rightType->castTo(context, rightValue, leftType, mLine);
    return IRWriter::newFCmpInst(context, predicate, leftValue, castedRightValue, "cmp");
  }
  
  reportIncompatableTypes(leftType, rightType);
  exit(1);
}

Value* RelationalExpression::generateIRForInts(IRGenerationContext& context) const {
  ICmpInst::Predicate predicate;
  switch (mOperation) {
    case RELATIONAL_OPERATION_LT : predicate = ICmpInst::ICMP_SLT; break;
    case RELATIONAL_OPERATION_GT : predicate = ICmpInst::ICMP_SGT; break;
    case RELATIONAL_OPERATION_LE : predicate = ICmpInst::ICMP_SLE; break;
    case RELATIONAL_OPERATION_GE : predicate = ICmpInst::ICMP_SGE; break;
    case RELATIONAL_OPERATION_EQ : predicate = ICmpInst::ICMP_EQ; break;
    case RELATIONAL_OPERATION_NE : predicate = ICmpInst::ICMP_NE; break;
  }

  const IType* leftType = mLeftExpression->getType(context);
  const IType* rightType = mRightExpression->getType(context);

  Value* leftValue = mLeftExpression->generateIR(context, IR_GENERATION_NORMAL);
  Value* rightValue = mRightExpression->generateIR(context, IR_GENERATION_NORMAL);
  
  if (leftType == rightType) {
    return IRWriter::newICmpInst(context, predicate, leftValue, rightValue, "cmp");
  }
  
  if (leftType->canAutoCastTo(rightType)) {
    Value* castedLeftValue = leftType->castTo(context, leftValue, rightType, mLine);
    return IRWriter::newICmpInst(context, predicate, castedLeftValue, rightValue, "cmp");
  }
  
  if (rightType->canAutoCastTo(leftType)) {
    Value* castedRightValue = rightType->castTo(context, rightValue, leftType, mLine);
    return IRWriter::newICmpInst(context, predicate, leftValue, castedRightValue, "cmp");
  }
  
  reportIncompatableTypes(leftType, rightType);
  exit(1);
}

void RelationalExpression::reportIncompatableTypes(const IType* leftType,
                                                   const IType* rightType) const {
  Log::e("Can not compare types " + leftType->getTypeName() + " and " + rightType->getTypeName());
}

bool RelationalExpression::isConstant() const {
  return false;
}

void RelationalExpression::printToStream(IRGenerationContext& context,
                                         std::iostream& stream) const {
  mLeftExpression->printToStream(context, stream);
  switch (mOperation) {
    case RELATIONAL_OPERATION_LT : stream << " < "; break;
    case RELATIONAL_OPERATION_GT : stream << " > "; break;
    case RELATIONAL_OPERATION_LE : stream << " <= "; break;
    case RELATIONAL_OPERATION_GE : stream << " >= "; break;
    case RELATIONAL_OPERATION_EQ : stream << " == "; break;
    case RELATIONAL_OPERATION_NE : stream << " != "; break;
  }
  mRightExpression->printToStream(context, stream);
}
