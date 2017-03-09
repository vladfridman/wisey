//
//  MethodCall.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/AutoCast.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/MethodCall.hpp"

using namespace std;
using namespace llvm;
using namespace yazyk;

Value* MethodCall::generateIR(IRGenerationContext& context) const {
  ICallableObjectType* callableObject = getCallableObject(context);
  Method* method = getMethod(context);
  checkArgumentType(callableObject, method, context);
  if (callableObject->getTypeKind() == MODEL_TYPE) {
    return generateModelMethodCallIR(context, (Model*) callableObject, method);
  }
  if (callableObject->getTypeKind() == INTERFACE_TYPE) {
    return generateInterfaceMethodCallIR(context, (Interface*) callableObject, method);
  }
  Log::e("Method '" + mMethodName + "()' call on an unknown object type '" +
         callableObject->getName() + "'");
  exit(1);
}

Value* MethodCall::generateInterfaceMethodCallIR(IRGenerationContext& context,
                                                 Interface* interface,
                                                 Method* method) const {
  Log::e("Interface methods calls are not yet implemented");
  exit(1);
  // TODO: implement interface method calls
  return NULL;
}

Value* MethodCall::generateModelMethodCallIR(IRGenerationContext& context,
                                             Model* model,
                                             Method* method) const {
  string llvmFunctionName = translateModelMethodToLLVMFunctionName(model, mMethodName);
  
  Function *function = context.getModule()->getFunction(llvmFunctionName.c_str());
  if (function == NULL) {
    Log::e("LLVM function implementing model '" + model->getName() + "' method '" +
           mMethodName + "' was not found");
    exit(1);
  }
  vector<Value*> arguments;
  arguments.push_back(mExpression.generateIR(context));
  vector<MethodArgument*> methodArguments = method->getArguments();
  vector<MethodArgument*>::iterator methodArgumentIterator = methodArguments.begin();
  for (IExpression* callArgument : mArguments) {
    Value* callArgumentValue = callArgument->generateIR(context);
    IType* callArgumentType = callArgument->getType(context);
    MethodArgument* methodArgument = *methodArgumentIterator;
    IType* methodArgumentType = methodArgument->getType();
    Value* callArgumentValueCasted = AutoCast::maybeCast(context,
                                                         callArgumentType,
                                                         callArgumentValue,
                                                         methodArgumentType);
    arguments.push_back(callArgumentValueCasted);
    methodArgumentIterator++;
  }
  string resultName = function->getReturnType()->isVoidTy() ? "" : "call";
  
  return CallInst::Create(function, arguments, resultName, context.getBasicBlock());
}

IType* MethodCall::getType(IRGenerationContext& context) const {
  return getMethod(context)->getReturnType();
}

ICallableObjectType* MethodCall::getCallableObject(IRGenerationContext& context) const {
  IType* expressionType = mExpression.getType(context);
  if (expressionType->getTypeKind() == PRIMITIVE_TYPE) {
    Log::e("Attempt to call a method '" + mMethodName + "' on a primitive type expression");
    exit(1);
  }
  if (expressionType->getTypeKind() == INTERFACE_TYPE ||
      expressionType->getTypeKind() == MODEL_TYPE) {
    return (ICallableObjectType*) expressionType;
  }
  Log::e("Method '" + mMethodName + "()' call on a non-callable object type '" +
         expressionType->getName() + "'");
  exit(1);
}

Method* MethodCall::getMethod(IRGenerationContext& context) const {
  ICallableObjectType* callableObject = getCallableObject(context);
  Method* method = callableObject->findMethod(mMethodName);
  if (method == NULL) {
    Log::e("Method '" + mMethodName + "' is not found in callable object '" +
           callableObject->getName() + "'");
    exit(1);
  }
  
  return method;
}

void MethodCall::checkArgumentType(ICallableObjectType* callableObject,
                                   Method* method,
                                   IRGenerationContext& context) const {
  vector<MethodArgument*> methodArguments = method->getArguments();
  vector<IExpression*>::const_iterator callArgumentsIterator = mArguments.begin();
  
  if (mArguments.size() != method->getArguments().size()) {
    Log::e("Number of arguments for method call '" + method->getName() +
           "' of the model type '" + callableObject->getName() + "' is not correct");
    exit(1);
  }
  
  for (MethodArgument* methodArgument : methodArguments) {
    IType* methodArgumentType = methodArgument->getType();
    IType* callArgumentType = (*callArgumentsIterator)->getType(context);
    
    if (!callArgumentType->canCastLosslessTo(methodArgumentType)) {
      Log::e("Call argument types do not match for a call to method '" + method->getName() +
             "' of the model type '" + callableObject->getName() + "'");
      exit(1);
    }
    
    callArgumentsIterator++;
  }
}

string MethodCall::translateModelMethodToLLVMFunctionName(Model* model, string methodName) {
  if (model == NULL) {
    return methodName;
  }
  return "model." + model->getName() + "." + methodName;
}

string MethodCall::translateInterfaceMethodToLLVMFunctionName(Model* model,
                                                                   const Interface* interface,
                                                                   string methodName) {
  if (model == NULL) {
    return methodName;
  }
  return "model." + model->getName() + ".interface." + interface->getName() + "." + methodName;
}
