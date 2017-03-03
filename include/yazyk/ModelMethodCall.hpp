//
//  ModelMethodCall.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelMethodCall_h
#define ModelMethodCall_h

#include "yazyk/IExpression.hpp"
#include "yazyk/Method.hpp"

namespace yazyk {

/**
 * Represents a call to a model's method
 */
class ModelMethodCall : public IExpression {
  IExpression& mExpression;
  std::string mMethodName;
  ExpressionList mArguments;
  
public:
  
  ModelMethodCall(IExpression& expression, std::string methodName, ExpressionList& arguments)
  : mExpression(expression), mMethodName(methodName), mArguments(arguments) { }
  
  ~ModelMethodCall() { };

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
                                                                Interface* interface,
                                                                std::string methodName);

private:

  Model* getModel(IRGenerationContext& context) const;
  
  Method* getMethod(IRGenerationContext& context) const;
  
  void checkArgumentType(Model* model, Method* method, IRGenerationContext& context) const;

};

} /* namespace yazyk */

#endif /* ModelMethodCall_h */
