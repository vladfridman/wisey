//
//  NegateExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef NegateExpression_h
#define NegateExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {

/**
 * Represents negate operation
 */
class NegateExpression : public IExpression {
  IExpression& mExpression;
  
public:
  
  NegateExpression(IExpression& expression) : mExpression(expression) { }
  
  ~NegateExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;

};

} /* namespace wisey */

#endif /* NegateExpression_h */
