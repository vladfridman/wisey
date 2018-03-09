//
//  ReceivedFieldDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ReceivedFieldDeclaration_h
#define ReceivedFieldDeclaration_h

#include "wisey/IField.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/InjectionArgument.hpp"

namespace wisey {
  
  class Field;
  
  /**
   * Represents a state field declaration in object definition
   */
  class ReceivedFieldDeclaration : public IObjectElementDeclaration {
    ITypeSpecifier* mTypeSpecifier;
    std::string mName;
    
  public:
    
    ReceivedFieldDeclaration(ITypeSpecifier* typeSpecifier, std::string name);
    
    ~ReceivedFieldDeclaration();
    
    IField* declare(IRGenerationContext& context, const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
  };
  
} /* namespace wisey */

#endif /* ReceivedFieldDeclaration_h */
