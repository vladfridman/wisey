//
//  MethodDeclaration.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "yazyk/LocalStackVariable.hpp"
#include "yazyk/MethodDeclaration.hpp"
#include "yazyk/ModelTypeSpecifier.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* MethodDeclaration::generateIR(IRGenerationContext& context, Model* model) const {
  Scopes& scopes = context.getScopes();
  VariableList::const_iterator iterator;
  
  Function* function = createFunction(context, model);
  
  if (strcmp(mId.getName().c_str(), "main") == 0) {
    context.setMainFunction(function);
  }

  BasicBlock *bblock = BasicBlock::Create(context.getLLVMContext(), "entry", function, 0);
  context.setBasicBlock(bblock);
  
  context.getScopes().pushScope();
  createArguments(context, function, model);
  mCompoundStatement.generateIR(context);
  scopes.popScope(context.getBasicBlock());
  
  addImpliedVoidReturn(context);
  
  return function;
}

Function* MethodDeclaration::createFunction(IRGenerationContext& context,
                                            Model* model = NULL) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Type*> argumentTypes;
  if (model != NULL) {
    argumentTypes.push_back(model->getLLVMType(llvmContext));
  }
  for (VariableList::const_iterator iterator = mArguments.begin();
       iterator != mArguments.end();
       iterator++) {
    IType* type = (**iterator).getTypeSpecifier().getType(context);
    argumentTypes.push_back(type->getLLVMType(llvmContext));
  }
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  FunctionType *ftype = FunctionType::get(mTypeSpecifier.getType(context)->getLLVMType(llvmContext),
                                          argTypesArray,
                                          false);
  return Function::Create(ftype,
                          GlobalValue::InternalLinkage,
                          mId.getName().c_str(),
                          context.getModule());
}

void MethodDeclaration::createArguments(IRGenerationContext& context,
                                        Function* function,
                                        Model* model) const {
  Function::arg_iterator arguments = function->arg_begin();
  if (model != NULL) {
    llvm::Argument *argument = &*arguments;
    argument->setName("this");
    arguments++;
  }
  for (VariableList::const_iterator iterator = mArguments.begin();
       iterator != mArguments.end();
       iterator++) {
    llvm::Argument *argument = &*arguments;
    argument->setName((**iterator).getId().getName());
    arguments++;
  }
  
  arguments = function->arg_begin();
  if (model != NULL) {
    storeArgumentValue(context, "this", model, &*arguments);
    arguments++;
  }
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

void MethodDeclaration::storeArgumentValue(IRGenerationContext& context,
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

void MethodDeclaration::addImpliedVoidReturn(IRGenerationContext& context) const {
  BasicBlock* currentBlock = context.getBasicBlock();
  if(currentBlock->size() == 0 || !ReturnInst::classof(&currentBlock->back())) {
    ReturnInst::Create(context.getLLVMContext(), NULL, currentBlock);
  }
}

Method* MethodDeclaration::getMethod(IRGenerationContext& context) const {
  vector<MethodArgument*> arguments;
  
  for (VariableList::const_iterator iterator = mArguments.begin();
       iterator != mArguments.end();
       iterator++) {
    IType* type = (**iterator).getTypeSpecifier().getType(context);
    string name = (**iterator).getId().getName();
    MethodArgument* methodArgument = new MethodArgument(type, name);
    arguments.push_back(methodArgument);
  }
  
  IType* returnType = mTypeSpecifier.getType(context);
  
  return new Method(mId.getName(), returnType, arguments);
}
