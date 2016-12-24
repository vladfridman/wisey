//
//  ConditionalExpression.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef ConditionalExpression_h
#define ConditionalExpression_h

#include "yazyk/IExpression.hpp"

namespace yazyk {

/**
 * Represents a condition expression such as a == b ? 1 : 2
 */
class ConditionalExpression : public IExpression {
  
  IExpression& conditionExpression;
  IExpression& conditionTrueExpression;
  IExpression& conditionFalseExpression;
  
public:
  ConditionalExpression(IExpression& conditionExpression,
                        IExpression& conditionTrueExpression,
                        IExpression& conditionFalseExpression)
  : conditionExpression(conditionExpression),
    conditionTrueExpression(conditionTrueExpression),
    conditionFalseExpression(conditionFalseExpression) {  }
  
  llvm::Value* generateIR(IRGenerationContext& context) override;
};

} /* namespace yazyk */

#endif /* ConditionalExpression_h */
