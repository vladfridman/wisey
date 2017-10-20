//
//  TypeComparisionExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef TypeComparisionExpression_h
#define TypeComparisionExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {

/**
 * Represents an instanceof expression for type comparision
 */
class TypeComparisionExpression : public IExpression {
  IExpression* mExpression;
  ITypeSpecifier* mTypeSpecifier;
  
public:
  
  TypeComparisionExpression(IExpression* expression, ITypeSpecifier* typeSpecifier) :
  mExpression(expression), mTypeSpecifier(typeSpecifier) { }
  
  
  ~TypeComparisionExpression();
  
  IVariable* getVariable(IRGenerationContext& context) const override;

  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
  void addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const override;
  
  bool isConstant() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

private:
  
  llvm::Value* checkInterfaceImplemented(IRGenerationContext& context,
                                         const IType* expressionType,
                                         const IObjectType* objectType) const;
  
  llvm::Value* generateIRforOwnerTypes(IRGenerationContext& context,
                                       const IObjectOwnerType* expressionType,
                                       const IObjectOwnerType* type) const;
  
  llvm::Value* generateIRforReferenceTypes(IRGenerationContext& context,
                                           const IObjectType* expressionType,
                                           const IObjectType* type) const;
};

} /* namespace wisey */

#endif /* TypeComparisionExpression_h */
