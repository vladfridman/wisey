//
//  Method.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/CompoundStatement.hpp"
#include "yazyk/IRWriter.hpp"
#include "yazyk/LocalStackVariable.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/Method.hpp"
#include "yazyk/MethodArgument.hpp"
#include "yazyk/MethodCall.hpp"
#include "yazyk/Model.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

string Method::getName() const {
  return mName;
}

AccessLevel Method::getAccessLevel() const {
  return mAccessLevel;
}

IType* Method::getReturnType() const {
  return mReturnType;
}

vector<MethodArgument*> Method::getArguments() const {
  return mArguments;
}

unsigned long Method::getIndex() const {
  return mIndex;
}

vector<IType*> Method::getThrownExceptions() const {
  return mThrownExceptions;
}

Function* Method::defineFunction(IRGenerationContext& context,
                                 IObjectWithMethodsType* objectType) const {
  FunctionType *ftype = IMethodDescriptor::getLLVMFunctionType((IMethodDescriptor*) this,
                                                               context,
                                                               objectType);
  string functionName = MethodCall::translateObjectMethodToLLVMFunctionName(objectType, mName);
  
  return Function::Create(ftype,
                          GlobalValue::InternalLinkage,
                          functionName,
                          context.getModule());
}

void Method::generateIR(IRGenerationContext& context,
                        Function* function,
                        IObjectWithMethodsType* objectType) const {
  Scopes& scopes = context.getScopes();
  
  BasicBlock *bblock = BasicBlock::Create(context.getLLVMContext(), "entry", function, 0);
  context.setBasicBlock(bblock);
  
  context.getScopes().pushScope();
  context.getScopes().setReturnType(mReturnType);
  createArguments(context, function, objectType);
  mCompoundStatement->generateIR(context);
  
  checkForUnhandledExceptions(context);
  scopes.popScope(context);
  
  maybeAddImpliedVoidReturn(context);
}

void Method::createArguments(IRGenerationContext& context,
                             Function* function,
                             IObjectWithMethodsType* objectType) const {
  Function::arg_iterator llvmFunctionArguments = function->arg_begin();
  llvm::Argument *llvmFunctionArgument = &*llvmFunctionArguments;
  llvmFunctionArgument->setName("this");
  llvmFunctionArguments++;
  for (MethodArgument* methodArgument : mArguments) {
    llvmFunctionArgument = &*llvmFunctionArguments;
    llvmFunctionArgument->setName(methodArgument->getName());
    llvmFunctionArguments++;
  }
  
  llvmFunctionArguments = function->arg_begin();
  storeArgumentValue(context, "this", objectType, &*llvmFunctionArguments);
  llvmFunctionArguments++;
  for (MethodArgument* methodArgument : mArguments) {
    storeArgumentValue(context,
                       methodArgument->getName(),
                       methodArgument->getType(),
                       &*llvmFunctionArguments);
    llvmFunctionArguments++;
  }
}

void Method::storeArgumentValue(IRGenerationContext& context,
                                std::string variableName,
                                IType* variableType,
                                llvm::Value* variableValue) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  string newName = variableName + ".param";
  AllocaInst *alloc = new AllocaInst(variableType->getLLVMType(llvmContext),
                                     newName,
                                     context.getBasicBlock());
  new StoreInst(variableValue, alloc, context.getBasicBlock());
  IVariable* variable = new LocalStackVariable(variableName, variableType, alloc);
  context.getScopes().setVariable(variable);
}

void Method::maybeAddImpliedVoidReturn(IRGenerationContext& context) const {
  if (!context.getBasicBlock()->getTerminator()) {
    IRWriter::createReturnInst(context, NULL);
    return;
  }
}

void Method::checkForUnhandledExceptions(IRGenerationContext& context) const {
  Scope* scope = context.getScopes().getScope();
  map<string, IType*> exceptions = scope->getExceptions();
  if (exceptions.size() == 0) {
    return;
  }
  
  for (IType* thrownException : mThrownExceptions) {
    scope->removeException(thrownException);
  }
  
  exceptions = scope->getExceptions();
  bool hasUnhangledExceptions = false;
  for (map<string, IType*>::const_iterator iterator = exceptions.begin();
       iterator != exceptions.end();
       iterator++) {
    Log::e("Method " + getName() + " neither handles the exception " + iterator->first +
           " nor throws it");
    hasUnhangledExceptions = true;
  }
  
  if (hasUnhangledExceptions) {
    exit(1);
  }
}
