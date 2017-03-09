//
//  MethodCall.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodCall_h
#define MethodCall_h

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

  Model* getModel(IRGenerationContext& context) const;
  
  Method* getMethod(IRGenerationContext& context) const;
  
  void checkArgumentType(Model* model, Method* method, IRGenerationContext& context) const;

};

} /* namespace yazyk */

#endif /* MethodCall_h */
