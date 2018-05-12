//
//  StaticMethodCall.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef StaticMethodCall_h
#define StaticMethodCall_h

#include "wisey/IMethodCall.hpp"
#include "wisey/IObjectTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a static method call of the form MModel.someMethod() where MModel is object type
   */
  class StaticMethodCall : public IMethodCall {
    IObjectTypeSpecifier* mObjectTypeSpecifier;
    std::string mMethodName;
    ExpressionList mArguments;
    int mLine;
    
  public:
    
    StaticMethodCall(IObjectTypeSpecifier* objectTypeSpecifier,
                     std::string methodName,
                     ExpressionList arguments,
                     int mLine);
    
    ~StaticMethodCall();
    
    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    bool checkAccess(IRGenerationContext& context, const IMethodDescriptor* methodDescriptor) const;
    
    const IMethodDescriptor* getMethodDescriptor(IRGenerationContext& context) const;
    
    void checkArgumentType(const IMethodDescriptor* methodDescriptor,
                           IRGenerationContext& context) const;
    
    llvm::Value* generateMethodCallIR(IRGenerationContext& context,
                                      const IMethodDescriptor* methodDescriptor,
                                      const IType* assignToType) const;
  };
  
} /* namespace wisey */

#endif /* StaticMethodCall_h */

