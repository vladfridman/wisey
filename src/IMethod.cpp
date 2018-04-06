//
//  IMethod.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

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
                                 Value* value) {
  type->createParameterVariable(context, name, value);
}

void IMethod::storeSystemArgumentValue(IRGenerationContext& context,
                                       string name,
                                       const IObjectType* type,
                                       Value* value) {
  IVariable* variable = new ParameterSystemReferenceVariable(name, type, value);
  context.getScopes().setVariable(variable);
}

void IMethod::checkForUnhandledExceptions(IRGenerationContext& context, const IMethod* method) {
  Scope* scope = context.getScopes().getScope();
  map<string, const Model*> exceptions = scope->getExceptions();
  if (exceptions.size() == 0) {
    return;
  }
  
  for (const Model* thrownException : method->getThrownExceptions()) {
    scope->removeException(thrownException);
  }
  
  exceptions = scope->getExceptions();
  bool hasUnhandledExceptions = false;
  for (map<string, const Model*>::const_iterator iterator = exceptions.begin();
       iterator != exceptions.end();
       iterator++) {
    if (!iterator->first.find(Names::getLangPackageName())) {
      continue;
    }
    Log::e_deprecated((method->isStatic() ? "Static method " : "Method ") + method->getName() +
           " neither handles the exception " + iterator->first + " nor throws it");
    hasUnhandledExceptions = true;
  }
  
  if (hasUnhandledExceptions) {
    exit(1);
  }
}

void IMethod::maybeAddImpliedVoidReturn(IRGenerationContext& context,
                                        const IMethod* method,
                                        int line) {
  if (context.getBasicBlock()->getTerminator()) {
    return;
  }
  
  const IType* returnType = method->getReturnType();
  if (returnType != PrimitiveTypes::VOID_TYPE) {
    Log::e_deprecated((method->isStatic() ? "Static method " : "Method ") + method->getName() +
           " must return a value of type " + returnType->getTypeName());
    exit(1);
  }
  
  context.getScopes().freeOwnedMemory(context, line);
  IRWriter::createReturnInst(context, NULL);
}

Function* IMethod::defineFunction(IRGenerationContext& context,
                                  const IObjectType* objectType,
                                  const IMethod* method) {
  FunctionType* ftype = method->getLLVMType(context);
  string methodName = method->getName();
  string functionName = IMethodCall::translateObjectMethodToLLVMFunctionName(objectType,
                                                                             methodName);
  
  GlobalValue::LinkageTypes linkageType = method->getAccessLevel() == PRIVATE_ACCESS
    ? GlobalValue::InternalLinkage
    : GlobalValue::ExternalLinkage;
  Function* function = Function::Create(ftype, linkageType, functionName, context.getModule());
  
  string functionNameConstantName = MethodCall::getMethodNameConstantName(methodName);
  if (!context.getModule()->getNamedGlobal(functionNameConstantName)) {
    llvm::Constant* stringConstant =
      ConstantDataArray::getString(context.getLLVMContext(), methodName);
    new GlobalVariable(*context.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::InternalLinkage,
                       stringConstant,
                       functionNameConstantName);
  }
  
  return function;
}

void IMethod::defineCurrentMethodNameVariable(IRGenerationContext& context, string methodName) {
  string constantName = IMethodCall::getMethodNameConstantName(methodName);
  GlobalVariable* constant = context.getModule()->getNamedGlobal(constantName);
  ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
  Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  Type* elementType = constant->getType()->getPointerElementType();
  
  Value* value = ConstantExpr::getGetElementPtr(elementType, constant, Idx);
  
  ParameterPrimitiveVariable* methodNameVariable =
  new ParameterPrimitiveVariable(Names::getCurrentMethodVariableName(),
                                 PrimitiveTypes::STRING_TYPE,
                                 value);
  context.getScopes().setVariable(methodNameVariable);
}
