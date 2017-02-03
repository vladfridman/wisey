//
//  FunctionDeclaration.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "yazyk/FunctionDeclaration.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* FunctionDeclaration::generateIR(IRGenerationContext& context) const {
  Scopes& scopes = context.getScopes();
  vector<Type*> argTypes;
  VariableList::const_iterator it;
  for (it = mArguments.begin(); it != mArguments.end(); it++) {
    argTypes.push_back((**it).getTypeSpecifier().getLLVMType(context));
  }
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argTypes);
  FunctionType *ftype = FunctionType::get(mType.getLLVMType(context),
                                          argTypesArray,
                                          false);
  Function *function = Function::Create(ftype,
                                        GlobalValue::InternalLinkage,
                                        mId.getName().c_str(),
                                        context.getModule());
  if (strcmp(mId.getName().c_str(), "main") == 0) {
    context.setMainFunction(function);
  }
  Function::arg_iterator args = function->arg_begin();
  for (it = mArguments.begin(); it != mArguments.end(); it++) {
    Argument *arg = &*args;
    arg->setName((**it).getId().getName());
  }
  BasicBlock *bblock = BasicBlock::Create(context.getLLVMContext(), "entry", function, 0);
  context.setBasicBlock(bblock);
  scopes.pushScope();
  
  args = function->arg_begin();
  for (it = mArguments.begin(); it != mArguments.end(); it++) {
    Value *value = &*args;
    string newName = (**it).getId().getName() + ".param";
    AllocaInst *alloc = new AllocaInst((**it).getTypeSpecifier().getLLVMType(context),
                                       newName,
                                       bblock);
    value = new StoreInst(value, alloc, bblock);
    scopes.setStackVariable((**it).getId().getName(), alloc);
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

