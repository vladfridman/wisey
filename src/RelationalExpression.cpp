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

Value* RelationalExpression::generateIR(IRGenerationContext& context) const {
  ICmpInst::Predicate predicate;
  switch (mOperation) {
    case RELATIONAL_OPERATION_LT : predicate = ICmpInst::ICMP_SLT; break;
    case RELATIONAL_OPERATION_GT : predicate = ICmpInst::ICMP_SGT; break;
    case RELATIONAL_OPERATION_LE : predicate = ICmpInst::ICMP_SLE; break;
    case RELATIONAL_OPERATION_GE : predicate = ICmpInst::ICMP_SGE; break;
    case RELATIONAL_OPERATION_EQ : predicate = ICmpInst::ICMP_EQ; break;
    case RELATIONAL_OPERATION_NE : predicate = ICmpInst::ICMP_NE; break;
  }
  
  Value * leftValue = mLeftExpression.generateIR(context);
  Value * rightValue = mRightExpression.generateIR(context);
  
  return IRWriter::newICmpInst(context, predicate, leftValue, rightValue, "cmp");
}

IType* RelationalExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}

void RelationalExpression::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a relational expression result, it is not a heap pointer");
  exit(1);
}
