//
//  MethodCall.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/AutoCast.hpp"
#include "yazyk/IRWriter.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/MethodArgument.hpp"
#include "yazyk/MethodCall.hpp"

using namespace std;
using namespace llvm;
using namespace yazyk;

Value* MethodCall::generateIR(IRGenerationContext& context) const {
  IObjectWithMethodsType* objectWithMethodsType = getObjectWithMethods(context);
  IMethodDescriptor* methodDescriptor = getMethodDescriptor(context);
  if (!checkAccess(context, objectWithMethodsType, methodDescriptor)) {
    Log::e("Method '" + mMethodName + "()' of object '" + objectWithMethodsType->getName() +
           "' is private");
    exit(1);
  }
  checkArgumentType(objectWithMethodsType, methodDescriptor, context);
  context.getScopes().getScope()->addExceptions(methodDescriptor->getThrownExceptions());
  if (objectWithMethodsType->getTypeKind() == MODEL_TYPE ||
      objectWithMethodsType->getTypeKind() == CONTROLLER_TYPE) {
    return generateModelMethodCallIR(context,
                                     (IObjectWithMethodsType*) objectWithMethodsType,
                                     methodDescriptor);
  }
  if (objectWithMethodsType->getTypeKind() == INTERFACE_TYPE) {
    return generateInterfaceMethodCallIR(context,
                                         (Interface*) objectWithMethodsType,
                                         methodDescriptor);
  }
  Log::e("Method '" + mMethodName + "()' call on an unknown object type '" +
         objectWithMethodsType->getName() + "'");
  exit(1);
}

bool MethodCall::checkAccess(IRGenerationContext& context,
                             IObjectWithMethodsType* object,
                             IMethodDescriptor* methodDescriptor) const {
  
  if (methodDescriptor->getAccessLevel() == AccessLevel::PUBLIC_ACCESS) {
    return true;
  }
  IVariable* thisVariable = context.getScopes().getVariable("this");
  
  return thisVariable != NULL && thisVariable->getType() == object;
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
  IRWriter::newBitCastInst(context, expressionValue, pointerToVTablePointer);
  LoadInst* vTable = new LoadInst(vTablePointer, "vtable", basicBlock);
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()),
                            methodDescriptor->getIndex() + VTABLE_METHODS_OFFSET);
  GetElementPtrInst* virtualFunction = IRWriter::createGetElementPtrInst(context, vTable, index);
  LoadInst* function = new LoadInst(virtualFunction, "", basicBlock);
  
  return createFunctionCall(context,
                            (Function*) function,
                            functionType->getReturnType(),
                            methodDescriptor);
}

Value* MethodCall::generateModelMethodCallIR(IRGenerationContext& context,
                                             IObjectWithMethodsType* object,
                                             IMethodDescriptor* methodDescriptor) const {
  string llvmFunctionName = translateObjectMethodToLLVMFunctionName(object, mMethodName);
  
  Function *function = context.getModule()->getFunction(llvmFunctionName.c_str());
  if (function == NULL) {
    Log::e("LLVM function implementing object '" + object->getName() + "' method '" +
           mMethodName + "' was not found");
    exit(1);
  }

  return createFunctionCall(context, function, function->getReturnType(), methodDescriptor);
}

Value* MethodCall::createFunctionCall(IRGenerationContext& context,
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
  
  if (!methodDescriptor->getThrownExceptions().size()) {
    return IRWriter::createCallInst(context, function, arguments, resultName);
  }
  BasicBlock* exceptionContinueBlock = context.getScopes().getExceptionContinueBlock();
  return InvokeInst::Create(function,
                            exceptionContinueBlock,
                            context.getScopes().getLandingPadBlock(),
                            arguments,
                            resultName,
                            context.getBasicBlock());
}

IType* MethodCall::getType(IRGenerationContext& context) const {
  return getMethodDescriptor(context)->getReturnType();
}

void MethodCall::releaseOwnership(IRGenerationContext& context) const {
  // TODO implement this if needed
}

IObjectWithMethodsType* MethodCall::getObjectWithMethods(IRGenerationContext& context) const {
  IType* expressionType = mExpression.getType(context);
  if (expressionType->getTypeKind() == PRIMITIVE_TYPE) {
    Log::e("Attempt to call a method '" + mMethodName + "' on a primitive type expression");
    exit(1);
  }
  
  return dynamic_cast<IObjectWithMethodsType*>(expressionType);
}

IMethodDescriptor* MethodCall::getMethodDescriptor(IRGenerationContext& context) const {
  IObjectWithMethodsType* objectWithMethods = getObjectWithMethods(context);
  IMethodDescriptor* methodDescriptor = objectWithMethods->findMethod(mMethodName);
  if (methodDescriptor == NULL) {
    Log::e("Method '" + mMethodName + "' is not found in object '" +
           objectWithMethods->getName() + "'");
    exit(1);
  }
  
  return methodDescriptor;
}

void MethodCall::checkArgumentType(IObjectWithMethodsType* objectWithMethods,
                                   IMethodDescriptor* methodDescriptor,
                                   IRGenerationContext& context) const {
  vector<MethodArgument*> methodArguments = methodDescriptor->getArguments();
  vector<IExpression*>::const_iterator callArgumentsIterator = mArguments.begin();
  
  if (mArguments.size() != methodDescriptor->getArguments().size()) {
    Log::e("Number of arguments for method call '" + methodDescriptor->getName() +
           "' of the object type '" + objectWithMethods->getName() + "' is not correct");
    exit(1);
  }
  
  for (MethodArgument* methodArgument : methodArguments) {
    IType* methodArgumentType = methodArgument->getType();
    IType* callArgumentType = (*callArgumentsIterator)->getType(context);
    
    if (!callArgumentType->canAutoCastTo(methodArgumentType)) {
      Log::e("Call argument types do not match for a call to method '" +
             methodDescriptor->getName() +
             "' of the object type '" + objectWithMethods->getName() + "'");
      exit(1);
    }
    
    callArgumentsIterator++;
  }
}

string MethodCall::translateObjectMethodToLLVMFunctionName(IObjectWithMethodsType* object,
                                                           string methodName) {
  if (object == NULL) {
    return methodName;
  }
  return "object." + object->getName() + "." + methodName;
}

string MethodCall::translateInterfaceMethodToLLVMFunctionName(IObjectWithMethodsType* object,
                                                              const Interface* interface,
                                                              string methodName) {
  if (object == NULL) {
    return methodName;
  }
  return "object." + object->getName() + ".interface." + interface->getName() + "." + methodName;
}
