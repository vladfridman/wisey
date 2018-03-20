//
//  FixedFieldDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef FixedFieldDeclaration_h
#define FixedFieldDeclaration_h

#include "wisey/IField.hpp"
#include "wisey/IObjectElementDefinition.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/InjectionArgument.hpp"

namespace wisey {
  
  class Field;
  
  /**
   * Represents a fixed field declaration in object definition
   */
  class FixedFieldDeclaration : public IObjectElementDefinition {
    const ITypeSpecifier* mTypeSpecifier;
    std::string mName;
    
  public:
    
    FixedFieldDeclaration(const ITypeSpecifier* typeSpecifier, std::string name);
    
    ~FixedFieldDeclaration();
    
    IField* define(IRGenerationContext& context, const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
  };
  
} /* namespace wisey */

#endif /* FixedFieldDeclaration_h */

