//
//  MethodCall.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodCall_h
#define MethodCall_h

#include "wisey/IObjectWithMethodsType.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/Method.hpp"

namespace wisey {

/**
 * Represents a method call for an object that could be a model or an interface or a controller
 */
class MethodCall : public IExpression {
  const unsigned int VTABLE_METHODS_OFFSET = 2;
  
  IExpression& mExpression;
  std::string mMethodName;
  ExpressionList mArguments;
  
public:
  
  MethodCall(IExpression& expression, std::string methodName, ExpressionList& arguments)
  : mExpression(expression), mMethodName(methodName), mArguments(arguments) { }
  
  ~MethodCall() { };

  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  /**
   * Translate model method name into its LLVM implemenation function name
   */
  static std::string translateObjectMethodToLLVMFunctionName(IObjectWithMethodsType* object,
                                                             std::string methodName);

  /**
   * Translate model method name into its LLVM implemenation function name
   */
  static std::string translateInterfaceMethodToLLVMFunctionName(IObjectWithMethodsType* object,
                                                                const Interface* interface,
                                                                std::string methodName);

private:

  bool checkAccess(IRGenerationContext& context,
                   IObjectWithMethodsType* object,
                   IMethodDescriptor* methodDescriptor) const;
  
  llvm::Value* generateModelMethodCallIR(IRGenerationContext& context,
                                         IObjectWithMethodsType* object,
                                         IMethodDescriptor* methodDescriptor) const;
  
  llvm::Value* generateInterfaceMethodCallIR(IRGenerationContext& context,
                                             Interface* interface,
                                             IMethodDescriptor* methodDescriptor) const;
  
  IObjectWithMethodsType* getObjectWithMethods(IRGenerationContext& context) const;
  
  IMethodDescriptor* getMethodDescriptor(IRGenerationContext& context) const;
  
  void checkArgumentType(IObjectWithMethodsType* objectWithMethods,
                         IMethodDescriptor* methodDescriptor,
                         IRGenerationContext& context) const;

  llvm::Value* createFunctionCall(IRGenerationContext& context,
                                  llvm::Function* function,
                                  llvm::Type* returnType,
                                  IMethodDescriptor* methodDescriptor) const;
};

} /* namespace wisey */

#endif /* MethodCall_h */
