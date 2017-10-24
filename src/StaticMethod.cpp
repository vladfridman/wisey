//
//  StaticMethod.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

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

StaticMethod::~StaticMethod() {
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

Function* StaticMethod::defineFunction(IRGenerationContext& context, const IObjectType* object) {
  mFunction = IMethod::defineFunction(context, object, this);
  
  return mFunction;
}

void StaticMethod::generateIR(IRGenerationContext& context, const IObjectType* objectType) const {
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

void StaticMethod::createArguments(IRGenerationContext& context,
                                   Function* function,
                                   const IObjectType* objectType) const {
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
  return OBJECT_ELEMENT_METHOD;
}

void StaticMethod::printToStream(IRGenerationContext& context, iostream& stream) const {
  IMethodDescriptor::printDescriptorToStream(this, stream);
}
