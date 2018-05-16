//
//  ExternalConstantDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalConstantDeclaration_h
#define ExternalConstantDeclaration_h

#include "wisey/Constant.hpp"
#include "wisey/IObjectElementDefinition.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a constant defined in an external object
   */
  class ExternalConstantDeclaration : public IObjectElementDefinition {
    
    const ITypeSpecifier* mTypeSpecifier;
    std::string mName;
    int mLine;
    
  public:
    
    ExternalConstantDeclaration(const ITypeSpecifier* typeSpecifier,
                                std::string name,
                                int line);
    
    ~ExternalConstantDeclaration();
    
    Constant* define(IRGenerationContext& context, const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;
    
  };
  
} /* namespace wisey */

#endif /* ExternalConstantDeclaration_h */
