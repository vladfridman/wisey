//
//  StateFieldDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef StateFieldDefinition_h
#define StateFieldDefinition_h

#include "IField.hpp"
#include "IObjectElementDefinition.hpp"
#include "ITypeSpecifier.hpp"
#include "InjectionArgument.hpp"

namespace wisey {
  
  class Field;
  
  /**
   * Represents a state field declaration in object definition
   */
  class StateFieldDefinition : public IObjectElementDefinition {
    const ITypeSpecifier* mTypeSpecifier;
    std::string mName;
    int mLine;
    
  public:
    
    StateFieldDefinition(const ITypeSpecifier* typeSpecifier, std::string name, int line);
    
    ~StateFieldDefinition();
    
    IField* define(IRGenerationContext& context, const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;

  };
  
} /* namespace wisey */

#endif /* StateFieldDefinition_h */

