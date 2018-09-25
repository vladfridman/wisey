//
//  IdentifierChain.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodOrPackage_h
#define MethodOrPackage_h

#include "IExpression.hpp"
#include "IObjectType.hpp"

namespace wisey {
  
  /**
   * Represents an identifier chain that could either be a package id or a method reference
   *
   * If the chain consists of two elements and the first element is of object type
   * e.g. objectA.methodB then it is a method reference
   * If the chain is of two elements and the first element is of undefined type then it is
   * interpreted as a package name e.g. wisey.lang
   * If the chain consists of three or more elements then it is interpreted as a package name
   */
  class IdentifierChain : public IExpression {
    
    const IExpression* mObjectExpression;
    const std::string mName;
    int mLine;
    
  public:
    
    IdentifierChain(const IExpression* objectExpression, std::string name, int line);
    
    ~IdentifierChain();
    
    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    bool checkAccess(IRGenerationContext& context, const IMethodDescriptor* methodDescriptor) const;
    
    const IMethodDescriptor* getMethodDescriptor(IRGenerationContext& context) const;
    
  };
  
}

#endif /* IdentifierChain_h */

