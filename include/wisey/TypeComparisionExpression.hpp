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
   * Represents an m expression for type comparision
   */
  class TypeComparisionExpression : public IExpression {
    const IExpression* mExpression;
    const ITypeSpecifier* mTypeSpecifier;
    int mLine;
    
  public:
    
    TypeComparisionExpression(const IExpression* expression,
                              const ITypeSpecifier* typeSpecifier,
                              int line);
    
    ~TypeComparisionExpression();
    
    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    llvm::Value* checkInterfaceImplemented(IRGenerationContext& context,
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

