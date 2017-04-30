//
//  FunctionDeclaration.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/FunctionDeclaration.hpp"
#include "yazyk/IRWriter.hpp"
#include "yazyk/LocalStackVariable.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* FunctionDeclaration::generateIR(IRGenerationContext& context) const {
  Scopes& scopes = context.getScopes();
  VariableList::const_iterator iterator;
  IType* returnType = mReturnTypeSpecifier.getType(context);
  
  Function* function = createFunction(context);
  
  if (strcmp(mMethodName.c_str(), "main") == 0) {
    context.setMainFunction(function);
  }
  context.addGlobalFunction(returnType, mMethodName);
  
  BasicBlock *bblock = BasicBlock::Create(context.getLLVMContext(), "entry", function, 0);
  context.setBasicBlock(bblock);
  
  context.getScopes().pushScope();
  context.getScopes().setReturnType(mReturnTypeSpecifier.getType(context));
  createArguments(context, function);
  mCompoundStatement.generateIR(context);
  scopes.popScope(context);
  
  addImpliedVoidReturn(context);
  
  return function;
}

Function* FunctionDeclaration::createFunction(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Type*> argumentTypes;
  for (VariableList::const_iterator iterator = mArguments.begin();
       iterator != mArguments.end();
       iterator++) {
    IType* type = (**iterator).getTypeSpecifier().getType(context);
    argumentTypes.push_back(type->getLLVMType(llvmContext));
  }
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = mReturnTypeSpecifier.getType(context)->getLLVMType(llvmContext);
  FunctionType *ftype = FunctionType::get(llvmReturnType,
                                          argTypesArray,
                                          false);
  GlobalValue::LinkageTypes likage = strcmp(mMethodName.c_str(), "main")
  ? GlobalValue::InternalLinkage : GlobalValue::ExternalLinkage;
  
  return Function::Create(ftype, likage, mMethodName, context.getModule());
}

void FunctionDeclaration::createArguments(IRGenerationContext& context, Function* function) const {
  Function::arg_iterator arguments = function->arg_begin();
  for (VariableList::const_iterator iterator = mArguments.begin();
       iterator != mArguments.end();
       iterator++) {
    llvm::Argument *argument = &*arguments;
    argument->setName((**iterator).getId().getName());
    arguments++;
  }
  
  arguments = function->arg_begin();
  for (VariableList::const_iterator iterator = mArguments.begin();
       iterator != mArguments.end();
       iterator++) {
    VariableDeclaration* variableDeclaration = *iterator;
    storeArgumentValue(context,
                       variableDeclaration->getId().getName(),
                       variableDeclaration->getTypeSpecifier().getType(context),
                       &*arguments);
    arguments++;
  }
}

void FunctionDeclaration::storeArgumentValue(IRGenerationContext& context,
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

void FunctionDeclaration::addImpliedVoidReturn(IRGenerationContext& context) const {
  BasicBlock* currentBlock = context.getBasicBlock();
  if(currentBlock->size() == 0 || !ReturnInst::classof(&currentBlock->back())) {
    IRWriter::createReturnInst(context, NULL);
  }
}
