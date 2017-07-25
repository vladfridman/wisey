//
//  CastExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef CastExpression_h
#define CastExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {

/**
 * Represents a cast operation
 */
class CastExpression : public IExpression {
  ITypeSpecifier* mTypeSpecifier;
  IExpression* mExpression;
  
public:
  
  CastExpression(ITypeSpecifier* typeSpecifier, IExpression* expression)
  : mTypeSpecifier(typeSpecifier), mExpression(expression) { }
  
  ~CastExpression();
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
  void addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const override;
 
};
  
} /* namespace wisey */

#endif /* CastExpression_h */
