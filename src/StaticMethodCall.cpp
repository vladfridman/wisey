//
//  StaticMethodCall.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "Argument.hpp"
#include "AutoCast.hpp"
#include "Composer.hpp"
#include "FakeExpression.hpp"
#include "GetTypeNameMethod.hpp"
#include "IRWriter.hpp"
#include "LLVMFunction.hpp"
#include "LocalArrayOwnerVariable.hpp"
#include "LocalOwnerVariable.hpp"
#include "LocalReferenceVariable.hpp"
#include "Log.hpp"
#include "StaticMethodCall.hpp"
#include "ThreadExpression.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

StaticMethodCall::StaticMethodCall(IObjectTypeSpecifier* objectTypeSpecifier,
                                   string methodName,
                                   ExpressionList arguments,
                                   bool canThrow,
                                   int line) :
mObjectTypeSpecifier(objectTypeSpecifier),
mMethodName(methodName),
mArguments(arguments),
mCanThrow(canThrow),
mLine(line) { }

StaticMethodCall::~StaticMethodCall() {
  for (const IExpression* expression : mArguments) {
    delete expression;
  }
  mArguments.clear();
  delete mObjectTypeSpecifier;
}

StaticMethodCall* StaticMethodCall::create(IObjectTypeSpecifier* objectTypeSpecifier,
                                           string methodName,
                                           ExpressionList arguments,
                                           int line) {
  return new StaticMethodCall(objectTypeSpecifier, methodName, arguments, true, line);
}

StaticMethodCall* StaticMethodCall::createCantThrow(IObjectTypeSpecifier* objectTypeSpecifier,
                                                    string methodName,
                                                    ExpressionList arguments,
                                                    int line) {
  return new StaticMethodCall(objectTypeSpecifier, methodName, arguments, false, line);
}

int StaticMethodCall::getLine() const {
  return mLine;
}

Value* StaticMethodCall::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  const IMethodDescriptor* methodDescriptor = getMethodDescriptor(context);
  const IObjectType* objectType = mObjectTypeSpecifier->getType(context);
  if (!checkAccess(context, methodDescriptor)) {
    context.reportError(mLine,
                        (methodDescriptor->isNative() ? "LLVM function '" : "Static method '") +
                        mMethodName + "' of object " + objectType->getTypeName() +
                        " is private");
    throw 1;
  }
  checkArgumentType(methodDescriptor, context);
  std::vector<const Model*> thrownExceptions = methodDescriptor->getThrownExceptions();
  context.getScopes().getScope()->addExceptions(thrownExceptions, mLine);
  
  return generateMethodCallIR(context, methodDescriptor, assignToType);
}

bool StaticMethodCall::checkAccess(IRGenerationContext& context,
                                   const IMethodDescriptor* methodDescriptor) const {
  
  if (methodDescriptor->isPublic()) {
    return true;
  }
  const IObjectType* currentObjectType = context.getObjectType();
  const IObjectType* methodObjectType = mObjectTypeSpecifier->getType(context);
  return IObjectType::getParentOrSelf(&context, currentObjectType, mLine) ==
  IObjectType::getParentOrSelf(&context, methodObjectType, mLine);
}

Value* StaticMethodCall::generateMethodCallIR(IRGenerationContext& context,
                                              const IMethodDescriptor* methodDescriptor,
                                              const IType* assignToType) const {
  const IObjectType* objectType = mObjectTypeSpecifier->getType(context);
  if (methodDescriptor == GetTypeNameMethod::GET_TYPE_NAME_METHOD) {
    return GetTypeNameMethod::GET_TYPE_NAME_METHOD->generateIR(context, objectType);
  }
  string llvmFunctionName = IMethodCall::translateObjectMethodToLLVMFunctionName(objectType,
                                                                                 mMethodName);
  Function *function = context.getModule()->getFunction(llvmFunctionName.c_str());
  if (function == NULL) {
    context.reportError(mLine, "LLVM function implementing object " + objectType->getTypeName() +
                        " method '" + mMethodName + "' was not found");
    throw 1;
  }
  
  vector<Value*> arguments;
  
  if (!methodDescriptor->isNative()) {
    IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
    Value* threadObject = threadVariable->generateIdentifierIR(context, mLine);
    IVariable* callStackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
    Value* callStackObject = callStackVariable->generateIdentifierIR(context, mLine);
    arguments.push_back(threadObject);
    arguments.push_back(callStackObject);
  }
  
  vector<const Argument*> methodArguments = methodDescriptor->getArguments();
  vector<const Argument*>::iterator methodArgumentIterator = methodArguments.begin();
  for (const IExpression* callArgument : mArguments) {
    const Argument* methodArgument = *methodArgumentIterator;
    Value* callArgumentValue = callArgument->generateIR(context, methodArgument->getType());
    const IType* callArgumentType = callArgument->getType(context);
    const IType* methodArgumentType = methodArgument->getType();
    Value* callArgumentValueCasted = AutoCast::maybeCast(context,
                                                         callArgumentType,
                                                         callArgumentValue,
                                                         methodArgumentType,
                                                         mLine);
    arguments.push_back(callArgumentValueCasted);
    methodArgumentIterator++;
  }
  string resultName = function->getReturnType()->isVoidTy() ? "" : "call";
  
  Composer::setLineNumber(context, mLine);

  Value* result = mCanThrow
  ? (Value*) IRWriter::createInvokeInst(context, function, arguments, resultName, mLine)
  : (Value*) IRWriter::createCallInst(context, function, arguments, resultName);
  
  const IType* returnType = methodDescriptor->getReturnType();
  if (!returnType->isOwner() || assignToType->isOwner()) {
    return result;
  }
  
  string variableName = IVariable::getTemporaryVariableName(this);
  returnType->createLocalVariable(context, variableName, mLine);
  vector<const IExpression*> arrayIndicies;
  context.getScopes().getVariable(variableName)->
  generateAssignmentIR(context, new FakeExpression(result, returnType), arrayIndicies, mLine);

  return result;
}

const IType* StaticMethodCall::getType(IRGenerationContext& context) const {
  return getMethodDescriptor(context)->getReturnType();
}

const IMethodDescriptor* StaticMethodCall::getMethodDescriptor(IRGenerationContext& context) const {
  const IObjectType* objectType = mObjectTypeSpecifier->getType(context);
  if (!mMethodName.compare(GetTypeNameMethod::GET_TYPE_NAME_METHOD_NAME)) {
    return GetTypeNameMethod::GET_TYPE_NAME_METHOD;
  }
  const IMethodDescriptor* staticMethod = objectType->findMethod(mMethodName);
  IMethodDescriptor* llvmFunction = objectType->findLLVMFunction(mMethodName);
  if (staticMethod == NULL && llvmFunction == NULL) {
    context.reportError(mLine, "Static method '" + mMethodName + "' is not found in object " +
                        objectType->getTypeName());
    throw 1;
  }
  const IMethodDescriptor* methodDescriptor = staticMethod == NULL ? llvmFunction : staticMethod;
  if (!methodDescriptor->isStatic()) {
    context.reportError(mLine, "Method '" + mMethodName + "' of object type " +
                        objectType->getTypeName() + " is not static");
    throw 1;
  }
  
  return methodDescriptor;
}

void StaticMethodCall::checkArgumentType(const IMethodDescriptor* methodDescriptor,
                                         IRGenerationContext& context) const {
  vector<const Argument*> methodArguments = methodDescriptor->getArguments();
  ExpressionList::const_iterator callArgumentsIterator = mArguments.begin();
  
  if (mArguments.size() != methodDescriptor->getArguments().size()) {
    string methodOrFunction = methodDescriptor->isNative() ? "LLVM function " : "static method ";
    context.reportError(mLine,
                        "Number of arguments for " + methodOrFunction +
                        "call '" + methodDescriptor->getName() +
                        "' of the object type " +
                        mObjectTypeSpecifier->getType(context)->getTypeName() + " is not correct");
    throw 1;
  }
  
  for (const Argument* methodArgument : methodArguments) {
    const IType* methodArgumentType = methodArgument->getType();
    const IType* callArgumentType = (*callArgumentsIterator)->getType(context);
    
    if (!callArgumentType->canAutoCastTo(context, methodArgumentType)) {
      string methodOrFunction = methodDescriptor->isNative() ? "LLVM function '" : "method '";
      context.reportError(mLine,
                          "Call argument types do not match for a call to " + methodOrFunction +
                          methodDescriptor->getName() + "' of the object type " +
                          mObjectTypeSpecifier->getType(context)->getTypeName());
      throw 1;
    }
    
    callArgumentsIterator++;
  }
}

bool StaticMethodCall::isConstant() const {
  return false;
}

bool StaticMethodCall::isAssignable() const {
  return false;
}

void StaticMethodCall::printToStream(IRGenerationContext& context,
                                     std::iostream& stream) const {
  mObjectTypeSpecifier->printToStream(context, stream);
  stream << "." << mMethodName << "(";
  for (const IExpression* expression : mArguments) {
    expression->printToStream(context, stream);
    if (expression != mArguments.at(mArguments.size() - 1)) {
      stream << ", ";
    }
  }
  stream << ")";
}

