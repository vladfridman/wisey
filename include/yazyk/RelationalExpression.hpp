//
//  RelationalExpression.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef RelationalExpression_h
#define RelationalExpression_h

#include "yazyk/IExpression.hpp"
#include "yazyk/IHasType.hpp"
#include "yazyk/RelationalOperation.hpp"

namespace yazyk {
  
/**
 * Represents a relational expression such as a >= b
 */
class RelationalExpression : public IExpression, public IHasType {
  IExpression& mLeftExpression;
  IExpression& mRightExpression;
  RelationalOperation mOperation;
  
public:

  RelationalExpression(IExpression& leftExpression,
                       RelationalOperation operation,
                       IExpression& rightExpression) :
    mLeftExpression(leftExpression), mRightExpression(rightExpression), mOperation(operation) { }

  ~RelationalExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* RelationalExpression_h */
