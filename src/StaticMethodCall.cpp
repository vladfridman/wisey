//
//  StaticMethodCall.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodArgument.hpp"
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
  for (IExpression* expression : mArguments) {
    delete expression;
  }
  mArguments.clear();
  delete mObjectTypeSpecifier;
}

IVariable* StaticMethodCall::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value* StaticMethodCall::generateIR(IRGenerationContext& context) const {
  IMethodDescriptor* methodDescriptor = getMethodDescriptor(context);
  const IObjectType* objectType = mObjectTypeSpecifier->getType(context);
  if (!checkAccess(context, methodDescriptor)) {
    Log::e("Static method '" + mMethodName + "()' of object '" + objectType->getName() +
           "' is private");
    exit(1);
  }
  checkArgumentType(methodDescriptor, context);
  std::vector<const Model*> thrownExceptions = methodDescriptor->getThrownExceptions();
  context.getScopes().getScope()->addExceptions(thrownExceptions);
  
  return generateMethodCallIR(context, methodDescriptor);
}

bool StaticMethodCall::checkAccess(IRGenerationContext& context,
                                   IMethodDescriptor* methodDescriptor) const {
  
  if (methodDescriptor->getAccessLevel() == AccessLevel::PUBLIC_ACCESS) {
    return true;
  }
  const IObjectType* currentObjectType = context.getScopes().getObjectType();
  
  return currentObjectType != NULL && currentObjectType == mObjectTypeSpecifier->getType(context);
}

Value* StaticMethodCall::generateMethodCallIR(IRGenerationContext& context,
                                              IMethodDescriptor* methodDescriptor) const {
  const IObjectType* objectType = mObjectTypeSpecifier->getType(context);
  string llvmFunctionName = objectType->getName() + "." + mMethodName;
  
  Function *function = context.getModule()->getFunction(llvmFunctionName.c_str());
  if (function == NULL) {
    Log::e("LLVM function implementing object '" + objectType->getName() + "' method '" +
           mMethodName + "' was not found");
    exit(1);
  }
  
  vector<Value*> arguments;
  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  Value* threadObject = threadVariable->generateIdentifierIR(context);
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
      callArgument->releaseOwnership(context);
    }
    arguments.push_back(callArgumentValueCasted);
    methodArgumentIterator++;
  }
  string resultName = function->getReturnType()->isVoidTy() ? "" : "call";
  
  Composer::pushCallStack(context, mLine);

  Value* result = IRWriter::createInvokeInst(context, function, arguments, resultName);
  
  Composer::popCallStack(context);

  const IType* returnType = methodDescriptor->getReturnType();
  if (returnType->getTypeKind() == PRIMITIVE_TYPE) {
    return result;
  }
  
  string variableName = IVariable::getTemporaryVariableName(this);
  
  Value* pointer = IRWriter::newAllocaInst(context, result->getType(), "returnedObjectPointer");
  IRWriter::newStoreInst(context, result, pointer);

  IVariable* tempVariable = IType::isOwnerType(returnType)
    ? (IVariable*) new LocalOwnerVariable(variableName, (IObjectOwnerType*) returnType, pointer)
    : (IVariable*) new LocalReferenceVariable(variableName, (IObjectType*) returnType, pointer);
  
  if (IType::isReferenceType(returnType)) {
    ((IObjectType*) returnType)->incremenetReferenceCount(context, result);
  }

  context.getScopes().setVariable(tempVariable);
  return result;
}

const IType* StaticMethodCall::getType(IRGenerationContext& context) const {
  return getMethodDescriptor(context)->getReturnType();
}

void StaticMethodCall::releaseOwnership(IRGenerationContext& context) const {
  if (getMethodDescriptor(context)->getReturnType()->getTypeKind() == PRIMITIVE_TYPE) {
    return;
  }
  
  string variableName = IVariable::getTemporaryVariableName(this);
  context.getScopes().clearVariable(context, variableName);
}

void StaticMethodCall::addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const {
  string variableName = IVariable::getTemporaryVariableName(this);
  IVariable* variable = context.getScopes().getVariable(variableName);
  if (IType::isOwnerType(variable->getType())) {
    context.getScopes().addReferenceToOwnerVariable(variable, reference);
  }
}

IMethodDescriptor* StaticMethodCall::getMethodDescriptor(IRGenerationContext& context) const {
  const IObjectType* objectType = mObjectTypeSpecifier->getType(context);
  IMethodDescriptor* methodDescriptor = objectType->findMethod(mMethodName);
  if (methodDescriptor == NULL) {
    Log::e("Static method '" + mMethodName + "' is not found in object '" +
           objectType->getName() + "'");
    exit(1);
  }
  if (!methodDescriptor->isStatic()) {
    Log::e("Method '" + mMethodName + "' of object type '" +
           objectType->getName() + "' is not static");
    exit(1);
  }
  
  return methodDescriptor;
}

void StaticMethodCall::checkArgumentType(IMethodDescriptor* methodDescriptor,
                                         IRGenerationContext& context) const {
  vector<MethodArgument*> methodArguments = methodDescriptor->getArguments();
  vector<IExpression*>::const_iterator callArgumentsIterator = mArguments.begin();
  
  if (mArguments.size() != methodDescriptor->getArguments().size()) {
    Log::e("Number of arguments for static method call '" + methodDescriptor->getName() +
           "' of the object type '" + mObjectTypeSpecifier->getType(context)->getName() +
           "' is not correct");
    exit(1);
  }
  
  for (MethodArgument* methodArgument : methodArguments) {
    const IType* methodArgumentType = methodArgument->getType();
    const IType* callArgumentType = (*callArgumentsIterator)->getType(context);
    
    if (!callArgumentType->canAutoCastTo(methodArgumentType)) {
      Log::e("Call argument types do not match for a call to method '" +
             methodDescriptor->getName() +
             "' of the object type '" + mObjectTypeSpecifier->getType(context)->getName() + "'");
      exit(1);
    }
    
    callArgumentsIterator++;
  }
}

bool StaticMethodCall::existsInOuterScope(IRGenerationContext& context) const {
  return true;
}

bool StaticMethodCall::isConstant() const {
  return false;
}

void StaticMethodCall::printToStream(IRGenerationContext& context,
                                     std::iostream& stream) const {
  mObjectTypeSpecifier->printToStream(context, stream);
  stream << "." << mMethodName << "(";
  for (IExpression* expression : mArguments) {
    expression->printToStream(context, stream);
    if (expression != mArguments.at(mArguments.size() - 1)) {
      stream << ", ";
    }
  }
  stream << ")";
}
