//
//  MethodCall.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/HeapOwnerVariable.hpp"
#include "wisey/HeapReferenceVariable.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/Names.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

MethodCall::MethodCall(IExpression* expression,
                       std::string methodName,
                       ExpressionList arguments,
                       int line) :
mExpression(expression),
mMethodName(methodName),
mArguments(arguments),
mLine(line) { }

MethodCall::~MethodCall() {
  for (IExpression* expression : mArguments) {
    delete expression;
  }
  mArguments.clear();
  if (mExpression) {
    delete mExpression;
  }
}

IVariable* MethodCall::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value* MethodCall::generateIR(IRGenerationContext& context) const {
  const IObjectType* objectWithMethodsType = getObjectWithMethods(context);
  IMethodDescriptor* methodDescriptor = getMethodDescriptor(context);
  if (!checkAccess(context, objectWithMethodsType, methodDescriptor)) {
    Log::e("Method '" + mMethodName + "()' of object '" + objectWithMethodsType->getName() +
           "' is private");
    exit(1);
  }
  checkArgumentType(objectWithMethodsType, methodDescriptor, context);
  std::vector<const Model*> thrownExceptions = methodDescriptor->getThrownExceptions();
  context.getScopes().getScope()->addExceptions(thrownExceptions);
  
  if (methodDescriptor->isStatic()) {
    return generateStaticMethodCallIR(context, objectWithMethodsType, methodDescriptor);
  }
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
                             const IObjectType* object,
                             IMethodDescriptor* methodDescriptor) const {
  
  if (methodDescriptor->getAccessLevel() == AccessLevel::PUBLIC_ACCESS) {
    return true;
  }
  IVariable* thisVariable = context.getThis();
  if (thisVariable == NULL) {
    return context.getScopes().getObjectType() == object;
  }
  
  return thisVariable != NULL && thisVariable->getType() == object;
}

Value* MethodCall::generateInterfaceMethodCallIR(IRGenerationContext& context,
                                                 const Interface* interface,
                                                 IMethodDescriptor* methodDescriptor) const {
  Value* expressionValueStore = generateExpressionIR(context);
  Value* expressionValueLoaded = IRWriter::newLoadInst(context, expressionValueStore, "");

  FunctionType* functionType =
    IMethod::getLLVMFunctionType(methodDescriptor, context, interface);
  Type* pointerToVTablePointer = functionType->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer =
  IRWriter::newBitCastInst(context, expressionValueLoaded, pointerToVTablePointer);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()),
                              interface->getMethodIndex(methodDescriptor) + VTABLE_METHODS_OFFSET);
  GetElementPtrInst* virtualFunction = IRWriter::createGetElementPtrInst(context, vTable, index);
  LoadInst* function = IRWriter::newLoadInst(context, virtualFunction, "");
  
  Composer::checkNullAndThrowNPE(context, expressionValueLoaded);
  
  vector<Value*> arguments;
  arguments.push_back(expressionValueStore);

  return createFunctionCall(context,
                            interface,
                            (Function*) function,
                            methodDescriptor,
                            arguments,
                            expressionValueStore);
}

Value* MethodCall::generateObjectMethodCallIR(IRGenerationContext& context,
                                              const IObjectType* object,
                                              IMethodDescriptor* methodDescriptor) const {
  Value* expressionValueStore = generateExpressionIR(context);
  Value* expressionValueLoaded = IRWriter::newLoadInst(context, expressionValueStore, "");
  
  Function* function = getMethodFunction(context, object);
  Composer::checkNullAndThrowNPE(context, expressionValueLoaded);
  
  vector<Value*> arguments;
  arguments.push_back(expressionValueStore);
  
  return createFunctionCall(context,
                            object,
                            function,
                            methodDescriptor,
                            arguments,
                            expressionValueStore);
}

Value* MethodCall::generateStaticMethodCallIR(IRGenerationContext& context,
                                              const IObjectType* object,
                                              IMethodDescriptor* methodDescriptor) const {
  Function* function = getMethodFunction(context, object);
  vector<Value*> arguments;
  
  return createFunctionCall(context, object, function, methodDescriptor, arguments, NULL);
}

Function* MethodCall::getMethodFunction(IRGenerationContext& context,
                                        const IObjectType* object) const {
  string llvmFunctionName = translateObjectMethodToLLVMFunctionName(object, mMethodName);
  
  Function *function = context.getModule()->getFunction(llvmFunctionName.c_str());
  if (function == NULL) {
    Log::e("LLVM function implementing object '" + object->getName() + "' method '" +
           mMethodName + "' was not found");
    exit(1);
  }

  return function;
}

Value* MethodCall::generateExpressionIR(IRGenerationContext& context) const {
  return mExpression != NULL
  ? mExpression->generateIR(context)
  : context.getThis()->generateIdentifierIR(context, "");
}

Value* MethodCall::createFunctionCall(IRGenerationContext& context,
                                      const IObjectType* object,
                                      Function* function,
                                      IMethodDescriptor* methodDescriptor,
                                      vector<Value*> arguments,
                                      Value* expressionValue) const {

  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  Value* threadObject = threadVariable->generateIdentifierIR(context, "threadLoaded");
  arguments.push_back(threadObject);
  
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
    if (IType::isOwnerType(methodArgument->getType())) {
      Value* loadedCasted = IRWriter::newLoadInst(context, callArgumentValueCasted, "");
      arguments.push_back(loadedCasted);
      callArgument->releaseOwnership(context);
    } else {
      arguments.push_back(callArgumentValueCasted);
    }
    methodArgumentIterator++;
  }
  
  pushCallStack(context, object, mMethodName, expressionValue, threadObject, mLine);
  
  Value* result;
  if (!methodDescriptor->getThrownExceptions().size()) {
    result = IRWriter::createCallInst(context, function, arguments, "");
  } else {
    result = IRWriter::createInvokeInst(context, function, arguments, "");
  }
  
  popCallStack(context, object, threadObject);
  
  const IType* returnType = methodDescriptor->getReturnType();
  if (returnType->getTypeKind() == PRIMITIVE_TYPE) {
    return result;
  }
  
  string variableName = IVariable::getTemporaryVariableName(this);

  Value* pointer = IRWriter::newAllocaInst(context, result->getType(), "returnedObjectPointer");
  IRWriter::newStoreInst(context, result, pointer);

  IVariable* tempVariable = IType::isOwnerType(returnType)
    ? (IVariable*) new HeapOwnerVariable(variableName, (IObjectOwnerType*) returnType, pointer)
    : (IVariable*) new HeapReferenceVariable(variableName, (IObjectType*) returnType, pointer);
  
  context.getScopes().setVariable(tempVariable);
  return IType::isOwnerType(returnType) ? pointer : result;
}

const IType* MethodCall::getType(IRGenerationContext& context) const {
  return getMethodDescriptor(context)->getReturnType();
}

void MethodCall::releaseOwnership(IRGenerationContext& context) const {
  if (getMethodDescriptor(context)->getReturnType()->getTypeKind() == PRIMITIVE_TYPE) {
    return;
  }
  
  string variableName = IVariable::getTemporaryVariableName(this);
  context.getScopes().clearVariable(context, variableName);
}

void MethodCall::addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const {
  string variableName = IVariable::getTemporaryVariableName(this);
  IVariable* variable = context.getScopes().getVariable(variableName);
  if (IType::isOwnerType(variable->getType())) {
    context.getScopes().addReferenceToOwnerVariable(variable, reference);
  }
}

const IObjectType* MethodCall::getObjectWithMethods(IRGenerationContext& context) const {
  if (mExpression == NULL && context.getThis()) {
    return (IObjectType*) context.getThis()->getType();
  }
  
  if (mExpression == NULL) {
    return context.getScopes().getObjectType();
  }
  
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
  const IObjectType* objectWithMethods = getObjectWithMethods(context);
  IMethodDescriptor* methodDescriptor = objectWithMethods->findMethod(mMethodName);
  if (methodDescriptor == NULL) {
    Log::e("Method '" + mMethodName + "' is not found in object '" +
           objectWithMethods->getName() + "'");
    exit(1);
  }
  
  return methodDescriptor;
}

void MethodCall::checkArgumentType(const IObjectType* objectWithMethods,
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

string MethodCall::translateInterfaceMethodToLLVMFunctionName(const IObjectType* object,
                                                              const Interface* interface,
                                                              string methodName) {
  if (object == NULL) {
    return methodName;
  }
  return object->getName() + ".interface." + interface->getName() + "." + methodName;
}

bool MethodCall::existsInOuterScope(IRGenerationContext& context) const {
  if (mExpression == NULL && context.getThis()) {
    return context.getThis()->existsInOuterScope();
  }
  if (mExpression == NULL) {
    return true;
  }
  return mExpression->existsInOuterScope(context);
}

void MethodCall::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  mExpression->printToStream(context, stream);
  stream << "." << mMethodName << "(";
  for (IExpression* expression : mArguments) {
    expression->printToStream(context, stream);
    if (expression != mArguments.at(mArguments.size() - 1)) {
      stream << ", ";
    }
  }
  stream << ")";
}

