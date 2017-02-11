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
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* MethodDeclaration::generateIR(IRGenerationContext& context) const {
  Scopes& scopes = context.getScopes();
  vector<Type*> argumentTypes;
  VariableList::const_iterator iterator;
  LLVMContext& llvmContext = context.getLLVMContext();
  for (iterator = mArguments.begin(); iterator != mArguments.end(); iterator++) {
    IType* type = (**iterator).getTypeSpecifier().getType(context);
    argumentTypes.push_back(type->getLLVMType(llvmContext));
  }
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  FunctionType *ftype = FunctionType::get(mTypeSpecifier.getType(context)->getLLVMType(llvmContext),
                                          argTypesArray,
                                          false);
  Function *function = Function::Create(ftype,
                                        GlobalValue::InternalLinkage,
                                        mId.getName().c_str(),
                                        context.getModule());
  if (strcmp(mId.getName().c_str(), "main") == 0) {
    context.setMainFunction(function);
  }
  Function::arg_iterator arguments = function->arg_begin();
  for (iterator = mArguments.begin(); iterator != mArguments.end(); iterator++) {
    llvm::Argument *argument = &*arguments;
    argument->setName((**iterator).getId().getName());
    arguments++;
  }
  BasicBlock *bblock = BasicBlock::Create(context.getLLVMContext(), "entry", function, 0);
  context.setBasicBlock(bblock);
  scopes.pushScope();
  
  arguments = function->arg_begin();
  for (iterator = mArguments.begin(); iterator != mArguments.end(); iterator++) {
    Value *value = &*arguments;
    string newName = (**iterator).getId().getName() + ".param";
    IType* type = (**iterator).getTypeSpecifier().getType(context);
    AllocaInst *alloc = new AllocaInst(type->getLLVMType(llvmContext),
                                       newName,
                                       bblock);
    value = new StoreInst(value, alloc, bblock);
    IVariable* variable = new LocalStackVariable((**iterator).getId().getName(), type, alloc);
    scopes.setVariable(variable);
    arguments++;
  }
  
  mCompoundStatement.generateIR(context);
  
  /** Add an implied void return */
  BasicBlock* currentBlock = context.getBasicBlock();
  if(currentBlock->size() == 0 || !ReturnInst::classof(&currentBlock->back())) {
    ReturnInst::Create(context.getLLVMContext(), NULL, currentBlock);
  }

  scopes.popScope(currentBlock);
  return function;
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
