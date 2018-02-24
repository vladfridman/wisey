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

TypeKind Method::getTypeKind() const {
  return TypeKind::FUNCTION_TYPE;
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

bool Method::isOwner() const {
  return false;
}

bool Method::isReference() const {
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
  llvmFunctionArgument->setName("this");
  llvmFunctionArguments++;
  llvmFunctionArgument = &*llvmFunctionArguments;
  llvmFunctionArgument->setName(ThreadExpression::THREAD);
  llvmFunctionArguments++;
  for (MethodArgument* methodArgument : mArguments) {
    llvmFunctionArgument = &*llvmFunctionArguments;
    llvmFunctionArgument->setName(methodArgument->getName());
    llvmFunctionArguments++;
  }
  
  llvmFunctionArguments = function->arg_begin();
  IMethod::storeArgumentValue(context, "this", mObjectType, &*llvmFunctionArguments);
  llvmFunctionArguments++;
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

ObjectElementType Method::getObjectElementType() const {
  return OBJECT_ELEMENT_METHOD;
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
