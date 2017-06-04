//
//  RelationalExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef RelationalExpression_h
#define RelationalExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/IHasType.hpp"
#include "wisey/RelationalOperation.hpp"

namespace wisey {
  
/**
 * Represents a relational expression such as a >= b
 */
class RelationalExpression : public IExpression {
  IExpression* mLeftExpression;
  IExpression* mRightExpression;
  RelationalOperation mOperation;
  
public:

  RelationalExpression(IExpression* leftExpression,
                       RelationalOperation operation,
                       IExpression* rightExpression) :
    mLeftExpression(leftExpression), mRightExpression(rightExpression), mOperation(operation) { }

  ~RelationalExpression();
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
};
  
} /* namespace wisey */

#endif /* RelationalExpression_h */
