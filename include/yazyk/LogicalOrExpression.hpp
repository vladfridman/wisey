//
//  LogicalOrExpression.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef LogicalOrExpression_h
#define LogicalOrExpression_h

#include "yazyk/node.hpp"

namespace yazyk {

/**
 * Represents a logical OR expression such as 'a || b'
 */
class LogicalOrExpression : public IExpression {
  IExpression& mLeftExpression;
  IExpression& mRightExpression;
  
public:
  LogicalOrExpression(IExpression& leftExpression, IExpression& rightExpression) :
    mLeftExpression(leftExpression),
    mRightExpression(rightExpression) { }
  
  ~LogicalOrExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) override;
};

} /* namespace yazyk */

#endif /* LogicalOrExpression_h */
