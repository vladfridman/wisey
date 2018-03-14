//
//  IdentifierChain.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodOrPackage_h
#define MethodOrPackage_h

#include "wisey/IExpression.hpp"
#include "wisey/IObjectType.hpp"

namespace wisey {
  
  /**
   * Represents an identifier chain that could either be a package id or a method reference
   *
   * If the chain consists of two elements and the first element is of object type
   * e.g. objectA.methodB then it is a method reference
   * If the chain is of two elements and the first element is of undefined type then it is interpreted
   * as a package name e.g. wisey.lang
   * If the chain consists of three or more elements then it is interpreted as a package name
   */
  class IdentifierChain : public IExpression {
    
    IExpression* mObjectExpression;
    const std::string mName;
    
  public:
    
    IdentifierChain(IExpression* objectExpression, const std::string name);
    
    ~IdentifierChain();
    
    IVariable* getVariable(IRGenerationContext& context,
                           std::vector<const IExpression*>& arrayIndices) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    bool checkAccess(IRGenerationContext& context, IMethodDescriptor* methodDescriptor) const;
    
    IMethodDescriptor* getMethodDescriptor(IRGenerationContext& context) const;
    
  };
  
}

#endif /* IdentifierChain_h */

