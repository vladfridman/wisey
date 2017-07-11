//
//  MethodCall.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AutoCast.hpp"
#include "wisey/HeapVariable.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodCall.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

MethodCall::~MethodCall() {
  for (IExpression* expression : mArguments) {
    delete expression;
  }
  mArguments.clear();
  delete mExpression;
}

Value* MethodCall::generateIR(IRGenerationContext& context) const {
  IObjectType* objectWithMethodsType = getObjectWithMethods(context);
  IMethodDescriptor* methodDescriptor = getMethodDescriptor(context);
  if (!checkAccess(context, objectWithMethodsType, methodDescriptor)) {
    Log::e("Method '" + mMethodName + "()' of object '" + objectWithMethodsType->getName() +
           "' is private");
    exit(1);
  }
  checkArgumentType(objectWithMethodsType, methodDescriptor, context);
  context.getScopes().getScope()->addExceptions(methodDescriptor->getThrownExceptions());
  if (IType::isConcreteObjectType(objectWithMethodsType)) {
    return generateObjectMethodCallIR(context,
                                      (IObjectType*) objectWithMethodsType,
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
                             IObjectType* object,
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
  Value* expressionValue = mExpression->generateIR(context);

  FunctionType* functionType =
    IMethodDescriptor::getLLVMFunctionType(methodDescriptor, context, interface);
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

Value* MethodCall::generateObjectMethodCallIR(IRGenerationContext& context,
                                              IObjectType* object,
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
                                      Type* returnLLVMType,
                                      IMethodDescriptor* methodDescriptor) const {
  vector<Value*> arguments;
  arguments.push_back(mExpression->generateIR(context));
  vector<MethodArgument*> methodArguments = methodDescriptor->getArguments();
  vector<MethodArgument*>::iterator methodArgumentIterator = methodArguments.begin();
  for (IExpression* callArgument : mArguments) {
    Value* callArgumentValue = callArgument->generateIR(context);
    const IType* callArgumentType = callArgument->getType(context);
    MethodArgument* methodArgument = *methodArgumentIterator;
    const IType* methodArgumentType = methodArgument->getType();
    Value* callArgumentValueCasted = AutoCast::maybeCast(context,
                                                         callArgumentType,
                                                         callArgumentValue,
                                                         methodArgumentType);
    arguments.push_back(callArgumentValueCasted);
    if (IType::isOwnerType(methodArgument->getType())) {
      callArgument->releaseOwnership(context);
    }
    methodArgumentIterator++;
  }
  string resultName = returnLLVMType->isVoidTy() ? "" : "call";
  
  Value* result;
  if (!methodDescriptor->getThrownExceptions().size()) {
    result = IRWriter::createCallInst(context, function, arguments, resultName);
  } else {
    result = IRWriter::createInvokeInst(context, function, arguments, resultName);
  }
  
  const IType* returnType = methodDescriptor->getReturnType();
  if (returnType->getTypeKind() == PRIMITIVE_TYPE) {
    return result;
  }
  
  string variableName = IVariable::getTemporaryVariableName(this);

  if (!IType::isOwnerType(returnType)) {
    HeapVariable* tempVariable = new HeapVariable(variableName, returnType, result);
    context.getScopes().setVariable(tempVariable);
    return result;
  }
  
  Value* pointer = IRWriter::newAllocaInst(context, result->getType(), "returnedObjectPointer");
  IRWriter::newStoreInst(context, result, pointer);
  HeapVariable* tempVariable = new HeapVariable(variableName, returnType, pointer);
  context.getScopes().setVariable(tempVariable);
  return result;
}

const IType* MethodCall::getType(IRGenerationContext& context) const {
  return getMethodDescriptor(context)->getReturnType();
}

void MethodCall::releaseOwnership(IRGenerationContext& context) const {
  if (getMethodDescriptor(context)->getReturnType()->getTypeKind() == PRIMITIVE_TYPE) {
    return;
  }
  
  string variableName = IVariable::getTemporaryVariableName(this);
  context.getScopes().clearVariable(IVariable::getTemporaryVariableName(this));
}

IObjectType* MethodCall::getObjectWithMethods(IRGenerationContext& context) const {
  const IType* expressionType = mExpression->getType(context);
  if (expressionType->getTypeKind() == PRIMITIVE_TYPE) {
    Log::e("Attempt to call a method '" + mMethodName + "' on a primitive type expression");
    exit(1);
  }
  
  if (IType::isOwnerType(expressionType)) {
    return ((IObjectOwnerType*) expressionType)->getObject();
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
    const IType* methodArgumentType = methodArgument->getType();
    const IType* callArgumentType = (*callArgumentsIterator)->getType(context);
    
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

bool MethodCall::existsInOuterScope(IRGenerationContext& context) const {
  return mExpression->existsInOuterScope(context);
}
