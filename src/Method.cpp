//
//  Method.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/CompoundStatement.hpp"
#include "yazyk/LocalStackVariable.hpp"
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

AccessSpecifier Method::getAccessSpecifier() const {
  return mAccessSpecifier;
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
  BasicBlock* currentBlock = context.getBasicBlock();
  if(currentBlock->size() == 0 || !ReturnInst::classof(&currentBlock->back())) {
    ReturnInst::Create(context.getLLVMContext(), NULL, currentBlock);
  }
}
