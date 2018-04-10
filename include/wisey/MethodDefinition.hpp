//
//  MethodDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef MethodDefinition_h
#define MethodDefinition_h

#include "wisey/AccessLevel.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/IMethodDefinition.hpp"
#include "wisey/IModelTypeSpecifier.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/MethodQualifier.hpp"
#include "wisey/VariableDeclaration.hpp"

namespace wisey {
  
  /**
   * Represents a concrete object's method definition.
   *
   * A method contains a coumpound statement that is the body of the method.
   */
  class MethodDefinition : public IMethodDefinition {
    const AccessLevel mAccessLevel;
    const ITypeSpecifier* mReturnTypeSpecifier;
    std::string mName;
    VariableList mArguments;
    std::vector<IModelTypeSpecifier*> mExceptions;
    MethodQualifiers* mMethodQualifiers;
    CompoundStatement* mCompoundStatement;
    int mLine;
    
  public:
    
    MethodDefinition(const AccessLevel AccessLevel,
                      const ITypeSpecifier* returnTypeSpecifier,
                      std::string name,
                      VariableList arguments,
                      std::vector<IModelTypeSpecifier*> exceptions,
                      MethodQualifiers* methodQualifiers,
                      CompoundStatement* compoundStatement,
                      int line);
    
    ~MethodDefinition();
    
    IMethod* define(IRGenerationContext& context, const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;

  };
  
} /* namespace wisey */

#endif /* MethodDefinition_h */

