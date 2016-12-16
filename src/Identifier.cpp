//
//  Identifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include "llvm/IR/TypeBuilder.h"

#include "yazyk/Identifier.hpp"
#include "yazyk/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* Identifier::generateIR(IRGenerationContext& context) {
  if (context.locals().find(name) == context.locals().end()) {
    cerr << "undeclared variable " << name << endl;
    return NULL;
  }
  return new LoadInst(context.locals()[name], mVariableName, context.currentBlock());
}

Value * TypeSpecifier::generateIR(IRGenerationContext &context) {
  return NULL;
}

Function* MethodCall::declarePrintf(IRGenerationContext& context) {
  FunctionType *printf_type = TypeBuilder<int(char *, ...), false>::get(context.getLLVMContext());
  
  Function *function = cast<Function>(
    context.getModule()->getOrInsertFunction("printf",
      printf_type,
      AttributeSet().addAttribute(context.getLLVMContext(), 1U, Attribute::NoAlias)));

  return function;
}
