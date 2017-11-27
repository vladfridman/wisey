//
//  BooleanNotExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef BooleanNotExpression_h
#define BooleanNotExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
/**
 * Represents a boolean NOT operation
 */
class BooleanNotExpression : public IExpression {
  IExpression* mExpression;
  
public:
  
  BooleanNotExpression(IExpression* expression) : mExpression(expression) { }
  
  ~BooleanNotExpression();
  
  IVariable* getVariable(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context, IRGenerationFlag flag) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  bool isConstant() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

};
  
} /* namespace wisey */

#endif /* BooleanNotExpression_h */
