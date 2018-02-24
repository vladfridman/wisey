//
//  MethodCall.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AutoCast.hpp"
#include "wisey/CheckForNullAndThrowFunction.hpp"
#include "wisey/Composer.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalArrayOwnerVariable.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodArgument.hpp"
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

IVariable* MethodCall::getVariable(IRGenerationContext& context,
                                   vector<const IExpression*>& arrayIndices) const {
  return NULL;
}

Value* MethodCall::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  IMethodDescriptor* methodDescriptor = getMethodDescriptor(context);
  const IObjectType* objectWithMethodsType = methodDescriptor->getObjectType();
  checkArgumentType(objectWithMethodsType, methodDescriptor, context);
  std::vector<const Model*> thrownExceptions = methodDescriptor->getThrownExceptions();
  context.getScopes().getScope()->addExceptions(thrownExceptions);
  
  if (methodDescriptor->isStatic()) {
    return generateStaticMethodCallIR(context,
                                      objectWithMethodsType,
                                      methodDescriptor,
                                      assignToType);
  }
  
  string npeFullName = Names::getLangPackageName() + "." + Names::getNPEModelName();
  context.getScopes().getScope()->addException(context.getModel(npeFullName));
  
  if (IType::isConcreteObjectType(objectWithMethodsType)) {
    return generateObjectMethodCallIR(context,
                                      (IObjectType*) objectWithMethodsType,
                                      methodDescriptor,
                                      assignToType);
  }
  if (objectWithMethodsType->getTypeKind() == INTERFACE_TYPE) {
    return generateInterfaceMethodCallIR(context,
                                         (Interface*) objectWithMethodsType,
                                         methodDescriptor,
                                         assignToType);
  }
  Log::e("Method call " + methodDescriptor->getTypeName() + " on an unknown object type '" +
         objectWithMethodsType->getTypeName() + "'");
  exit(1);
}

Value* MethodCall::generateInterfaceMethodCallIR(IRGenerationContext& context,
                                                 const Interface* interface,
                                                 IMethodDescriptor* methodDescriptor,
                                                 const IType* assignToType) const {
  Value* objectValue = mExpression->generateIR(context, PrimitiveTypes::VOID_TYPE);

  Composer::pushCallStack(context, mLine);
  CheckForNullAndThrowFunction::call(context, objectValue);
  Composer::popCallStack(context);

  FunctionType* functionType =
    IMethod::getLLVMFunctionType(context, methodDescriptor, interface);
  Type* pointerToVTablePointer = functionType->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer =
  IRWriter::newBitCastInst(context, objectValue, pointerToVTablePointer);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()),
                              interface->getMethodIndex(methodDescriptor) + VTABLE_METHODS_OFFSET);
  GetElementPtrInst* virtualFunction = IRWriter::createGetElementPtrInst(context, vTable, index);
  Function* function = (Function*) IRWriter::newLoadInst(context, virtualFunction, "");
  
  vector<Value*> arguments;
  arguments.push_back(objectValue);

  return createFunctionCall(context,
                            interface,
                            function,
                            methodDescriptor,
                            arguments,
                            assignToType);
}

Value* MethodCall::generateObjectMethodCallIR(IRGenerationContext& context,
                                              const IObjectType* objectType,
                                              IMethodDescriptor* methodDescriptor,
                                              const IType* assignToType) const {
  Value* objectValue = mExpression->generateIR(context, PrimitiveTypes::VOID_TYPE);
  Function* function = getMethodFunction(context, methodDescriptor);

  Composer::pushCallStack(context, mLine);
  CheckForNullAndThrowFunction::call(context, objectValue);
  Composer::popCallStack(context);

  vector<Value*> arguments;
  arguments.push_back(objectValue);
  
  return createFunctionCall(context,
                            objectType,
                            function,
                            methodDescriptor,
                            arguments,
                            assignToType);
}

Value* MethodCall::generateStaticMethodCallIR(IRGenerationContext& context,
                                              const IObjectType* objectType,
                                              IMethodDescriptor* methodDescriptor,
                                              const IType* assignToType) const {
  Function* function = getMethodFunction(context, methodDescriptor);
  vector<Value*> arguments;
  
  return createFunctionCall(context,
                            objectType,
                            function,
                            methodDescriptor,
                            arguments,
                            assignToType);
}

Value* MethodCall::createFunctionCall(IRGenerationContext& context,
                                      const IObjectType* object,
                                      Function* function,
                                      IMethodDescriptor* methodDescriptor,
                                      vector<Value*> arguments,
                                      const IType* assignToType) const {

  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  Value* threadObject = threadVariable->generateIdentifierIR(context);

  arguments.push_back(threadObject);
  
  vector<MethodArgument*> methodArguments = methodDescriptor->getArguments();
  vector<MethodArgument*>::iterator methodArgumentIterator = methodArguments.begin();
  for (const IExpression* callArgument : mArguments) {
    MethodArgument* methodArgument = *methodArgumentIterator;
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
  
  Composer::pushCallStack(context, mLine);

  Value* result = IRWriter::createInvokeInst(context, function, arguments, "", mLine);
  
  Composer::popCallStack(context);

  const IType* returnType = methodDescriptor->getReturnType();
  if (!returnType->isOwner() || assignToType->isOwner()) {
    return result;
  }
  
  string variableName = IVariable::getTemporaryVariableName(this);
  Value* pointer = IRWriter::newAllocaInst(context, result->getType(), "returnedObjectPointer");
  IRWriter::newStoreInst(context, result, pointer);

  IVariable* tempVariable = returnType->isArray()
  ? new LocalArrayOwnerVariable(variableName, (const ArrayOwnerType*) returnType, pointer)
  : (IVariable*) new LocalOwnerVariable(variableName, (IObjectOwnerType*) returnType, pointer);
  context.getScopes().setVariable(tempVariable);

  return result;
}

const IType* MethodCall::getType(IRGenerationContext& context) const {
  return getMethodDescriptor(context)->getReturnType();
}

IMethodDescriptor* MethodCall::getMethodDescriptor(IRGenerationContext& context) const {
  const IType* expressionType = mExpression->getType(context);
  if (expressionType->getTypeKind() != FUNCTION_TYPE) {
    Log::e("Can not call a method on expression of type " + expressionType->getTypeName());
    exit(1);
  }
  return (IMethodDescriptor*) expressionType;
}

void MethodCall::checkArgumentType(const IObjectType* objectWithMethods,
                                   IMethodDescriptor* methodDescriptor,
                                   IRGenerationContext& context) const {
  vector<MethodArgument*> methodArguments = methodDescriptor->getArguments();
  ExpressionList::const_iterator callArgumentsIterator = mArguments.begin();
  
  if (mArguments.size() != methodDescriptor->getArguments().size()) {
    Log::e("Number of arguments for method call '" + methodDescriptor->getName() +
           "' of the object type '" + objectWithMethods->getTypeName() + "' is not correct");
    exit(1);
  }
  
  for (MethodArgument* methodArgument : methodArguments) {
    const IType* methodArgumentType = methodArgument->getType();
    const IType* callArgumentType = (*callArgumentsIterator)->getType(context);
    
    if (!callArgumentType->canAutoCastTo(context, methodArgumentType)) {
      Log::e("Call argument types do not match for a call to method '" +
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
                                        IMethodDescriptor* methodDescriptor) const {
  const IObjectType* objectType = methodDescriptor->getObjectType();
  string methodName = methodDescriptor->getName();
  string llvmFunctionName = IMethodCall::translateObjectMethodToLLVMFunctionName(objectType,
                                                                                 methodName);
  
  Function *function = context.getModule()->getFunction(llvmFunctionName.c_str());
  if (function == NULL) {
    Log::e("LLVM function implementing object '" + objectType->getTypeName() + "' method '" +
           methodName + "' was not found");
    exit(1);
  }
  
  return function;
}

