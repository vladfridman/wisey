//
//  StaticMethodDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef StaticMethodDefinition_h
#define StaticMethodDefinition_h

#include "wisey/AccessLevel.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/IMethodDefinition.hpp"
#include "wisey/IModelTypeSpecifier.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/MethodQualifier.hpp"
#include "wisey/VariableDeclaration.hpp"

namespace wisey {
  
  /**
   * Represents a concrete object's static method definition.
   *
   * Static method does not have access to object instance
   */
  class StaticMethodDefinition : public IMethodDefinition {
    const AccessLevel mAccessLevel;
    const ITypeSpecifier* mReturnTypeSpecifier;
    std::string mName;
    VariableList mArguments;
    std::vector<IModelTypeSpecifier*> mExceptions;
    MethodQualifiers* mMethodQualifiers;
    CompoundStatement* mCompoundStatement;
    int mLine;
    
  public:
    
    StaticMethodDefinition(const AccessLevel AccessLevel,
                           const ITypeSpecifier* returnTypeSpecifier,
                           std::string name,
                           VariableList arguments,
                           std::vector<IModelTypeSpecifier*> exceptions,
                           CompoundStatement* compoundStatement,
                           int line);

    ~StaticMethodDefinition();
    
    IMethod* define(IRGenerationContext& context, const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;

  };
  
} /* namespace wisey */

#endif /* StaticMethodDefinition_h */

