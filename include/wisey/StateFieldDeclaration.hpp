//
//  StateFieldDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef StateFieldDeclaration_h
#define StateFieldDeclaration_h

#include "wisey/IField.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/InjectionArgument.hpp"

namespace wisey {
  
  class Field;
  
  /**
   * Represents a state field declaration in object definition
   */
  class StateFieldDeclaration : public IObjectElementDeclaration {
    ITypeSpecifier* mTypeSpecifier;
    std::string mName;
    
  public:
    
    StateFieldDeclaration(ITypeSpecifier* typeSpecifier, std::string name);
    
    ~StateFieldDeclaration();
    
    IField* declare(IRGenerationContext& context, const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
  };
  
} /* namespace wisey */

#endif /* StateFieldDeclaration_h */

