//
//  AddditiveMultiplicativeExpression.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef AddditiveMultiplicativeExpression_hpp
#define AddditiveMultiplicativeExpression_hpp

#include "yazyk/IExpression.hpp"

namespace yazyk {

/**
 * Represents simple binary expression such as addition or division
 */
class AddditiveMultiplicativeExpression : public IExpression {
  IExpression& mLeftExpression;
  IExpression& mRightExpression;
  int mOperation;
  
public:
  AddditiveMultiplicativeExpression(IExpression& leftExpression,
                                    int operation,
                                    IExpression& rightExpression) :
    mLeftExpression(leftExpression), mRightExpression(rightExpression), mOperation(operation) { }
  
  ~AddditiveMultiplicativeExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

  IType* getType(IRGenerationContext& context) const override;
  
private:
  
  void checkTypes(IRGenerationContext& context) const;
};

} /* namespace yazyk */

#endif /* AddditiveMultiplicativeExpression_hpp */
