//
//  FunctionCall.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>
#include <llvm/IR/TypeBuilder.h>

#include "yazyk/FunctionCall.hpp"
#include "yazyk/IntrinsicFunctions.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/Log.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* FunctionCall::generateIR(IRGenerationContext& context) const {
  Function *function = context.getModule()->getFunction(mFunctionName.c_str());
  if (function == NULL && mFunctionName.compare("printf") != 0) {
    Log::e(string("no such function ") + mFunctionName);
    exit(1);
  }
  if (function == NULL) {
    function = IntrinsicFunctions::getPrintfFunction(context);
  }
  vector<Value*> args;
  ExpressionList::const_iterator it;
  for (it = mArguments.begin(); it != mArguments.end(); it++) {
    args.push_back((**it).generateIR(context));
  }
  string resultName = function->getReturnType()->isVoidTy() ? "" : "call";

  return CallInst::Create(function, args, resultName, context.getBasicBlock());
}

IType* FunctionCall::getType(IRGenerationContext& context) const {
  return context.getGlobalFunctionType(mFunctionName);
}

void FunctionCall::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Function call result ownership release is not implemented");
  exit(1);
}
