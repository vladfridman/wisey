//
//  MethodSignatureDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodSignatureDeclaration_h
#define MethodSignatureDeclaration_h

#include "wisey/AccessLevel.hpp"
#include "wisey/ExternalMethod.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/IModelTypeSpecifier.hpp"
#include "wisey/VariableDeclaration.hpp"

namespace wisey {
  
  /**
   * Represents a method signature declaration contained within an interface
   */
  class MethodSignatureDeclaration : public IObjectElementDefinition {
    const ITypeSpecifier* mReturnTypeSpecifier;
    std::string mMethodName;
    VariableList mArguments;
    std::vector<IModelTypeSpecifier*> mThrownExceptions;
    
  public:
    
    MethodSignatureDeclaration(const ITypeSpecifier* returnTypeSpecifier,
                               std::string methodName,
                               const VariableList& arguments,
                               std::vector<IModelTypeSpecifier*> thrownExceptions);
    
    ~MethodSignatureDeclaration();
    
    MethodSignature* define(IRGenerationContext& context,
                            const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
  };
  
} /* namespace wisey */

#endif /* MethodSignatureDeclaration_h */

