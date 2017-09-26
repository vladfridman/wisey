//
//  StaticMethodCall.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef StaticMethodCall_h
#define StaticMethodCall_h

#include "wisey/IObjectTypeSpecifier.hpp"
#include "wisey/IExpression.hpp"

namespace wisey {

/**
 * Represents a static method call of the form MModel.someMethod() where MModel is object type
 */
class StaticMethodCall : public IExpression {
  IObjectTypeSpecifier* mObjectTypeSpecifier;
  std::string mMethodName;
  ExpressionList mArguments;
  
public:
  
  StaticMethodCall(IObjectTypeSpecifier* objectTypeSpecifier,
                   std::string methodName,
                   ExpressionList arguments)
  : mObjectTypeSpecifier(objectTypeSpecifier),
  mMethodName(methodName),
  mArguments(arguments) { }
  
  ~StaticMethodCall();
  
  IVariable* getVariable(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
  void addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

private:
  
  bool checkAccess(IRGenerationContext& context, IMethodDescriptor* methodDescriptor) const;
  
  IMethodDescriptor* getMethodDescriptor(IRGenerationContext& context) const;
  
  void checkArgumentType(IMethodDescriptor* methodDescriptor, IRGenerationContext& context) const;
  
  llvm::Value* generateMethodCallIR(IRGenerationContext& context,
                                    IMethodDescriptor* methodDescriptor) const;
};
  
} /* namespace wisey */

#endif /* StaticMethodCall_h */
