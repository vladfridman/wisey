//
//  StaticMethodCall.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Argument.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LocalArrayOwnerVariable.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/StaticMethodCall.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

StaticMethodCall::StaticMethodCall(IObjectTypeSpecifier* objectTypeSpecifier,
                                   string methodName,
                                   ExpressionList arguments,
                                   int line) :
mObjectTypeSpecifier(objectTypeSpecifier),
mMethodName(methodName),
mArguments(arguments),
mLine(line) { }

StaticMethodCall::~StaticMethodCall() {
  for (const IExpression* expression : mArguments) {
    delete expression;
  }
  mArguments.clear();
  delete mObjectTypeSpecifier;
}

int StaticMethodCall::getLine() const {
  return mLine;
}

Value* StaticMethodCall::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  IMethodDescriptor* methodDescriptor = getMethodDescriptor(context);
  const IObjectType* objectType = mObjectTypeSpecifier->getType(context);
  if (!checkAccess(context, methodDescriptor)) {
    Log::e_deprecated((methodDescriptor->isNative() ? "LLVM function '" : "Static method '") +
                      mMethodName + "' of object " + objectType->getTypeName() +
                      " is private");
    exit(1);
  }
  checkArgumentType(methodDescriptor, context);
  std::vector<const Model*> thrownExceptions = methodDescriptor->getThrownExceptions();
  context.getScopes().getScope()->addExceptions(thrownExceptions);
  
  return generateMethodCallIR(context, methodDescriptor, assignToType);
}

bool StaticMethodCall::checkAccess(IRGenerationContext& context,
                                   IMethodDescriptor* methodDescriptor) const {
  
  if (methodDescriptor->isPublic()) {
    return true;
  }
  const IObjectType* currentObjectType = context.getObjectType();
  
  return currentObjectType != NULL && currentObjectType == mObjectTypeSpecifier->getType(context);
}

Value* StaticMethodCall::generateMethodCallIR(IRGenerationContext& context,
                                              IMethodDescriptor* methodDescriptor,
                                              const IType* assignToType) const {
  const IObjectType* objectType = mObjectTypeSpecifier->getType(context);
  string llvmFunctionName = objectType->getTypeName() + "." + mMethodName;
  
  Function *function = context.getModule()->getFunction(llvmFunctionName.c_str());
  if (function == NULL) {
    Log::e_deprecated("LLVM function implementing object " + objectType->getTypeName() +
                      " method '" + mMethodName + "' was not found");
    exit(1);
  }
  
  vector<Value*> arguments;
  
  if (!methodDescriptor->isNative()) {
    IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
    Value* threadObject = threadVariable->generateIdentifierIR(context);
    IVariable* callStackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
    Value* callStackObject = callStackVariable->generateIdentifierIR(context);
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
  
  Composer::pushCallStack(context, mLine);
  
  Value* result = IRWriter::createInvokeInst(context, function, arguments, resultName, mLine);
  
  Composer::popCallStack(context);
  
  const IType* returnType = methodDescriptor->getReturnType();
  if (!returnType->isOwner() || assignToType->isOwner()) {
    return result;
  }
  
  string variableName = IVariable::getTemporaryVariableName(this);
  Value* pointer = IRWriter::newAllocaInst(context, result->getType(), "returnedObjectPointer");
  IRWriter::newStoreInst(context, result, pointer);
  
  IVariable* tempVariable = NULL;
  // TODO: Add createLocalOwnerVarible() method to IType
  if (returnType->isArray()) {
    tempVariable = new LocalArrayOwnerVariable(variableName,
                                               (const ArrayOwnerType*) returnType,
                                               pointer);
  } else {
    tempVariable = new LocalOwnerVariable(variableName,
                                          (const IOwnerType*) returnType,
                                          pointer);
  }
  context.getScopes().setVariable(tempVariable);
  
  return result;
}

const IType* StaticMethodCall::getType(IRGenerationContext& context) const {
  return getMethodDescriptor(context)->getReturnType();
}

IMethodDescriptor* StaticMethodCall::getMethodDescriptor(IRGenerationContext& context) const {
  const IObjectType* objectType = mObjectTypeSpecifier->getType(context);
  IMethodDescriptor* staticMethod = objectType->findMethod(mMethodName);
  IMethodDescriptor* llvmFunction = objectType->findLLVMFunction(mMethodName);
  if (staticMethod == NULL && llvmFunction == NULL) {
    Log::e_deprecated("Static method '" + mMethodName + "' is not found in object " +
                      objectType->getTypeName());
    exit(1);
  }
  IMethodDescriptor* methodDescriptor = staticMethod == NULL ? llvmFunction : staticMethod;
  if (!methodDescriptor->isStatic()) {
    Log::e_deprecated("Method '" + mMethodName + "' of object type " +
                      objectType->getTypeName() + " is not static");
    exit(1);
  }
  
  return methodDescriptor;
}

void StaticMethodCall::checkArgumentType(IMethodDescriptor* methodDescriptor,
                                         IRGenerationContext& context) const {
  vector<const Argument*> methodArguments = methodDescriptor->getArguments();
  ExpressionList::const_iterator callArgumentsIterator = mArguments.begin();
  
  if (mArguments.size() != methodDescriptor->getArguments().size()) {
    string methodOrFunction = methodDescriptor->isNative() ? "LLVM function " : "static method ";
    Log::e_deprecated("Number of arguments for " + methodOrFunction +
                      "call '" + methodDescriptor->getName() +
                      "' of the object type " +
                      mObjectTypeSpecifier->getType(context)->getTypeName() + " is not correct");
    exit(1);
  }
  
  for (const Argument* methodArgument : methodArguments) {
    const IType* methodArgumentType = methodArgument->getType();
    const IType* callArgumentType = (*callArgumentsIterator)->getType(context);
    
    if (!callArgumentType->canAutoCastTo(context, methodArgumentType)) {
      string methodOrFunction = methodDescriptor->isNative() ? "LLVM function '" : "method '";
      Log::e_deprecated("Call argument types do not match for a call to " + methodOrFunction +
                        methodDescriptor->getName() + "' of the object type " +
                        mObjectTypeSpecifier->getType(context)->getTypeName());
      exit(1);
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

