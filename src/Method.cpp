//
//  Method.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Cleanup.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/Model.hpp"
#include "wisey/Names.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Method::Method(const IObjectType* objectType,
               string name,
               AccessLevel accessLevel,
               const IType* returnType,
               vector<MethodArgument*> arguments,
               vector<const Model*> thrownExceptions,
               CompoundStatement* compoundStatement,
               int line) :
mObjectType(objectType),
mName(name),
mAccessLevel(accessLevel),
mReturnType(returnType),
mArguments(arguments),
mThrownExceptions(thrownExceptions),
mCompoundStatement(compoundStatement),
mFunction(NULL),
mLine(line) { }

Method::~Method() {
  for (MethodArgument* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
  // mCompoundStatement is deleted with MethodDeclaration
}

bool Method::isStatic() const {
  return false;
}

string Method::getName() const {
  return mName;
}

AccessLevel Method::getAccessLevel() const {
  return mAccessLevel;
}

const IType* Method::getReturnType() const {
  return mReturnType;
}

vector<MethodArgument*> Method::getArguments() const {
  return mArguments;
}

vector<const Model*> Method::getThrownExceptions() const {
  return mThrownExceptions;
}

string Method::getTypeName() const {
  return mObjectType->getTypeName() + "." + getName();
}

FunctionType* Method::getLLVMType(IRGenerationContext& context) const {
  return IMethodDescriptor::getLLVMFunctionType(context, this, mObjectType);
}

bool Method::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

bool Method::canAutoCastTo(IRGenerationContext& context, const IType *toType) const {
  return false;
}

Value* Method::castTo(IRGenerationContext& context,
                      Value* fromValue,
                      const IType* toType,
                      int line) const {
  return NULL;
}

bool Method::isPrimitive() const {
  return false;
}

bool Method::isOwner() const {
  return false;
}

bool Method::isReference() const {
  return false;
}

bool Method::isArray() const {
  return false;
}

bool Method::isFunction() const {
  return true;
}

bool Method::isPackage() const {
  return false;
}

bool Method::isController() const {
  return false;
}

bool Method::isInterface() const {
  return false;
}

bool Method::isModel() const {
  return false;
}

bool Method::isNode() const {
  return false;
}

bool Method::isThread() const {
  return false;
}

bool Method::isNative() const {
  return false;
}

Function* Method::defineFunction(IRGenerationContext& context) {
  mFunction = IMethod::defineFunction(context, mObjectType, this);
  
  return mFunction;
}

void Method::generateIR(IRGenerationContext& context) const {
  assert(mFunction != NULL);
  
  Scopes& scopes = context.getScopes();

  scopes.pushScope();
  scopes.setReturnType(mReturnType);
  BasicBlock* basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry", mFunction, 0);
  context.setBasicBlock(basicBlock);

  defineCurrentMethodNameVariable(context, mName);
  
  createArguments(context, mFunction);
  mCompoundStatement->generateIR(context);
  
  IMethod::maybeAddImpliedVoidReturn(context, this, mLine);
  IMethod::checkForUnhandledExceptions(context, this);

  scopes.popScope(context, mLine);
}

void Method::createArguments(IRGenerationContext& context, Function* function) const {
  Function::arg_iterator llvmFunctionArguments = function->arg_begin();
  llvm::Argument *llvmFunctionArgument = &*llvmFunctionArguments;
  llvmFunctionArgument->setName(IObjectType::THIS);
  llvmFunctionArguments++;
  llvmFunctionArgument = &*llvmFunctionArguments;
  llvmFunctionArgument->setName(ThreadExpression::THREAD);
  llvmFunctionArguments++;
  llvmFunctionArgument = &*llvmFunctionArguments;
  llvmFunctionArgument->setName(ThreadExpression::CALL_STACK);
  llvmFunctionArguments++;
  for (MethodArgument* methodArgument : mArguments) {
    llvmFunctionArgument = &*llvmFunctionArguments;
    llvmFunctionArgument->setName(methodArgument->getName());
    llvmFunctionArguments++;
  }
  
  llvmFunctionArguments = function->arg_begin();
  IMethod::storeSystemArgumentValue(context,
                                    IObjectType::THIS,
                                    mObjectType,
                                    &*llvmFunctionArguments);
  llvmFunctionArguments++;
  IMethod::storeSystemArgumentValue(context,
                                    ThreadExpression::THREAD,
                                    context.getInterface(Names::getThreadInterfaceFullName()),
                                    &*llvmFunctionArguments);
  llvmFunctionArguments++;
  IMethod::storeSystemArgumentValue(context,
                                    ThreadExpression::CALL_STACK,
                                    context.getController(Names::getCallStackControllerFullName()),
                                    &*llvmFunctionArguments);
  llvmFunctionArguments++;
  for (MethodArgument* methodArgument : mArguments) {
    IMethod::storeArgumentValue(context,
                                methodArgument->getName(),
                                methodArgument->getType(),
                                &*llvmFunctionArguments);
    llvmFunctionArguments++;
  }
}

bool Method::isConstant() const {
  return false;
}

bool Method::isField() const {
  return false;
}

bool Method::isMethod() const {
  return true;
}

bool Method::isStaticMethod() const {
  return false;
}

bool Method::isMethodSignature() const {
  return false;
}

const IObjectType* Method::getObjectType() const {
  return mObjectType;
}

void Method::printToStream(IRGenerationContext& context, iostream& stream) const {
  IMethodDescriptor::printDescriptorToStream(this, stream);
}

void Method::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void Method::createFieldVariable(IRGenerationContext& context,
                                 string name,
                                 const IConcreteObjectType* object) const {
  assert(false);
}

void Method::createParameterVariable(IRGenerationContext& context,
                                     string name,
                                     Value* value) const {
  assert(false);
}

const wisey::ArrayType* Method::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}
