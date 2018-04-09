//
//  LLVMFunctionDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMFunctionDefinition_h
#define LLVMFunctionDefinition_h

#include "wisey/CompoundStatement.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LLVMFunctionArgument.hpp"
#include "wisey/LLVMVariableDeclaration.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/IObjectElementDefinition.hpp"

namespace wisey {
  
  /**
   * Represents a concrete object's native llvm function definition.
   */
  class LLVMFunctionDefinition : public IObjectElementDefinition {
    std::string mName;
    AccessLevel mAccessLevel;
    const ITypeSpecifier* mReturnSpecifier;
    LLVMVariableList mArguments;
    CompoundStatement* mCompoundStatement;
    int mLine;
    
  public:
    
    LLVMFunctionDefinition(std::string name,
                           AccessLevel accessLevel,
                           const ITypeSpecifier* returnSpecifier,
                           LLVMVariableList arguments,
                           CompoundStatement* compoundStatement,
                           int line);

    ~LLVMFunctionDefinition();
    
    LLVMFunction* define(IRGenerationContext& context,
                         const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;

  private:
    
    std::vector<const LLVMFunctionArgument*> createArgumnetList(IRGenerationContext& context) const;

  };
  
} /* namespace wisey */

#endif /* LLVMFunctionDefinition_h */
