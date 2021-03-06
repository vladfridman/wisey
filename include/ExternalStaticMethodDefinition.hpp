//
//  ExternalStaticMethodDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalStaticMethodDefinition_h
#define ExternalStaticMethodDefinition_h

#include "ExternalStaticMethod.hpp"
#include "IMethodDefinition.hpp"
#include "IModelTypeSpecifier.hpp"
#include "ITypeSpecifier.hpp"
#include "MethodQualifier.hpp"

namespace wisey {
  
  /**
   * Represents a static method definition with an external implemention
   */
  class ExternalStaticMethodDefinition : public IMethodDefinition {
    const ITypeSpecifier* mReturnTypeSpecifier;
    std::string mName;
    VariableList mArguments;
    std::vector<IModelTypeSpecifier*> mThrownExceptions;
    MethodQualifiers* mMethodQualifiers;
    int mLine;
    
  public:
    
    ExternalStaticMethodDefinition(const ITypeSpecifier* returnTypeSpecifier,
                                   std::string name,
                                   const VariableList& arguments,
                                   std::vector<IModelTypeSpecifier*> thrownExceptions,
                                   int line);
    
    ~ExternalStaticMethodDefinition();
    
    ExternalStaticMethod* define(IRGenerationContext& context,
                                 const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;

  };
  
} /* namespace wisey */

#endif /* ExternalStaticMethodDefinition_h */

