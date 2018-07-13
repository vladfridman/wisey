//
//  MethodCall.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodCall_h
#define MethodCall_h

#include "wisey/IMethodCall.hpp"
#include "wisey/IMethodDescriptor.hpp"
#include "wisey/IObjectType.hpp"

namespace wisey {
  
  /**
   * Represents a method call for an object that could be a model or an interface or a controller
   */
  class MethodCall : public IMethodCall {
    const unsigned int VTABLE_METHODS_OFFSET = 3;
    
    IExpression* mExpression;
    ExpressionList mArguments;
    bool mCanThrow;
    int mLine;
    
    MethodCall(IExpression* expression, ExpressionList arguments, bool canThrow, int line);

  public:
    
    ~MethodCall();
    
    /**
     * Creates an instance of this class that would translate into an invoke llvm instruction
     */
    static MethodCall* create(IExpression* expression,  ExpressionList arguments, int line);
    
    /**
     * Creates an instance of this class that would translate into a call llvm instruction
     */
    static MethodCall* createCantThrow(IExpression* expression, ExpressionList arguments, int line);

    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    /**
     * Translate interface method name into its LLVM implemenation function name
     */
    static std::string translateInterfaceMethodToLLVMFunctionName(const IObjectType* object,
                                                                  const Interface* interface,
                                                                  std::string methodName);
    
  private:
    
    llvm::Value* generateStaticMethodCallIR(IRGenerationContext& context,
                                            const IObjectType* object,
                                            const IMethodDescriptor* methodDescriptor,
                                            const IType* assignToType) const;
    
    llvm::Value* generateObjectMethodCallIR(IRGenerationContext& context,
                                            const IObjectType* object,
                                            const IMethodDescriptor* methodDescriptor,
                                            const IType* assignToType) const;
    
    llvm::Value* generateInterfaceMethodCallIR(IRGenerationContext& context,
                                               const Interface* interface,
                                               const IMethodDescriptor* methodDescriptor,
                                               const IType* assignToType) const;
    
    llvm::Value* generateLLVMFunctionCallIR(IRGenerationContext& context,
                                            const IObjectType* object,
                                            const IMethodDescriptor* methodDescriptor,
                                            const IType* assignToType) const;
    
    void checkArgumentType(const IObjectType* objectWithMethods,
                           const IMethodDescriptor* methodDescriptor,
                           IRGenerationContext& context) const;
    
    llvm::Value* createFunctionCall(IRGenerationContext& context,
                                    const IObjectType* object,
                                    llvm::Function* function,
                                    const IMethodDescriptor* methodDescriptor,
                                    std::vector<llvm::Value*> arguments,
                                    const IType* assignToType) const;
    
    llvm::Function* getMethodFunction(IRGenerationContext& context,
                                      const IMethodDescriptor* methodDescriptor) const;
    
    const IMethodDescriptor* getMethodDescriptor(IRGenerationContext& context) const;
    
  };
  
} /* namespace wisey */

#endif /* MethodCall_h */

