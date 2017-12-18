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
  
  IVariable* getVariable(IRGenerationContext& context,
                         std::vector<const IExpression*>& arrayIndices) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context, IRGenerationFlag flag) const override;
  
  const IType* getType(IRGenerationContext& context) const override;

  bool isConstant() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

private:
  
  bool checkAccess(IRGenerationContext& context, IMethodDescriptor* methodDescriptor) const;
  
  IMethodDescriptor* getMethodDescriptor(IRGenerationContext& context) const;
  
  void checkArgumentType(IMethodDescriptor* methodDescriptor, IRGenerationContext& context) const;
  
  llvm::Value* generateMethodCallIR(IRGenerationContext& context,
                                    IMethodDescriptor* methodDescriptor,
                                    IRGenerationFlag flag) const;
};
  
} /* namespace wisey */

#endif /* StaticMethodCall_h */
