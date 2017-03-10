//
//  MethodCall.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodCall_h
#define MethodCall_h

#include "yazyk/ICallableObjectType.hpp"
#include "yazyk/IExpression.hpp"
#include "yazyk/Method.hpp"

namespace yazyk {

/**
 * Represents a method call for an object that could be a model or an interface or a controller
 */
class MethodCall : public IExpression {
  IExpression& mExpression;
  std::string mMethodName;
  ExpressionList mArguments;
  
public:
  
  MethodCall(IExpression& expression, std::string methodName, ExpressionList& arguments)
  : mExpression(expression), mMethodName(methodName), mArguments(arguments) { }
  
  ~MethodCall() { };

  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;
  
  /**
   * Translate model method name into its LLVM implemenation function name
   */
  static std::string translateModelMethodToLLVMFunctionName(Model* model, std::string methodName);

  /**
   * Translate model method name into its LLVM implemenation function name
   */
  static std::string translateInterfaceMethodToLLVMFunctionName(Model* model,
                                                                const Interface* interface,
                                                                std::string methodName);

private:

  llvm::Value* generateModelMethodCallIR(IRGenerationContext& context,
                                         Model* model,
                                         IMethodDescriptor* methodDescriptor) const;
  
  llvm::Value* generateInterfaceMethodCallIR(IRGenerationContext& context,
                                             Interface* interface,
                                             IMethodDescriptor* methodDescriptor) const;
  
  ICallableObjectType* getCallableObject(IRGenerationContext& context) const;
  
  IMethodDescriptor* getMethodDescriptor(IRGenerationContext& context) const;
  
  void checkArgumentType(ICallableObjectType* model,
                         IMethodDescriptor* methodDescriptor,
                         IRGenerationContext& context) const;

  llvm::CallInst* createFunctionCall(IRGenerationContext& context,
                                     llvm::Function* function,
                                     llvm::Type* returnType,
                                     IMethodDescriptor* methodDescriptor) const;
};

} /* namespace yazyk */

#endif /* MethodCall_h */
