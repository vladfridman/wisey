//
//  ConstantDeclaraton.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ConstantDeclaraton_h
#define ConstantDeclaraton_h

#include "AccessLevel.hpp"
#include "Constant.hpp"
#include "IExpression.hpp"
#include "IObjectElementDefinition.hpp"
#include "ITypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a constant declaration
   */
  class ConstantDefinition : public IObjectElementDefinition {
    
    const AccessLevel mAccessLevel;
    const ITypeSpecifier* mTypeSpecifier;
    std::string mName;
    IExpression* mExpression;
    int mLine;
    
  public:
    
    ConstantDefinition(const AccessLevel accessLevel,
                       const ITypeSpecifier* typeSpecifier,
                       std::string name,
                       IExpression* expression,
                       int line);

    ~ConstantDefinition();
    
    Constant* define(IRGenerationContext& context, const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;

  };
  
} /* namespace wisey */

#endif /* ConstantDeclaraton_h */

