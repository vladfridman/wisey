//
//  MethodSignatureDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodSignatureDeclaration_h
#define MethodSignatureDeclaration_h

#include "ITypeSpecifier.hpp"
#include "IModelTypeSpecifier.hpp"
#include "MethodQualifier.hpp"
#include "VariableDeclaration.hpp"

namespace wisey {
  
  /**
   * Represents a method signature declaration contained within an interface
   */
  class MethodSignatureDeclaration : public IObjectElementDefinition {
    const ITypeSpecifier* mReturnTypeSpecifier;
    std::string mMethodName;
    VariableList mArguments;
    std::vector<IModelTypeSpecifier*> mThrownExceptions;
    MethodQualifiers* mMethodQualifiers;
    int mLine;
    
  public:
    
    MethodSignatureDeclaration(const ITypeSpecifier* returnTypeSpecifier,
                               std::string methodName,
                               const VariableList& arguments,
                               std::vector<IModelTypeSpecifier*> thrownExceptions,
                               MethodQualifiers* methodQualifiers,
                               int line);
    
    ~MethodSignatureDeclaration();
    
    MethodSignature* define(IRGenerationContext& context,
                            const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;

  };
  
} /* namespace wisey */

#endif /* MethodSignatureDeclaration_h */

