//
//  ExternalMethodDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalMethodDefinition_h
#define ExternalMethodDefinition_h

#include "wisey/AccessLevel.hpp"
#include "wisey/ExternalMethod.hpp"
#include "wisey/IMethodDefinition.hpp"
#include "wisey/IModelTypeSpecifier.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/MethodQualifier.hpp"
#include "wisey/VariableDeclaration.hpp"

namespace wisey {
  
  /**
   * Represents method definition for an externally implemented object
   */
  class ExternalMethodDefinition : public IMethodDefinition {
    const ITypeSpecifier* mReturnTypeSpecifier;
    std::string mName;
    VariableList mArguments;
    std::vector<IModelTypeSpecifier*> mThrownExceptions;
    MethodQualifiers* mMethodQualifiers;
    int mLine;
    
  public:
    
    ExternalMethodDefinition(const ITypeSpecifier* returnTypeSpecifier,
                             std::string name,
                             const VariableList& arguments,
                             std::vector<IModelTypeSpecifier*> thrownExceptions,
                             MethodQualifiers* methodQualifiers,
                             int line);

    ~ExternalMethodDefinition();
    
    ExternalMethod* define(IRGenerationContext& context,
                           const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;

  };
  
} /* namespace wisey */

#endif /* ExternalMethodDefinition_h */

