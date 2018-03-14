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
    
    TypeComparisionExpression(IExpression* expression, ITypeSpecifier* typeSpecifier);
    
    ~TypeComparisionExpression();
    
    IVariable* getVariable(IRGenerationContext& context,
                           std::vector<const IExpression*>& arrayIndices) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    llvm::Value* checkInterfaceImplemented(IRGenerationContext& context,
                                           const IType* expressionType,
                                           const IObjectType* objectType) const;
    
    llvm::Value* generateIRforOwnerTypes(IRGenerationContext& context,
                                         const IObjectOwnerType* expressionType,
                                         const IObjectOwnerType* type) const;
    
    llvm::Value* generateIRforPointerTypes(IRGenerationContext& context,
                                           const IObjectType* expressionType,
                                           const IObjectType* type) const;
  };
  
} /* namespace wisey */

#endif /* TypeComparisionExpression_h */

