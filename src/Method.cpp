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

Function* Method::defineFunction(IRGenerationContext& context, const IObjectType* object) {
  mFunction = IMethod::defineFunction(context, object, this);
  
  return mFunction;
}

void Method::generateIR(IRGenerationContext& context, const IObjectType* objectType) const {
  assert(mFunction != NULL);
  
  Scopes& scopes = context.getScopes();

  map<string, IVariable*> clearedVariablesBefore = scopes.getClearedVariables();
  scopes.pushScope();
  scopes.setReturnType(mReturnType);
  BasicBlock* basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry", mFunction, 0);
  context.setBasicBlock(basicBlock);

  defineCurrentMethodNameVariable(context, mName);
  
  if (mThrownExceptions.size()) {
    Cleanup::generateCleanupTryCatchInfo(context, "cleanup.landing.pad");
  }
  
  createArguments(context, mFunction, objectType);
  mCompoundStatement->generateIR(context);
  
  IMethod::maybeAddImpliedVoidReturn(context, this);
  IMethod::checkForUnhandledExceptions(context, this);
  if (mThrownExceptions.size()) {
    Cleanup::generateCleanupLandingPad(context, NULL);
  }

  scopes.popScope(context);
  scopes.setClearedVariables(clearedVariablesBefore);
}

void Method::createArguments(IRGenerationContext& context,
                             Function* function,
                             const IObjectType* objectType) const {
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
  IMethod::storeArgumentValue(context, "this", objectType, &*llvmFunctionArguments);
  llvmFunctionArguments++;
  IMethod::storeArgumentValue(context,
                              "thread",
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

void Method::printToStream(IRGenerationContext& context, iostream& stream) const {
  IMethodDescriptor::printDescriptorToStream(this, stream);
}
