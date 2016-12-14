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
#include "yazyk/TypeIdentifier.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* FunctionDeclaration::generateIR(IRGenerationContext& context) {
  vector<Type*> argTypes;
  VariableList::const_iterator it;
  for (it = mArguments.begin(); it != mArguments.end(); it++) {
    argTypes.push_back(TypeIdentifier::typeOf(context.getLLVMContext(), (**it).type));
  }
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argTypes);
  FunctionType *ftype = FunctionType::get(TypeIdentifier::typeOf(context.getLLVMContext(), mType),
                                          argTypesArray,
                                          false);
  Function *function = Function::Create(ftype,
                                        GlobalValue::InternalLinkage,
                                        mId.name.c_str(),
                                        context.getModule());
  if (strcmp(mId.name.c_str(), "main") == 0) {
    context.setMainFunction(function);
  }
  Function::arg_iterator args = function->arg_begin();
  for (it = mArguments.begin(); it != mArguments.end(); it++) {
    Argument *arg = &*args;
    arg->setName((**it).id.name);
  }
  BasicBlock *bblock = BasicBlock::Create(context.getLLVMContext(), "entry", function, 0);
  
  context.pushBlock(bblock);
  
  args = function->arg_begin();
  for (it = mArguments.begin(); it != mArguments.end(); it++) {
    Value *value = &*args;
    string newName = (**it).id.name + ".param";
    AllocaInst *alloc = new AllocaInst(TypeIdentifier::typeOf(context.getLLVMContext(),
                                                              (**it).type),
                                       newName,
                                       bblock);
    value = new StoreInst(value, alloc, bblock);
    context.locals()[(**it).id.name] = alloc;
  }
  
  mBlock.generateIR(context);
  
  context.popBlock();
  return function;
}

