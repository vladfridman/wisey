//
//  ConstantDeclaraton.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ConstantDeclaraton_h
#define ConstantDeclaraton_h

#include "wisey/AccessLevel.hpp"
#include "wisey/Constant.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IObjectElementDefinition.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a constant declaration
   */
  class ConstantDeclaration : public IObjectElementDefinition {
    
    const AccessLevel mAccessLevel;
    const ITypeSpecifier* mTypeSpecifier;
    std::string mName;
    IExpression* mExpression;
    
  public:
    
    ConstantDeclaration(const AccessLevel accessLevel,
                        const ITypeSpecifier* typeSpecifier,
                        std::string name,
                        IExpression* expression);
    
    ~ConstantDeclaration();
    
    Constant* define(IRGenerationContext& context, const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
  };
  
} /* namespace wisey */

#endif /* ConstantDeclaraton_h */

