//
//  MethodCall.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>
#include <llvm/IR/TypeBuilder.h>

#include "yazyk/Identifier.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/MethodCall.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Function* MethodCall::declarePrintf(IRGenerationContext& context) {
  FunctionType *printfType = TypeBuilder<int(char *, ...), false>::get(context.getLLVMContext());
  
  Function *function = cast<Function>(
    context.getModule()->getOrInsertFunction(
      "printf",
      printfType,
      AttributeSet().addAttribute(context.getLLVMContext(), 1U, Attribute::NoAlias)));
  
  return function;
}

Value* MethodCall::generateIR(IRGenerationContext& context) {
  Function *function = context.getModule()->getFunction(mId.getName().c_str());
  if (function == NULL && mId.getName().compare("printf") != 0) {
    Log::e(string("no such function ") + mId.getName());
    exit(1);
  }
  if (function == NULL) {
    function = declarePrintf(context);
  }
  vector<Value*> args;
  ExpressionList::const_iterator it;
  for (it = mArguments.begin(); it != mArguments.end(); it++) {
    args.push_back((**it).generateIR(context));
  }
  string resultName = function->getReturnType()->isVoidTy() ? "" : "call";

  return CallInst::Create(function, args, resultName, context.currentBlock());
}
