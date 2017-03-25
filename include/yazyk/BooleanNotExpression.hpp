//
//  BooleanNotExpression.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef BooleanNotExpression_h
#define BooleanNotExpression_h

#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"

namespace yazyk {
  
/**
 * Represents a boolean NOT operation
 */
class BooleanNotExpression : public IExpression {
  IExpression& mExpression;
  
public:
  
  BooleanNotExpression(IExpression& expression) : mExpression(expression) { }
  
  ~BooleanNotExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;

};
  
} /* namespace yazyk */

#endif /* BooleanNotExpression_h */
