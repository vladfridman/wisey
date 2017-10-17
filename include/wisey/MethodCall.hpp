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
  std::string mMethodName;
  ExpressionList mArguments;
  int mLine;
  
public:
  
  MethodCall(IExpression* expression, std::string methodName, ExpressionList arguments, int line);
  
  ~MethodCall();
  
  IVariable* getVariable(IRGenerationContext& context) const override;

  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
  void addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

  /**
   * Translate interface method name into its LLVM implemenation function name
   */
  static std::string translateInterfaceMethodToLLVMFunctionName(const IObjectType* object,
                                                                const Interface* interface,
                                                                std::string methodName);

private:

  bool checkAccess(IRGenerationContext& context,
                   const IObjectType* object,
                   IMethodDescriptor* methodDescriptor) const;
  
  llvm::Value* generateStaticMethodCallIR(IRGenerationContext& context,
                                          llvm::Value* threadObject,
                                          const IObjectType* objectType,
                                          IMethodDescriptor* methodDescriptor) const;
  
  llvm::Value* generateObjectMethodCallIR(IRGenerationContext& context,
                                          llvm::Value* threadObject,
                                          const IObjectType* objectType,
                                          IMethodDescriptor* methodDescriptor) const;
  
  llvm::Value* generateInterfaceMethodCallIR(IRGenerationContext& context,
                                             llvm::Value* threadObject,
                                             const Interface* interface,
                                             IMethodDescriptor* methodDescriptor) const;
  
  llvm::Function* getMethodFunction(IRGenerationContext& context,
                                    const IObjectType* object) const;
  
  llvm::Value* generateExpressionIR(IRGenerationContext& context) const;
  
  const IObjectType* getObjectWithMethods(IRGenerationContext& context) const;
  
  IMethodDescriptor* getMethodDescriptor(IRGenerationContext& context) const;
  
  void checkArgumentType(const IObjectType* objectWithMethods,
                         IMethodDescriptor* methodDescriptor,
                         IRGenerationContext& context) const;

  llvm::Value* createFunctionCall(IRGenerationContext& context,
                                  const IObjectType* object,
                                  llvm::Value* threadObject,
                                  llvm::Function* function,
                                  IMethodDescriptor* methodDescriptor,
                                  std::vector<llvm::Value*> arguments,
                                  llvm::Value* expressionValue) const;

};

} /* namespace wisey */

#endif /* MethodCall_h */
