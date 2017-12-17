//
//  StaticMethod.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayElementType.hpp"
#include "wisey/Cleanup.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodArgument.hpp"
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
mLine(line),
mArrayElementType(new ArrayElementType(this)) {
}


StaticMethod::~StaticMethod() {
  delete mArrayElementType;
  for (MethodArgument* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
  // mCompoundStatement is deleted with MethodDeclaration
}

bool StaticMethod::isStatic() const {
  return true;
}

string StaticMethod::getName() const {
  return mName;
}

AccessLevel StaticMethod::getAccessLevel() const {
  return mAccessLevel;
}

const IType* StaticMethod::getReturnType() const {
  return mReturnType;
}

vector<MethodArgument*> StaticMethod::getArguments() const {
  return mArguments;
}

vector<const Model*> StaticMethod::getThrownExceptions() const {
  return mThrownExceptions;
}

string StaticMethod::getTypeName() const {
  return mObjectType->getTypeName() + "." + getName();
}

FunctionType* StaticMethod::getLLVMType(IRGenerationContext& context) const {
  return IMethodDescriptor::getLLVMFunctionType(context, this, mObjectType);
}

TypeKind StaticMethod::getTypeKind() const {
  return TypeKind::FUNCTION_TYPE;
}

bool StaticMethod::canCastTo(const IType* toType) const {
  return false;
}

bool StaticMethod::canAutoCastTo(const IType *toType) const {
  return false;
}

Value* StaticMethod::castTo(IRGenerationContext& context,
                            Value* fromValue,
                            const IType* toType,
                            int line) const {
  return NULL;
}

const ArrayElementType* StaticMethod::getArrayElementType() const {
  return mArrayElementType;
}

Function* StaticMethod::defineFunction(IRGenerationContext& context) {
  mFunction = IMethod::defineFunction(context, mObjectType, this);
  
  return mFunction;
}

void StaticMethod::generateIR(IRGenerationContext& context) const {
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

void StaticMethod::createArguments(IRGenerationContext& context, Function* function) const {
  Function::arg_iterator llvmFunctionArguments = function->arg_begin();
  llvm::Argument *llvmFunctionArgument = &*llvmFunctionArguments;
  llvmFunctionArgument->setName(ThreadExpression::THREAD);
  llvmFunctionArguments++;
  for (MethodArgument* methodArgument : mArguments) {
    llvmFunctionArgument = &*llvmFunctionArguments;
    llvmFunctionArgument->setName(methodArgument->getName());
    llvmFunctionArguments++;
  }
  
  llvmFunctionArguments = function->arg_begin();
  IMethod::storeArgumentValue(context,
                              ThreadExpression::THREAD,
                              context.getController(Names::getThreadControllerFullName()),
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

ObjectElementType StaticMethod::getObjectElementType() const {
  return OBJECT_ELEMENT_STATIC_METHOD;
}

const IObjectType* StaticMethod::getObjectType() const {
  return mObjectType;
}

void StaticMethod::printToStream(IRGenerationContext& context, iostream& stream) const {
  IMethodDescriptor::printDescriptorToStream(this, stream);
}
