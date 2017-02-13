//
//  AdditiveMultiplicativeExpression.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef AdditiveMultiplicativeExpression_hpp
#define AdditiveMultiplicativeExpression_hpp

#include "yazyk/IExpression.hpp"

namespace yazyk {

/**
 * Represents simple binary expression such as addition or division
 */
class AdditiveMultiplicativeExpression : public IExpression {
  IExpression& mLeftExpression;
  IExpression& mRightExpression;
  int mOperation;
  
public:
  AdditiveMultiplicativeExpression(IExpression& leftExpression,
                                   int operation,
                                   IExpression& rightExpression) :
    mLeftExpression(leftExpression), mRightExpression(rightExpression), mOperation(operation) { }
  
  ~AdditiveMultiplicativeExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

  IType* getType(IRGenerationContext& context) const override;
  
private:
  
  void checkTypes(IRGenerationContext& context) const;
};

} /* namespace yazyk */

#endif /* AdditiveMultiplicativeExpression_hpp */