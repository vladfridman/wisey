//
//  InjectedFieldDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef InjectedFieldDeclaration_h
#define InjectedFieldDeclaration_h

#include "wisey/IField.hpp"
#include "wisey/IObjectElementDefinition.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/InjectionArgument.hpp"

namespace wisey {
  
  class Field;
  
  /**
   * Represents a field in controller definition
   */
  class InjectedFieldDeclaration : public IObjectElementDefinition {
    const ITypeSpecifier* mTypeSpecifier;
    std::string mName;
    InjectionArgumentList mInjectionArgumentList;
    
  public:
    
    InjectedFieldDeclaration(const ITypeSpecifier* typeSpecifier,
                             std::string name,
                             InjectionArgumentList injectionArguments);
    
    ~InjectedFieldDeclaration();
    
    IField* define(IRGenerationContext& context, const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
  };
  
} /* namespace wisey */

#endif /* InjectedFieldDeclaration_h */

