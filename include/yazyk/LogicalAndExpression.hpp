//
//  LogicalAndExpression.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef LogicalAndExpression_h
#define LogicalAndExpression_h

#include "yazyk/IExpression.hpp"
#include "yazyk/IHasType.hpp"

namespace yazyk {

/**
 * Represents a logical AND expression such as 'a && b'
 */
class LogicalAndExpression : public IExpression {
  IExpression& mLeftExpression;
  IExpression& mRightExpression;
  
public:
  LogicalAndExpression(IExpression& leftExpression, IExpression& rightExpression) :
    mLeftExpression(leftExpression),
    mRightExpression(rightExpression) { }
  
  ~LogicalAndExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
};

} /* namespace yazyk */

#endif /* LogicalAndExpression_h */
