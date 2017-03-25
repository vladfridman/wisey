//
//  CastExpression.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef CastExpression_h
#define CastExpression_h

#include "yazyk/IExpression.hpp"
#include "yazyk/ITypeSpecifier.hpp"

namespace yazyk {

/**
 * Represents a cast operation
 */
class CastExpression : public IExpression {
  ITypeSpecifier& mTypeSpecifier;
  IExpression& mExpression;
  
public:
  
  CastExpression(ITypeSpecifier& typeSpecifier, IExpression& expression)
  : mTypeSpecifier(typeSpecifier), mExpression(expression) { }
  
  ~CastExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
};
  
} /* namespace yazyk */

#endif /* CastExpression_h */
