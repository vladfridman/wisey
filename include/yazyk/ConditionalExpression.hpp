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
  
  IExpression& mConditionExpression;
  IExpression& mConditionTrueExpression;
  IExpression& mConditionFalseExpression;
  
public:
  ConditionalExpression(IExpression& conditionExpression,
                        IExpression& conditionTrueExpression,
                        IExpression& conditionFalseExpression)
  : mConditionExpression(conditionExpression),
    mConditionTrueExpression(conditionTrueExpression),
    mConditionFalseExpression(conditionFalseExpression) {  }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

  IType* getType(IRGenerationContext& context) const override;
  
private:
  
  void checkTypes(IRGenerationContext& context) const;
};

} /* namespace yazyk */

#endif /* ConditionalExpression_h */
