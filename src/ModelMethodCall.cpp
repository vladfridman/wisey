//
//  ModelMethodCall.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Log.hpp"
#include "yazyk/ModelMethodCall.hpp"

using namespace std;
using namespace llvm;
using namespace yazyk;

Value* ModelMethodCall::generateIR(IRGenerationContext& context) const {
  Model* model = (Model*) getModel(context);
  Method* method = getMethod(context);
  checkArgumentType(model, method, context);

  string llvmFunctionName = translateMethodToLLVMFunctionName(model, mMethodName);
  
  Function *function = context.getModule()->getFunction(llvmFunctionName.c_str());
  if (function == NULL) {
    Log::e("LLVM function implementing model '" + model->getName() + "' method '" +
           mMethodName + "' was not found");
    exit(1);
  }
  vector<Value*> arguments;
  arguments.push_back(mExpression.generateIR(context));
  ExpressionList::const_iterator it;
  for (it = mArguments.begin(); it != mArguments.end(); it++) {
    arguments.push_back((**it).generateIR(context));
  }
  string resultName = function->getReturnType()->isVoidTy() ? "" : "call";
  
  return CallInst::Create(function, arguments, resultName, context.getBasicBlock());
}

IType* ModelMethodCall::getType(IRGenerationContext& context) const {
  return getMethod(context)->getReturnType();
}

Model* ModelMethodCall::getModel(IRGenerationContext& context) const {
  IType* expressionType = mExpression.getType(context);
  if (expressionType->getTypeKind() == PRIMITIVE_TYPE) {
    Log::e("Attempt to call a method '" + mMethodName + "' on a primitive type expression");
    exit(1);
  }
  
  return (Model*) expressionType;
}

Method* ModelMethodCall::getMethod(IRGenerationContext& context) const {
  Model* model = (Model*) getModel(context);
  Method* method = model->findMethod(mMethodName);
  if (method == NULL) {
    Log::e("Method '" + mMethodName + "' is not found in model '" + model->getName() + "'");
    exit(1);
  }
  
  return method;
}

void ModelMethodCall::checkArgumentType(Model* model,
                                        Method* method,
                                        IRGenerationContext& context) const {
  vector<MethodArgument*> methodArguments = method->getArguments();
  vector<IExpression*>::const_iterator callArgumentsIterator = mArguments.begin();
  
  if (mArguments.size() != method->getArguments().size()) {
    Log::e("Number of arguments for method call '" + method->getName() +
           "' of the model type '" + model->getName() + "' is not correct");
    exit(1);
  }
  
  for (vector<MethodArgument*>::iterator iterator = methodArguments.begin();
       iterator != methodArguments.end();
       iterator++) {
    MethodArgument* methodArgument = *iterator;
    IType* methodArgumentType = methodArgument->getType();
    IType* callArgumentType = (*callArgumentsIterator)->getType(context);
    
    if (methodArgumentType != callArgumentType) {
      Log::e("Call argument types do not match for a call to method '" + method->getName() +
             "' of the model type '" + model->getName() + "'");
      exit(1);
    }
    
    callArgumentsIterator++;
  }
}

string ModelMethodCall::translateMethodToLLVMFunctionName(Model* model, string methodName) {
  if (model == NULL) {
    return methodName;
  }
  return "model." + model->getName() + "." + methodName;
}
