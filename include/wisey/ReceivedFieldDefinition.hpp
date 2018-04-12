//
//  ReceivedFieldDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ReceivedFieldDefinition_h
#define ReceivedFieldDefinition_h

#include "wisey/IField.hpp"
#include "wisey/IObjectElementDefinition.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/InjectionArgument.hpp"

namespace wisey {
  
  class Field;
  
  /**
   * Represents a state field declaration in object definition
   */
  class ReceivedFieldDefinition : public IObjectElementDefinition {
    const ITypeSpecifier* mTypeSpecifier;
    std::string mName;
    int mLine;
    
  public:
    
    ReceivedFieldDefinition(const ITypeSpecifier* typeSpecifier, std::string name, int line);
    
    ~ReceivedFieldDefinition();
    
    IField* define(IRGenerationContext& context, const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;
    
  };
  
} /* namespace wisey */

#endif /* ReceivedFieldDefinition_h */

