//
//  IMethod.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Composer.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/Names.hpp"
#include "wisey/ParameterPrimitiveVariable.hpp"
#include "wisey/ParameterSystemReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

void IMethod::storeArgumentValue(IRGenerationContext& context,
                                 string name,
                                 const IType* type,
                                 Value* value,
                                 int line) {
  type->createParameterVariable(context, name, value, line);
}

void IMethod::storeSystemArgumentValue(IRGenerationContext& context,
                                       string name,
                                       const IObjectType* type,
                                       Value* value) {
  IVariable* variable = new ParameterSystemReferenceVariable(name, type, value, 0);
  context.getScopes().setVariable(context, variable);
}

void IMethod::checkForUnhandledExceptions(IRGenerationContext& context,
                                          const IMethod* method,
                                          int line) {
  Scope* scope = context.getScopes().getScope();
  map<string, int> exceptions = scope->getExceptions();
  if (exceptions.size() == 0) {
    return;
  }
  
  for (const Model* thrownException : method->getThrownExceptions()) {
    scope->removeException(thrownException);
  }
  
  exceptions = scope->getExceptions();
  bool hasUnhandledExceptions = false;
  for (auto iterator = exceptions.begin(); iterator != exceptions.end(); iterator++) {
    if (!iterator->first.find(Names::getLangPackageName()) ||
        !iterator->first.find(Names::getThreadsPackageName()) ||
        !iterator->first.find(Names::getIOPackageName())) {
      continue;
    }
    string prefix = (method->isStatic() ? "Static method " : "Method ");
    context.reportError(iterator->second,
                        prefix + method->getName() + " neither handles the exception " +
                        iterator->first + " nor throws it");
    hasUnhandledExceptions = true;
  }
  
  if (hasUnhandledExceptions) {
    throw 1;
  }
}

void IMethod::maybeAddImpliedVoidReturn(IRGenerationContext& context,
                                        const IMethod* method,
                                        int line) {
  if (context.getBasicBlock()->getTerminator()) {
    return;
  }
  
  const IType* returnType = method->getReturnType();
  if (returnType != PrimitiveTypes::VOID) {
    context.reportError(line,
                        (method->isStatic() ? "Static method " : "Method ") + method->getName() +
                        " must return a value of type " + returnType->getTypeName());
    throw 1;
  }
  
  context.getScopes().freeOwnedMemory(context, NULL, line);
  Composer::popCallStack(context);
  IRWriter::createReturnInst(context, NULL);
}

Function* IMethod::declareFunctionForObject(IRGenerationContext& context,
                                            const IObjectType* objectType,
                                            const IMethod* method) {
  FunctionType* ftype = method->getLLVMType(context);
  string methodName = method->getName();
  string functionName = IMethodCall::translateObjectMethodToLLVMFunctionName(objectType,
                                                                             methodName);
  
  GlobalValue::LinkageTypes linkageType = method->isPublic()
    ? GlobalValue::ExternalLinkage
    : GlobalValue::InternalLinkage;
  Function* function = Function::Create(ftype, linkageType, functionName, context.getModule());
  
  return function;
}
