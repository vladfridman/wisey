//
//  MethodCall.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AutoCast.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodCall.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

Value* MethodCall::generateIR(IRGenerationContext& context) const {
  IObjectType* object = getObjectWithMethods(context);
  IMethodDescriptor* methodDescriptor = getMethodDescriptor(context);
  if (!checkAccess(context, object, methodDescriptor)) {
    Log::e("Method '" + mMethodName + "()' of object '" + object->getName() + "' is private");
    exit(1);
  }
  checkArgumentType(object, methodDescriptor, context);
  context.getScopes().getScope()->addExceptions(methodDescriptor->getThrownExceptions());

  Value* expressionValue = mExpression.generateIR(context);
  FunctionType* functionType =
  IMethodDescriptor::getLLVMFunctionType(methodDescriptor, context, object);
  Type* pointerToVTablePointer = functionType->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer =
  IRWriter::newBitCastInst(context, expressionValue, pointerToVTablePointer);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()),
                              methodDescriptor->getIndex() + VTABLE_METHODS_OFFSET);
  GetElementPtrInst* virtualFunction = IRWriter::createGetElementPtrInst(context, vTable, index);
  LoadInst* function = IRWriter::newLoadInst(context, virtualFunction, "");
  
  return createFunctionCall(context,
                            (Function*) function,
                            functionType->getReturnType(),
                            methodDescriptor);
}

bool MethodCall::checkAccess(IRGenerationContext& context,
                             IObjectType* object,
                             IMethodDescriptor* methodDescriptor) const {
  
  if (methodDescriptor->getAccessLevel() == AccessLevel::PUBLIC_ACCESS) {
    return true;
  }
  IVariable* thisVariable = context.getScopes().getVariable("this");
  
  return thisVariable != NULL && thisVariable->getType() == object;
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
  return IRWriter::createInvokeInst(context, function, arguments, resultName);
}

IType* MethodCall::getType(IRGenerationContext& context) const {
  return getMethodDescriptor(context)->getReturnType();
}

void MethodCall::releaseOwnership(IRGenerationContext& context) const {
  // TODO implement this if needed
}

IObjectType* MethodCall::getObjectWithMethods(IRGenerationContext& context) const {
  IType* expressionType = mExpression.getType(context);
  if (expressionType->getTypeKind() == PRIMITIVE_TYPE) {
    Log::e("Attempt to call a method '" + mMethodName + "' on a primitive type expression");
    exit(1);
  }
  
  return (IObjectType*) expressionType;
}

IMethodDescriptor* MethodCall::getMethodDescriptor(IRGenerationContext& context) const {
  IObjectType* objectWithMethods = getObjectWithMethods(context);
  IMethodDescriptor* methodDescriptor = objectWithMethods->findMethod(mMethodName);
  if (methodDescriptor == NULL) {
    Log::e("Method '" + mMethodName + "' is not found in object '" +
           objectWithMethods->getName() + "'");
    exit(1);
  }
  
  return methodDescriptor;
}

void MethodCall::checkArgumentType(IObjectType* objectWithMethods,
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

string MethodCall::translateObjectMethodToLLVMFunctionName(IObjectType* object,
                                                           string methodName) {
  if (object == NULL) {
    return methodName;
  }
  return object->getName() + "." + methodName;
}

string MethodCall::translateInterfaceMethodToLLVMFunctionName(IObjectType* object,
                                                              const Interface* interface,
                                                              string methodName) {
  if (object == NULL) {
    return methodName;
  }
  return object->getName() + ".interface." + interface->getName() + "." + methodName;
}
