//
//  ConditionalExpression.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef ConditionalExpression_h
#define ConditionalExpression_h

#include <llvm/IR/Value.h>

#include "yazyk/codegen.hpp"
#include "yazyk/node.hpp"

namespace yazyk {

/**
 * Represents a condition expression such as a == b ? 1 : 2
 */
class ConditionalExpression : public IExpression {
  
public:
  IExpression& conditionExpression;
  IExpression& conditionTrueExpression;
  IExpression& conditionFalseExpression;
  
  ConditionalExpression(IExpression& conditionExpression,
                        IExpression& conditionTrueExpression,
                        IExpression& conditionFalseExpression)
  : conditionExpression(conditionExpression),
    conditionTrueExpression(conditionTrueExpression),
    conditionFalseExpression(conditionFalseExpression) {  }
  
  llvm::Value* generateIR(IRGenerationContext& context) override;
};

}

#endif /* ConditionalExpression_h */
