//
//  RelationalExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Value.h>

#include "yazyk/RelationalExpression.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* RelationalExpression::generateIR(IRGenerationContext& context) {
  ICmpInst::Predicate predicate;
  switch (operation) {
    case RELATIONAL_OPERATION_LT : predicate = ICmpInst::ICMP_SLT; break;
    case RELATIONAL_OPERATION_GT : predicate = ICmpInst::ICMP_SGT; break;
    case RELATIONAL_OPERATION_LE : predicate = ICmpInst::ICMP_SLE; break;
    case RELATIONAL_OPERATION_GE : predicate = ICmpInst::ICMP_SGE; break;
    case RELATIONAL_OPERATION_EQ : predicate = ICmpInst::ICMP_EQ; break;
    case RELATIONAL_OPERATION_NE : predicate = ICmpInst::ICMP_NE; break;
  }
  
  Value * lhsValue = lhs.generateIR(context);
  Value * rhsValue = rhs.generateIR(context);
  
  return new ICmpInst(*context.currentBlock(),
                      predicate,
                      lhsValue,
                      rhsValue,
                      "cmp");
}
