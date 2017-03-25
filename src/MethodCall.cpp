//
//  MethodCall.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/AutoCast.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/MethodArgument.hpp"
#include "yazyk/MethodCall.hpp"

using namespace std;
using namespace llvm;
using namespace yazyk;

Value* MethodCall::generateIR(IRGenerationContext& context) const {
  ICallableObjectType* callableObject = getCallableObject(context);
  IMethodDescriptor* methodDescriptor = getMethodDescriptor(context);
  checkArgumentType(callableObject, methodDescriptor, context);
  if (callableObject->getTypeKind() == MODEL_TYPE) {
    return generateModelMethodCallIR(context, (Model*) callableObject, methodDescriptor);
  }
  if (callableObject->getTypeKind() == INTERFACE_TYPE) {
    return generateInterfaceMethodCallIR(context, (Interface*) callableObject, methodDescriptor);
  }
  Log::e("Method '" + mMethodName + "()' call on an unknown object type '" +
         callableObject->getName() + "'");
  exit(1);
}

Value* MethodCall::generateInterfaceMethodCallIR(IRGenerationContext& context,
                                                 Interface* interface,
                                                 IMethodDescriptor* methodDescriptor) const {
  BasicBlock* basicBlock = context.getBasicBlock();
  Value* expressionValue = mExpression.generateIR(context);
  FunctionType* functionType =
    IMethodDescriptor::getLLVMFunctionType(methodDescriptor, context, interface);
  Type* pointerToVTablePointer = functionType->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer =
    new BitCastInst(expressionValue, pointerToVTablePointer, "", basicBlock);
  LoadInst* vTable = new LoadInst(vTablePointer, "vtable", basicBlock);
  Value *Idx[1];
  Idx[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()),
                            methodDescriptor->getIndex() + VTABLE_METHODS_OFFSET);
  GetElementPtrInst* virtualFunction = GetElementPtrInst::Create(functionType->getPointerTo(),
                                                                 vTable,
                                                                 Idx,
                                                                 "vfn",
                                                                 basicBlock);
  LoadInst* function = new LoadInst(virtualFunction, "", basicBlock);
  
  return createFunctionCall(context,
                            (Function*) function,
                            functionType->getReturnType(),
                            methodDescriptor);
}

Value* MethodCall::generateModelMethodCallIR(IRGenerationContext& context,
                                             Model* model,
                                             IMethodDescriptor* methodDescriptor) const {
  string llvmFunctionName = translateModelMethodToLLVMFunctionName(model, mMethodName);
  
  Function *function = context.getModule()->getFunction(llvmFunctionName.c_str());
  if (function == NULL) {
    Log::e("LLVM function implementing model '" + model->getName() + "' method '" +
           mMethodName + "' was not found");
    exit(1);
  }

  return createFunctionCall(context, function, function->getReturnType(), methodDescriptor);
}

CallInst* MethodCall::createFunctionCall(IRGenerationContext& context,
                                         Function* function,
                                         Type* returnType,
                                         IMethodDescriptor* methodDescriptor) const {
  vector<Value*> arguments;
  arguments.push_back(mExpression.generateIR(context));
  vector<MethodArgument*> methodArguments = methodDescriptor->getArguments();
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
  string resultName = returnType->isVoidTy() ? "" : "call";
  
  return CallInst::Create(function, arguments, resultName, context.getBasicBlock());
}

IType* MethodCall::getType(IRGenerationContext& context) const {
  return getMethodDescriptor(context)->getReturnType();
}

void MethodCall::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Releasing ownership of a method call result is not implemented");
  exit(1);
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

IMethodDescriptor* MethodCall::getMethodDescriptor(IRGenerationContext& context) const {
  ICallableObjectType* callableObject = getCallableObject(context);
  IMethodDescriptor* methodDescriptor = callableObject->findMethod(mMethodName);
  if (methodDescriptor == NULL) {
    Log::e("Method '" + mMethodName + "' is not found in callable object '" +
           callableObject->getName() + "'");
    exit(1);
  }
  
  return methodDescriptor;
}

void MethodCall::checkArgumentType(ICallableObjectType* callableObject,
                                   IMethodDescriptor* methodDescriptor,
                                   IRGenerationContext& context) const {
  vector<MethodArgument*> methodArguments = methodDescriptor->getArguments();
  vector<IExpression*>::const_iterator callArgumentsIterator = mArguments.begin();
  
  if (mArguments.size() != methodDescriptor->getArguments().size()) {
    Log::e("Number of arguments for method call '" + methodDescriptor->getName() +
           "' of the model type '" + callableObject->getName() + "' is not correct");
    exit(1);
  }
  
  for (MethodArgument* methodArgument : methodArguments) {
    IType* methodArgumentType = methodArgument->getType();
    IType* callArgumentType = (*callArgumentsIterator)->getType(context);
    
    if (!callArgumentType->canAutoCastTo(methodArgumentType)) {
      Log::e("Call argument types do not match for a call to method '" + methodDescriptor->getName() +
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
