//
//  MethodCall.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Argument.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/CheckForNullAndThrowFunction.hpp"
#include "wisey/Composer.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalArrayOwnerVariable.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

MethodCall::MethodCall(IExpression* expression,
                       ExpressionList arguments,
                       int line) :
mExpression(expression),
mArguments(arguments),
mLine(line) { }

MethodCall::~MethodCall() {
  for (const IExpression* expression : mArguments) {
    delete expression;
  }
  mArguments.clear();
  delete mExpression;
}

int MethodCall::getLine() const {
  return mLine;
}

Value* MethodCall::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  const IMethodDescriptor* methodDescriptor = getMethodDescriptor(context);
  const IObjectType* object = methodDescriptor->getParentObject();
  checkArgumentType(object, methodDescriptor, context);
  std::vector<const Model*> thrownExceptions = methodDescriptor->getThrownExceptions();
  context.getScopes().getScope()->addExceptions(thrownExceptions, mLine);
  
  if (methodDescriptor->isNative()) {
    return generateLLVMFunctionCallIR(context, object, methodDescriptor, assignToType);
  }
  
  if (methodDescriptor->isStatic()) {
    return generateStaticMethodCallIR(context, object, methodDescriptor, assignToType);
  }
  
  string npeFullName = Names::getLangPackageName() + "." + Names::getNPEModelName();
  context.getScopes().getScope()->addException(context.getModel(npeFullName, mLine), mLine);
  
  if (IType::isConcreteObjectType(object)) {
    return generateObjectMethodCallIR(context,
                                      (const IObjectType*) object,
                                      methodDescriptor,
                                      assignToType);
  }
  if (object->isInterface()) {
    return generateInterfaceMethodCallIR(context,
                                         (const Interface*) object,
                                         methodDescriptor,
                                         assignToType);
  }
  Log::e_deprecated("Method call " + methodDescriptor->getTypeName() +
                    " on an unknown object type '" + object->getTypeName() + "'");
  exit(1);
}

Value* MethodCall::generateInterfaceMethodCallIR(IRGenerationContext& context,
                                                 const Interface* interface,
                                                 const IMethodDescriptor* methodDescriptor,
                                                 const IType* assignToType) const {
  Value* objectValue = mExpression->generateIR(context, PrimitiveTypes::VOID_TYPE);

  Composer::setLineNumber(context, mLine);
  CheckForNullAndThrowFunction::call(context, objectValue);

  FunctionType* functionType =
    IMethod::getLLVMFunctionType(context, methodDescriptor, interface, mLine);
  Type* pointerToVTablePointer = functionType->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer =
  IRWriter::newBitCastInst(context, objectValue, pointerToVTablePointer);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()),
                              interface->getMethodIndex(methodDescriptor) + VTABLE_METHODS_OFFSET);
  GetElementPtrInst* virtualFunction = IRWriter::createGetElementPtrInst(context, vTable, index);
  Function* function = (Function*) IRWriter::newLoadInst(context, virtualFunction, "");
  
  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  Value* threadObject = threadVariable->generateIdentifierIR(context);
  IVariable* callStackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  Value* callStackObject = callStackVariable->generateIdentifierIR(context);
  
  vector<Value*> arguments;
  arguments.push_back(objectValue);
  arguments.push_back(threadObject);
  arguments.push_back(callStackObject);

  return createFunctionCall(context,
                            interface,
                            function,
                            methodDescriptor,
                            arguments,
                            assignToType);
}

Value* MethodCall::generateObjectMethodCallIR(IRGenerationContext& context,
                                              const IObjectType* objectType,
                                              const IMethodDescriptor* methodDescriptor,
                                              const IType* assignToType) const {
  Value* objectValue = mExpression->generateIR(context, PrimitiveTypes::VOID_TYPE);
  Function* function = getMethodFunction(context, methodDescriptor);

  Composer::setLineNumber(context, mLine);
  CheckForNullAndThrowFunction::call(context, objectValue);

  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  Value* threadObject = threadVariable->generateIdentifierIR(context);
  IVariable* callStackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  Value* callStackObject = callStackVariable->generateIdentifierIR(context);

  vector<Value*> arguments;
  arguments.push_back(objectValue);
  arguments.push_back(threadObject);
  arguments.push_back(callStackObject);

  return createFunctionCall(context,
                            objectType,
                            function,
                            methodDescriptor,
                            arguments,
                            assignToType);
}

Value* MethodCall::generateLLVMFunctionCallIR(IRGenerationContext& context,
                                              const IObjectType* object,
                                              const IMethodDescriptor* methodDescriptor,
                                              const IType* assignToType) const {
  Function* function = getMethodFunction(context, methodDescriptor);
  vector<Value*> arguments;
  
  return createFunctionCall(context, object, function, methodDescriptor, arguments, assignToType);
}

Value* MethodCall::generateStaticMethodCallIR(IRGenerationContext& context,
                                              const IObjectType* object,
                                              const IMethodDescriptor* methodDescriptor,
                                              const IType* assignToType) const {
  Function* function = getMethodFunction(context, methodDescriptor);

  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  Value* threadObject = threadVariable->generateIdentifierIR(context);
  IVariable* callStackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  Value* callStackObject = callStackVariable->generateIdentifierIR(context);
  
  vector<Value*> arguments;
  arguments.push_back(threadObject);
  arguments.push_back(callStackObject);

  return createFunctionCall(context, object, function, methodDescriptor, arguments, assignToType);
}

Value* MethodCall::createFunctionCall(IRGenerationContext& context,
                                      const IObjectType* object,
                                      Function* function,
                                      const IMethodDescriptor* methodDescriptor,
                                      vector<Value*> arguments,
                                      const IType* assignToType) const {
  Composer::setLineNumber(context, mLine);

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
  
  Value* result = IRWriter::createInvokeInst(context, function, arguments, "", mLine);
  
  const IType* returnType = methodDescriptor->getReturnType();
  if (!returnType->isOwner() || assignToType->isOwner()) {
    return result;
  }
  
  Value* pointer = IRWriter::newAllocaInst(context, result->getType(), "returnedObjectPointer");
  IRWriter::newStoreInst(context, result, pointer);

  string variableName = IVariable::getTemporaryVariableName(this);
  returnType->createLocalVariable(context, variableName);

  return result;
}

const IType* MethodCall::getType(IRGenerationContext& context) const {
  return getMethodDescriptor(context)->getReturnType();
}

const IMethodDescriptor* MethodCall::getMethodDescriptor(IRGenerationContext& context) const {
  const IType* expressionType = mExpression->getType(context);
  if (!expressionType->isFunction()) {
    context.reportError(mLine,
                        "Can not call a method on expression of type " +
                        expressionType->getTypeName());
    exit(1);
  }
  return (const IMethodDescriptor*) expressionType;
}

void MethodCall::checkArgumentType(const IObjectType* objectWithMethods,
                                   const IMethodDescriptor* methodDescriptor,
                                   IRGenerationContext& context) const {
  vector<const Argument*> methodArguments = methodDescriptor->getArguments();
  ExpressionList::const_iterator callArgumentsIterator = mArguments.begin();
  
  if (mArguments.size() != methodDescriptor->getArguments().size()) {
    Log::e_deprecated("Number of arguments for method call '" + methodDescriptor->getName() +
           "' of the object type '" + objectWithMethods->getTypeName() + "' is not correct");
    exit(1);
  }
  
  for (const Argument* methodArgument : methodArguments) {
    const IType* methodArgumentType = methodArgument->getType();
    const IType* callArgumentType = (*callArgumentsIterator)->getType(context);
    
    if (!callArgumentType->canAutoCastTo(context, methodArgumentType)) {
      Log::e_deprecated("Call argument types do not match for a call to method '" +
             methodDescriptor->getName() +
             "' of the object type '" + objectWithMethods->getTypeName() + "'");
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
  return object->getTypeName() + ".interface." + interface->getTypeName() + "." + methodName;
}

bool MethodCall::isConstant() const {
  return false;
}

bool MethodCall::isAssignable() const {
  return false;
}

void MethodCall::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  mExpression->printToStream(context, stream);
  stream << "(";
  for (const IExpression* expression : mArguments) {
    expression->printToStream(context, stream);
    if (expression != mArguments.at(mArguments.size() - 1)) {
      stream << ", ";
    }
  }
  stream << ")";
}

Function* MethodCall::getMethodFunction(IRGenerationContext& context,
                                        const IMethodDescriptor* methodDescriptor) const {
  const IObjectType* objectType = methodDescriptor->getParentObject();
  string methodName = methodDescriptor->getName();
  string llvmFunctionName = IMethodCall::translateObjectMethodToLLVMFunctionName(objectType,
                                                                                 methodName);
  
  Function *function = context.getModule()->getFunction(llvmFunctionName.c_str());
  if (function == NULL) {
    Log::e_deprecated("LLVM function implementing object '" + objectType->getTypeName() + "' method '" +
           methodName + "' was not found");
    exit(1);
  }
  
  return function;
}

