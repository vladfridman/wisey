//
//  StaticMethod.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Argument.hpp"
#include "wisey/Cleanup.hpp"
#include "wisey/Composer.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/Model.hpp"
#include "wisey/Names.hpp"
#include "wisey/StaticMethod.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

StaticMethod::StaticMethod(const IObjectType* objectType,
                           string name,
                           AccessLevel accessLevel,
                           const IType* returnType,
                           vector<const Argument*> arguments,
                           vector<const Model*> thrownExceptions,
                           MethodQualifiers* methodQualifiers,
                           CompoundStatement* compoundStatement,
                           int line) :
mObjectType(objectType),
mName(name),
mIsPublic(accessLevel == PUBLIC_ACCESS),
mReturnType(returnType),
mArguments(arguments),
mThrownExceptions(thrownExceptions),
mMethodQualifiers(methodQualifiers),
mCompoundStatement(compoundStatement),
mLine(line) { }

StaticMethod::~StaticMethod() {
  for (const Argument* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
  // mCompoundStatement is deleted with MethodDefinition
}

bool StaticMethod::isStatic() const {
  return true;
}

string StaticMethod::getName() const {
  return mName;
}

bool StaticMethod::isPublic() const {
  return mIsPublic;
}

const IType* StaticMethod::getReturnType() const {
  return mReturnType;
}

vector<const wisey::Argument*> StaticMethod::getArguments() const {
  return mArguments;
}

vector<const Model*> StaticMethod::getThrownExceptions() const {
  return mThrownExceptions;
}

string StaticMethod::getTypeName() const {
  return mObjectType->getTypeName() + "." + getName();
}

FunctionType* StaticMethod::getLLVMType(IRGenerationContext& context) const {
  return IMethodDescriptor::getLLVMFunctionType(context, this, mObjectType, mLine);
}

bool StaticMethod::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

bool StaticMethod::canAutoCastTo(IRGenerationContext& context, const IType *toType) const {
  return false;
}

Value* StaticMethod::castTo(IRGenerationContext& context,
                            Value* fromValue,
                            const IType* toType,
                            int line) const {
  return NULL;
}

bool StaticMethod::isPrimitive() const {
  return false;
}

bool StaticMethod::isOwner() const {
  return false;
}

bool StaticMethod::isReference() const {
  return false;
}

bool StaticMethod::isArray() const {
  return false;
}

bool StaticMethod::isFunction() const {
  return true;
}

bool StaticMethod::isPackage() const {
  return false;
}

bool StaticMethod::isController() const {
  return false;
}

bool StaticMethod::isInterface() const {
  return false;
}

bool StaticMethod::isModel() const {
  return false;
}

bool StaticMethod::isNode() const {
  return false;
}

bool StaticMethod::isNative() const {
  return false;
}

bool StaticMethod::isPointer() const {
  return false;
}

bool StaticMethod::isImmutable() const {
  return false;
}

Function* StaticMethod::defineFunction(IRGenerationContext& context) const {
  return IMethod::defineFunction(context, mObjectType, this);
}

void StaticMethod::generateIR(IRGenerationContext& context) const {
  string functionName = IMethodCall::translateObjectMethodToLLVMFunctionName(mObjectType, mName);
  Function* function = context.getModule()->getFunction(functionName);
  assert(function != NULL);

  Scopes& scopes = context.getScopes();
  
  scopes.pushScope();
  scopes.setReturnType(mReturnType);
  BasicBlock* basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry", function, 0);
  context.setBasicBlock(basicBlock);
  
  defineCurrentMethodNameVariable(context, mName);
  createArguments(context, function);
  Composer::pushCallStack(context, mLine);
  mCompoundStatement->generateIR(context);
  IMethod::maybeAddImpliedVoidReturn(context, this, mLine);
  IMethod::checkForUnhandledExceptions(context, this, mLine);
  
  scopes.popScope(context, mLine);
}

void StaticMethod::createArguments(IRGenerationContext& context, Function* function) const {
  Function::arg_iterator llvmFunctionArguments = function->arg_begin();
  llvm::Argument *llvmFunctionArgument = &*llvmFunctionArguments;
  llvmFunctionArgument->setName(ThreadExpression::THREAD);
  llvmFunctionArguments++;
  llvmFunctionArgument = &*llvmFunctionArguments;
  llvmFunctionArgument->setName(ThreadExpression::CALL_STACK);
  llvmFunctionArguments++;
  for (const Argument* methodArgument : mArguments) {
    llvmFunctionArgument = &*llvmFunctionArguments;
    llvmFunctionArgument->setName(methodArgument->getName());
    llvmFunctionArguments++;
  }
  
  llvmFunctionArguments = function->arg_begin();
  IMethod::storeSystemArgumentValue(context,
                                    ThreadExpression::THREAD,
                                    context.getInterface(Names::getThreadInterfaceFullName(),
                                                         mLine),
                                    &*llvmFunctionArguments);
  llvmFunctionArguments++;
  IMethod::storeSystemArgumentValue(context,
                                    ThreadExpression::CALL_STACK,
                                    context.getController(Names::getCallStackControllerFullName(),
                                                          mLine),
                                    &*llvmFunctionArguments);
  llvmFunctionArguments++;
  for (const Argument* methodArgument : mArguments) {
    IMethod::storeArgumentValue(context,
                                methodArgument->getName(),
                                methodArgument->getType(),
                                &*llvmFunctionArguments,
                                mMethodQualifiers->getLine());
    llvmFunctionArguments++;
  }
}

int StaticMethod::getLine() const {
  return mLine;
}

bool StaticMethod::isConstant() const {
  return false;
}

bool StaticMethod::isField() const {
  return false;
}

bool StaticMethod::isMethod() const {
  return false;
}

bool StaticMethod::isStaticMethod() const {
  return true;
}

bool StaticMethod::isMethodSignature() const {
  return false;
}

bool StaticMethod::isLLVMFunction() const {
  return false;
}

bool StaticMethod::isOverride() const {
  return false;
}

MethodQualifiers* StaticMethod::getMethodQualifiers() const {
  return mMethodQualifiers;
}

const IObjectType* StaticMethod::getParentObject() const {
  return mObjectType;
}

void StaticMethod::printToStream(IRGenerationContext& context, iostream& stream) const {
  IMethodDescriptor::printDescriptorToStream(this, stream);
}

void StaticMethod::createLocalVariable(IRGenerationContext& context,
                                       string name,
                                       int line) const {
  assert(false);
}

void StaticMethod::createFieldVariable(IRGenerationContext& context,
                                       string name,
                                       const IConcreteObjectType* object,
                                       int line) const {
  assert(false);
}

void StaticMethod::createParameterVariable(IRGenerationContext& context,
                                           string name,
                                           Value* value,
                                           int line) const {
  assert(false);
}

const wisey::ArrayType* StaticMethod::getArrayType(IRGenerationContext& context, int line) const {
  ArrayType::reportNonArrayType(context, line);
  throw 1;
}

Instruction* StaticMethod::inject(IRGenerationContext& context,
                                  const InjectionArgumentList injectionArgumentList,
                                  int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
