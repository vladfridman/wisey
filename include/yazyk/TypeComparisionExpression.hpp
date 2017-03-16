//
//  TypeComparisionExpression.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef TypeComparisionExpression_h
#define TypeComparisionExpression_h

#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/ITypeSpecifier.hpp"

namespace yazyk {

/**
 * Represents an instanceof expression for type comparision
 */
class TypeComparisionExpression : public IExpression {
  IExpression& mExpression;
  ITypeSpecifier& mTypeSpecifier;
  
public:
  
  TypeComparisionExpression(IExpression& expression, ITypeSpecifier& typeSpecifier) :
  mExpression(expression), mTypeSpecifier(typeSpecifier) { }
  
  
  ~TypeComparisionExpression() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;
  
private:
  
  llvm::Value* checkInterfaceImplemented(IRGenerationContext& context) const;
};

} /* namespace yazyk */

#endif /* TypeComparisionExpression_h */
