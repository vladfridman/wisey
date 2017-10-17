//
//  IMethodCall.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IMethodCall_h
#define IMethodCall_h

#include "wisey/IExpression.hpp"
#include "wisey/IObjectType.hpp"

namespace wisey {
  
/**
 * Represents a method call that could either be static or non-static method call
 */
class IMethodCall : public IExpression {
  
public:
  
  /**
   * Push a method call on stack
   */
  static void pushCallStack(IRGenerationContext& context,
                            const IObjectType* object,
                            std::string methodName,
                            llvm::Value* expressionValue,
                            llvm::Value* threadObject,
                            int line);

  /**
   * Pop a method call from stack
   */
  static void popCallStack(IRGenerationContext& context,
                           const IObjectType* object,
                           llvm::Value* threadObject);
  
  /**
   * Translate object method name into its LLVM implemenation function name
   */
  static std::string translateObjectMethodToLLVMFunctionName(const IObjectType* object,
                                                             std::string methodName);

  /**
   * Return name of the global constant containing method name
   */
  static std::string getMethodNameConstantName(std::string methodName);

private:
  
  static llvm::Constant* getMethodNameConstantPointer(IRGenerationContext& context,
                                                      std::string methodName);
  
  static llvm::Value* getObjectNamePointer(IRGenerationContext& context,
                                           const IObjectType* object,
                                           llvm::Value* expressionValue);

};
  
} /* namespace wisey */

#endif /* IMethodCall_h */
